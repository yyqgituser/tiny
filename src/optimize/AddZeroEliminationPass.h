#ifndef OPTIMIZE_ADDZEROELIMINATIONPASS_H_
#define OPTIMIZE_ADDZEROELIMINATIONPASS_H_

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Function.h>

namespace optimize {

// Algebraic identity: replaces  x + 0  and  0 + x  with  x, removing the add.
// An experimental custom LLVM pass (research playground for backend optimization).
struct AddZeroEliminationPass : public llvm::PassInfoMixin<AddZeroEliminationPass> {
    llvm::PreservedAnalyses run(llvm::Function& f, llvm::FunctionAnalysisManager& fam);
};

}

#endif
