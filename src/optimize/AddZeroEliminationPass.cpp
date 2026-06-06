#include "optimize/AddZeroEliminationPass.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/ADT/SmallVector.h>

using namespace llvm;

namespace optimize {

PreservedAnalyses AddZeroEliminationPass::run(Function& f, FunctionAnalysisManager& fam) {
    SmallVector<Instruction*, 8> toErase;
    for (BasicBlock& bb : f) {
        for (Instruction& inst : bb) {
            auto* binOp = dyn_cast<BinaryOperator>(&inst);
            if (!binOp || binOp->getOpcode() != Instruction::Add) {
                continue;
            }
            Value* lhs = binOp->getOperand(0);
            Value* rhs = binOp->getOperand(1);
            Value* replacement = nullptr;
            if (auto* c = dyn_cast<ConstantInt>(rhs)) {
                if (c->isZero()) {
                    replacement = lhs;
                }
            }
            if (auto* c = dyn_cast<ConstantInt>(lhs)) {
                if (c->isZero()) {
                    replacement = rhs;
                }
            }
            if (replacement) {
                binOp->replaceAllUsesWith(replacement);
                toErase.push_back(binOp);
            }
        }
    }
    for (Instruction* inst : toErase) {
        inst->eraseFromParent();
    }
    if (toErase.empty()) {
        return PreservedAnalyses::all();
    }
    return PreservedAnalyses::none();
}

}
