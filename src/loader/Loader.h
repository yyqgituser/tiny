#ifndef LOADER_LOADER_H_
#define LOADER_LOADER_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include "linker/LinkedImage.h"

namespace loader {

class Loader {
public:
    Loader();
    ~Loader();
    Loader(const Loader&) = delete;
    Loader& operator=(const Loader&) = delete;

    void registerNative(const std::string& fullName, void* funcPtr);
    void load(const linker::LinkedImage& image);
    void* findSymbol(const std::string& name);

    void* getBaseAddr() const { return baseAddr; }
    void* getDataBase() const;

private:
    void* baseAddr = nullptr;
    std::size_t codeSize = 0;
    std::size_t dataSize = 0;
    std::size_t totalSize = 0;
    const linker::LinkedImage* image = nullptr;
    std::unordered_map<std::string, void*> nativeRegistry;

    void allocateMemory();
    void copyContent();
    void patchGotSlots();
    void protectCodeRX();
};

}
#endif
