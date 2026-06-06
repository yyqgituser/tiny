#ifndef LINKER_LINKEDIMAGE_H_
#define LINKER_LINKEDIMAGE_H_

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include "linker/ImportEntry.h"

namespace linker {

// In-memory linked image produced by Linker, consumed directly by Loader.
// No persistence (no HMX file). codeBytes is already relocated; dataBytes holds
// .data/.rdata content followed by 8-byte GOT slots for imports.
struct LinkedImage {
    std::vector<uint8_t> codeBytes;
    std::vector<uint8_t> dataBytes;
    std::vector<ImportEntry> imports;
    // Defined symbol name -> image offset (relative to code base). Used to find
    // the entry point (e.g. "main").
    std::unordered_map<std::string, uint32_t> symbolOffsets;
};

}
#endif
