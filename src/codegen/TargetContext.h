#ifndef CODEGEN_TARGETCONTEXT_H_
#define CODEGEN_TARGETCONTEXT_H_

#include <memory>
#include <string>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Target/TargetMachine.h>

namespace codegen {

class TargetContext {
public:
    TargetContext();
    ~TargetContext();
    TargetContext(const TargetContext&) = delete;
    TargetContext& operator=(const TargetContext&) = delete;

    llvm::LLVMContext& getLLVMContext() { return llvmContext; }
    llvm::TargetMachine& getTargetMachine() { return *targetMachine; }
    const std::string& getTargetTriple() const { return targetTriple; }

private:
    llvm::LLVMContext llvmContext;
    std::string targetTriple;
    std::unique_ptr<llvm::TargetMachine> targetMachine;
};

}
#endif
