#include "loader/Loader.h"

#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>

#include <Windows.h>

using namespace std;

namespace loader {

// MUST stay in sync with Linker's SECTION_ALIGNMENT.
static constexpr size_t SECTION_ALIGNMENT = 4096;

static size_t alignUp(size_t value, size_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

static string formatLastError(const char* prefix) {
    DWORD err = GetLastError();
    ostringstream oss;
    oss << prefix << " (GetLastError=" << err << ")";
    return oss.str();
}

Loader::Loader() = default;

Loader::~Loader() {
    if (baseAddr) {
        VirtualFree(baseAddr, 0, MEM_RELEASE);
        baseAddr = nullptr;
    }
}

void Loader::registerNative(const string& fullName, void* funcPtr) {
    nativeRegistry[fullName] = funcPtr;
}

void Loader::load(const linker::LinkedImage& img) {
    image = &img;
    codeSize = img.codeBytes.size();
    dataSize = img.dataBytes.size();
    allocateMemory();
    copyContent();
    patchGotSlots();
    protectCodeRX();
}

void* Loader::getDataBase() const {
    if (!baseAddr) return nullptr;
    return static_cast<uint8_t*>(baseAddr) + alignUp(codeSize, SECTION_ALIGNMENT);
}

void Loader::allocateMemory() {
    size_t codeRegion = alignUp(codeSize, SECTION_ALIGNMENT);
    size_t dataRegion = alignUp(dataSize, SECTION_ALIGNMENT);
    totalSize = codeRegion + dataRegion;
    baseAddr = VirtualAlloc(nullptr, totalSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!baseAddr) {
        throw runtime_error(formatLastError("Loader: VirtualAlloc failed"));
    }
}

void Loader::copyContent() {
    memcpy(baseAddr, image->codeBytes.data(), codeSize);
    uint8_t* dataBase = static_cast<uint8_t*>(getDataBase());
    memcpy(dataBase, image->dataBytes.data(), dataSize);
}

void Loader::patchGotSlots() {
    uint8_t* dataBase = static_cast<uint8_t*>(getDataBase());
    for (const auto& imp : image->imports) {
        auto it = nativeRegistry.find(imp.fullName);
        if (it == nativeRegistry.end()) {
            ostringstream oss;
            oss << "Loader: native function '" << imp.fullName << "' not registered. Registered natives:";
            if (nativeRegistry.empty()) {
                oss << " <none>";
            } else {
                for (const auto& entry : nativeRegistry) {
                    oss << " " << entry.first;
                }
            }
            throw runtime_error(oss.str());
        }
        void* fnPtr = it->second;
        memcpy(dataBase + imp.dataOffset, &fnPtr, sizeof(void*));
    }
}

void Loader::protectCodeRX() {
    DWORD oldProtect = 0;
    BOOL ok = VirtualProtect(baseAddr, alignUp(codeSize, SECTION_ALIGNMENT), PAGE_EXECUTE_READ, &oldProtect);
    if (!ok) {
        throw runtime_error(formatLastError("Loader: VirtualProtect failed"));
    }
}

void* Loader::findSymbol(const string& name) {
    if (!image) {
        throw runtime_error("Loader: not loaded");
    }
    auto it = image->symbolOffsets.find(name);
    if (it == image->symbolOffsets.end()) {
        throw runtime_error("Loader: symbol not found: " + name);
    }
    return static_cast<uint8_t*>(baseAddr) + it->second;
}

}
