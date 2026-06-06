#ifndef OPTIMIZE_OPTIMIZER_H_
#define OPTIMIZE_OPTIMIZER_H_

#include <llvm/IR/Module.h>

namespace optimize {

// Runs tiny's custom optimization pipeline over a module.
// Research entry point: add custom passes here.
class Optimizer {
public:
    void run(llvm::Module& module);
};

}

#endif
