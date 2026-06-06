#include "linker/Linker.h"
#include "codegen/TargetContext.h"

#include <cstring>
#include <sstream>
#include <stdexcept>

#include <llvm/IR/Module.h>
#include <llvm/Object/COFF.h>
#include <llvm/Object/ObjectFile.h>
#include <llvm/Object/SymbolicFile.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/ADT/SmallVector.h>

using namespace llvm;
using namespace std;

namespace linker {

// POC: code/data section alignment in the loaded image. Linker computes
// data-section PC-relative displacements assuming data starts at
// align_up(codeSize, SECTION_ALIGNMENT); Loader must use the same rule.
static constexpr uint32_t SECTION_ALIGNMENT = 4096;

Linker::Linker(codegen::TargetContext& targetContext)
: targetContext(targetContext) {
}

Linker::~Linker() = default;

LinkedImage Linker::link(Module& module) {
    textBytes.clear();
    dataBytes.clear();
    symbols.clear();
    imports.clear();

    auto cofBytes = compileToCOFF(module);
    parseCOFFAndStage(cofBytes);
    applyRelocationsTo(cofBytes);
    return buildImage();
}

vector<char> Linker::compileToCOFF(Module& module) {
    TargetMachine& tm = targetContext.getTargetMachine();
    SmallVector<char, 0> buf;
    raw_svector_ostream stream(buf);
    legacy::PassManager pm;
    if (tm.addPassesToEmitFile(pm, stream, nullptr, CodeGenFileType::ObjectFile)) {
        throw runtime_error("Linker: addPassesToEmitFile failed (target cannot emit object file)");
    }
    pm.run(module);
    return vector<char>(buf.begin(), buf.end());
}

void Linker::parseCOFFAndStage(const vector<char>& bytes) {
    auto buf = MemoryBuffer::getMemBuffer(StringRef(bytes.data(), bytes.size()), "tiny.obj", false);
    auto objOrErr = object::ObjectFile::createObjectFile(buf->getMemBufferRef());
    if (!objOrErr) {
        throw runtime_error("Linker: createObjectFile failed: " + toString(objOrErr.takeError()));
    }
    auto& obj = **objOrErr;

    uint32_t dataOffsetInData = 0;
    uint32_t rdataOffsetInData = 0;

    // dataBytes layout: [.data | .rdata | GOT slots appended later, 8-byte aligned]
    // POC: .xdata/.pdata (unwind info) are dropped. fib/main are leaf-ish, no
    // exception unwinding in the 55 path, so this is acceptable for now.
    // MUST revisit when exceptions / debugger stack walking are needed.
    for (auto& s : obj.sections()) {
        auto nameOrErr = s.getName();
        if (!nameOrErr) continue;
        string name = nameOrErr->str();
        auto contentsOrErr = s.getContents();
        if (!contentsOrErr) continue;
        if (name == ".text") {
            textBytes.assign(contentsOrErr->begin(), contentsOrErr->end());
        } else if (name == ".data") {
            dataOffsetInData = static_cast<uint32_t>(dataBytes.size());
            dataBytes.insert(dataBytes.end(), contentsOrErr->begin(), contentsOrErr->end());
        } else if (name == ".rdata") {
            rdataOffsetInData = static_cast<uint32_t>(dataBytes.size());
            dataBytes.insert(dataBytes.end(), contentsOrErr->begin(), contentsOrErr->end());
        }
    }

    for (auto& sym : obj.symbols()) {
        auto nameOrErr = sym.getName();
        auto flagsOrErr = sym.getFlags();
        if (!nameOrErr || !flagsOrErr) continue;
        string name = nameOrErr->str();
        uint32_t flags = *flagsOrErr;
        if (name.empty() || name[0] == '.' || name == "@feat.00") continue;

        SymbolInfo info;
        if (flags & object::SymbolRef::SF_Undefined) {
            info.isDefined = false;
            info.sectionName = "";
            info.valueInSection = 0;
            while (dataBytes.size() % 8 != 0) {
                dataBytes.push_back(0);
            }
            uint32_t gotOffset = static_cast<uint32_t>(dataBytes.size());
            dataBytes.resize(dataBytes.size() + 8, 0);
            info.imageOffset = gotOffset;

            ImportEntry imp;
            imp.fullName = name;        // M1.0: IR name == registered native name
            imp.dataOffset = gotOffset;
            imports.push_back(move(imp));
        } else {
            info.isDefined = true;
            auto secOrErr = sym.getSection();
            auto addrOrErr = sym.getAddress();
            if (!secOrErr || !addrOrErr) continue;
            auto sec = *secOrErr;
            if (sec == obj.section_end()) continue;
            auto sn = sec->getName();
            if (!sn) continue;
            info.sectionName = sn->str();
            info.valueInSection = *addrOrErr;
            info.imageOffset = 0;
        }
        symbols[name] = info;
    }

    uint32_t codeSize = static_cast<uint32_t>(textBytes.size());
    uint32_t dataBaseInImage = (codeSize + (SECTION_ALIGNMENT - 1)) & ~(SECTION_ALIGNMENT - 1);
    for (auto& [name, info] : symbols) {
        if (!info.isDefined) {
            info.imageOffset = dataBaseInImage + info.imageOffset;
        } else if (info.sectionName == ".text") {
            info.imageOffset = static_cast<uint32_t>(info.valueInSection);
        } else if (info.sectionName == ".data") {
            info.imageOffset = dataBaseInImage + dataOffsetInData + static_cast<uint32_t>(info.valueInSection);
        } else if (info.sectionName == ".rdata") {
            info.imageOffset = dataBaseInImage + rdataOffsetInData + static_cast<uint32_t>(info.valueInSection);
        }
    }
}

void Linker::applyRelocationsTo(const vector<char>& cofBytes) {
    auto buf = MemoryBuffer::getMemBuffer(StringRef(cofBytes.data(), cofBytes.size()), "tiny.obj", false);
    auto objOrErr = object::ObjectFile::createObjectFile(buf->getMemBufferRef());
    if (!objOrErr) {
        throw runtime_error("Linker: re-parse for relocations failed: " + toString(objOrErr.takeError()));
    }
    auto& obj = **objOrErr;

    for (auto& s : obj.sections()) {
        auto nameOrErr = s.getName();
        if (!nameOrErr || nameOrErr->str() != ".text") continue;
        for (auto& r : s.relocations()) {
            uint64_t relOffset = r.getOffset();
            uint64_t relType = r.getType();
            if (relType != COFF::IMAGE_REL_AMD64_REL32) {
                auto symIt = r.getSymbol();
                string symName = "<unknown>";
                if (symIt != obj.symbol_end()) {
                    auto sn = symIt->getName();
                    if (sn) symName = sn->str();
                }
                ostringstream oss;
                oss << "Linker: unsupported relocation type " << relType
                    << " at .text offset 0x" << hex << relOffset
                    << " for symbol '" << symName << "'";
                throw runtime_error(oss.str());
            }
            auto symIt = r.getSymbol();
            if (symIt == obj.symbol_end()) {
                ostringstream oss;
                oss << "Linker: relocation at .text offset 0x" << hex << relOffset << " has no symbol";
                throw runtime_error(oss.str());
            }
            auto nameOrErrR = symIt->getName();
            if (!nameOrErrR) {
                ostringstream oss;
                oss << "Linker: relocation at .text offset 0x" << hex << relOffset << " has anonymous symbol";
                throw runtime_error(oss.str());
            }
            string symName = nameOrErrR->str();
            auto it = symbols.find(symName);
            if (it == symbols.end()) {
                ostringstream oss;
                oss << "Linker: relocation at .text offset 0x" << hex << relOffset
                    << " references unknown symbol '" << symName << "'";
                throw runtime_error(oss.str());
            }
            uint32_t targetImageOffset = it->second.imageOffset;
            uint32_t nextInstrImageOffset = static_cast<uint32_t>(relOffset + 4);
            int32_t displacement =
                static_cast<int32_t>(targetImageOffset) - static_cast<int32_t>(nextInstrImageOffset);
            uint32_t udisp = static_cast<uint32_t>(displacement);
            textBytes[relOffset + 0] = static_cast<uint8_t>(udisp & 0xFF);
            textBytes[relOffset + 1] = static_cast<uint8_t>((udisp >> 8) & 0xFF);
            textBytes[relOffset + 2] = static_cast<uint8_t>((udisp >> 16) & 0xFF);
            textBytes[relOffset + 3] = static_cast<uint8_t>((udisp >> 24) & 0xFF);
        }
        break;
    }
}

LinkedImage Linker::buildImage() {
    LinkedImage image;
    image.codeBytes = move(textBytes);
    image.dataBytes = move(dataBytes);
    image.imports = move(imports);
    for (auto& [name, info] : symbols) {
        if (info.isDefined && info.sectionName == ".text") {
            image.symbolOffsets[name] = info.imageOffset;
        }
    }
    return image;
}

}
