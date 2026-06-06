#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include "TinyScanner.h"
#include "TinyTokenScanner.h"
#include "TinyParser.h"
#include "TinySyntaxNode.h"

#include "builder/TinyNodeBuilder.h"
#include "sema/Sema.h"
#include "ast/Decl.h"
#include "codegen/TargetContext.h"
#include "codegen/CodeGen.h"
#include "optimize/Optimizer.h"
#include "linker/Linker.h"
#include "loader/Loader.h"

using namespace std;

// tiny runtime natives.
extern "C" int32_t print_int(int32_t n) {
    printf("%d\n", n);
    return 0;
}

extern "C" void* tiny_malloc(int64_t size) {
    return malloc(static_cast<size_t>(size));
}

static void printUsage() {
    cerr << "Usage: tiny [-emit-llvm] [-O] <file.tiny>" << endl;
}

int main(int argc, char* argv[]) {
    setbuf(stdout, nullptr);

    bool emitLlvm = false;
    bool doOptimize = false;
    string path;
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-emit-llvm") {
            emitLlvm = true;
        } else if (arg == "-O") {
            doOptimize = true;
        } else if (!arg.empty() && arg[0] == '-') {
            cerr << "Error: unknown option '" << arg << "'" << endl;
            printUsage();
            return 1;
        } else {
            path = arg;
        }
    }
    if (path.empty()) {
        printUsage();
        return 1;
    }

    try {
        // front end: source -> tokens -> syntax tree -> AST -> sema
        auto scanner = make_shared<tiny::TinyScanner>(path);
        auto tokenScanner = make_shared<tiny::TinyTokenScanner>(scanner);
        tiny::TinyParser parser(tokenScanner);
        shared_ptr<tiny::TinySyntaxNode> root = parser.parse();

        auto nodeBuilder = make_shared<builder::TinyNodeBuilder>();
        root->accept(nodeBuilder);
        shared_ptr<ast::Program> program = nodeBuilder->getResult();

        auto sema = make_shared<sema::Sema>();
        sema->analyze(program);

        // back end: AST -> LLVM IR -> optimize -> link -> load -> run
        codegen::TargetContext tc;
        auto codeGen = make_shared<codegen::CodeGen>(tc);
        auto module = codeGen->generate(program);

        if (doOptimize) {
            optimize::Optimizer opt;
            opt.run(*module);
        }

        if (emitLlvm) {
            module->print(llvm::outs(), nullptr);
        }

        linker::Linker linker(tc);
        linker::LinkedImage image = linker.link(*module);

        loader::Loader ldr;
        ldr.registerNative("print_int", reinterpret_cast<void*>(&print_int));
        ldr.registerNative("malloc", reinterpret_cast<void*>(&tiny_malloc));
        ldr.load(image);

        auto fn = reinterpret_cast<int(*)()>(ldr.findSymbol("main"));
        return fn();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}
