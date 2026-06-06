#ifndef SEMA_SEMA_H_
#define SEMA_SEMA_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "ast/Decl.h"
#include "ast/Stmt.h"
#include "ast/Expr.h"
#include "ast/Type.h"
#include "ast/TinyAstVisitor.h"
#include "sema/FunctionSignature.h"

namespace sema {

// Two-pass semantic analysis over the tiny AST:
//   pass 1: collect all function signatures (supports forward refs / recursion)
//   pass 2: visitor traversal — resolve VarRef -> param index, CallExpr -> function.
// Results are annotated onto the AST nodes. Throws std::runtime_error on
// undefined name / arg-count mismatch (input validation of user source).
class Sema : public ast::TinyAstVisitor, public std::enable_shared_from_this<Sema> {
public:
    Sema();
    void analyze(std::shared_ptr<ast::Program> program);

    void visitProgram(std::shared_ptr<ast::Program> node) override;
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
    struct VarInfo { int slot; ast::Type type; };

    std::unordered_map<std::string, FunctionSignature> functions;
    std::vector<std::unordered_map<std::string, VarInfo>> scopes;   // scope stack
    int nextSlot = 0;
    std::shared_ptr<ast::FunctionDef> currentFn;
    std::vector<ast::Type> slotTypesAcc;   // per-function slot type accumulator

    void registerBuiltins();
    void collectSignatures(std::shared_ptr<ast::Program> program);
    int declareVar(const std::string& name, const ast::Type& type); // throws on dup in top scope
    VarInfo lookupVar(const std::string& name);                     // throws if not found
};

}
#endif
