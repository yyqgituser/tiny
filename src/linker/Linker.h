#ifndef LINKER_LINKER_H_
#define LINKER_LINKER_H_

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include "linker/LinkedImage.h"

namespace codegen { class TargetContext; }
namespace llvm { class Module; }

namespace linker {

class Linker {
public:
    Linker(codegen::TargetContext& targetContext);
    ~Linker();

    // Compile an llvm::Module to an in-memory LinkedImage (COFF -> parse ->
    // relocate). Throws std::runtime_error on failure.
    LinkedImage link(llvm::Module& module);

private:
    codegen::TargetContext& targetContext;

    std::vector<uint8_t> textBytes;
    std::vector<uint8_t> dataBytes;     // .data/.rdata first, then GOT slots

    struct SymbolInfo {
        bool isDefined;
        std::string sectionName;        // ".text", ".data", ".rdata", or ""
        uint64_t valueInSection;
        uint32_t imageOffset;
    };
    std::unordered_map<std::string, SymbolInfo> symbols;
    std::vector<ImportEntry> imports;

    std::vector<char> compileToCOFF(llvm::Module& module);
    void parseCOFFAndStage(const std::vector<char>& bytes);
    void applyRelocationsTo(const std::vector<char>& cofBytes);
    LinkedImage buildImage();
};

}
#endif
