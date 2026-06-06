#ifndef CODEGEN_CODEGEN_H_
#define CODEGEN_CODEGEN_H_

#include <memory>
#include <string>
#include <unordered_map>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/DerivedTypes.h>

#include "codegen/TargetContext.h"
#include "ast/TinyAstVisitor.h"
#include "ast/Decl.h"
#include "ast/Stmt.h"
#include "ast/Expr.h"

namespace codegen {

// Translates an annotated tiny AST (Program) into an llvm::Module.
// visitor + result-slot (value) pattern, same as hila CodeGenerator but no
// objects. Native calls go through a GOT slot (GlobalVariable function pointer)
// loaded then called indirectly (scheme A, verified in M1.0).
class CodeGen : public ast::TinyAstVisitor, public std::enable_shared_from_this<CodeGen> {
public:
    CodeGen(TargetContext& targetContext);
    ~CodeGen();

    std::unique_ptr<llvm::Module> generate(std::shared_ptr<ast::Program> program);

    void visitFunctionDef(std::shared_ptr<ast::FunctionDef> node) override;
    void visitBlock(std::shared_ptr<ast::Block> node) override;
    void visitIfStmt(std::shared_ptr<ast::IfStmt> node) override;
    void visitWhileStmt(std::shared_ptr<ast::WhileStmt> node) override;
    void visitReturnStmt(std::shared_ptr<ast::ReturnStmt> node) override;
    void visitExprStmt(std::shared_ptr<ast::ExprStmt> node) override;
    void visitIntLiteral(std::shared_ptr<ast::IntLiteral> node) override;
    void visitVarRef(std::shared_ptr<ast::VarRef> node) override;
    void visitBinaryExpr(std::shared_ptr<ast::BinaryExpr> node) override;
    void visitCallExpr(std::shared_ptr<ast::CallExpr> node) override;
    void visitLocalVarDecl(std::shared_ptr<ast::LocalVarDecl> node) override;
    void visitAssignStmt(std::shared_ptr<ast::AssignStmt> node) override;
    void visitNewArray(std::shared_ptr<ast::NewArray> node) override;
    void visitArrayIndex(std::shared_ptr<ast::ArrayIndex> node) override;

private:
    TargetContext& targetContext;
    llvm::LLVMContext& context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    // declared user functions: name -> Function*
    std::unordered_map<std::string, llvm::Function*> userFunctions;
    // native GOT slots: name -> GlobalVariable* (function pointer)
    std::unordered_map<std::string, llvm::GlobalVariable*> nativePtrs;
    std::unordered_map<std::string, llvm::FunctionType*> nativeTypes;

    llvm::Function* currentFunction = nullptr;
    llvm::Value* value = nullptr;   // expression result slot
    std::vector<llvm::Value*> slots;   // slotIndex -> alloca ptr

    llvm::Type* mapType(const ast::Type& t);
    llvm::FunctionType* functionType(std::shared_ptr<ast::FunctionDef> fn);
    void declareFunctions(std::shared_ptr<ast::Program> program);
    llvm::GlobalVariable* getOrCreateNativeSlot(const std::string& name);
    bool currentBlockTerminated();
};

}
#endif
