#include "codegen/TargetContext.h"
#include <optional>
#include <stdexcept>
#include <string>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/TargetRegistry.h>

using namespace std;
using namespace llvm;

namespace codegen {

TargetContext::TargetContext() {
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    targetTriple = "x86_64-pc-windows-msvc";
    string err;
    auto* target = TargetRegistry::lookupTarget(targetTriple, err);
    if (!target) {
        throw runtime_error("TargetContext: target lookup failed for triple '" + targetTriple + "': " + err);
    }
    TargetOptions opts;
    auto rm = optional<Reloc::Model>(Reloc::PIC_);
    targetMachine.reset(target->createTargetMachine(targetTriple, "generic", "", opts, rm));
    if (!targetMachine) {
        throw runtime_error("TargetContext: createTargetMachine failed for triple '" + targetTriple + "'");
    }
}

TargetContext::~TargetContext() = default;

}
