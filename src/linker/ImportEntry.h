#ifndef LINKER_IMPORTENTRY_H_
#define LINKER_IMPORTENTRY_H_

#include <cstdint>
#include <string>

namespace linker {

// One external symbol (GOT slot) the loader must fill at load time.
struct ImportEntry {
    std::string fullName;     // external symbol name, e.g. "print_int"
    uint32_t dataOffset;      // offset within LinkedImage.dataBytes of the 8-byte slot
};

}
#endif
