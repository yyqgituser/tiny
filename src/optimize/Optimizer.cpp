#include "optimize/Optimizer.h"
#include "optimize/AddZeroEliminationPass.h"

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/IR/PassManager.h>

using namespace llvm;

namespace optimize {

void Optimizer::run(Module& module) {
    LoopAnalysisManager lam;
    FunctionAnalysisManager fam;
    CGSCCAnalysisManager cgam;
    ModuleAnalysisManager mam;

    PassBuilder pb;
    pb.registerModuleAnalyses(mam);
    pb.registerCGSCCAnalyses(cgam);
    pb.registerFunctionAnalyses(fam);
    pb.registerLoopAnalyses(lam);
    pb.crossRegisterProxies(lam, fam, cgam, mam);

    ModulePassManager mpm;
    mpm.addPass(createModuleToFunctionPassAdaptor(AddZeroEliminationPass()));
    mpm.run(module, mam);
}

}
