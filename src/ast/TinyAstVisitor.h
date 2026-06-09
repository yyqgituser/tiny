#ifndef AST_TINYASTVISITOR_H_
#define AST_TINYASTVISITOR_H_

#include <memory>
#include "ast/Expr.h"
#include "ast/Stmt.h"
#include "ast/Decl.h"

namespace ast {

class TinyAstVisitor {
public:
    TinyAstVisitor() {}
    virtual ~TinyAstVisitor() {}

    virtual void visitProgram(std::shared_ptr<Program> node) {}
    virtual void visitFunctionDef(std::shared_ptr<FunctionDef> node) {}
    virtual void visitParam(std::shared_ptr<Param> node) {}
    virtual void visitBlock(std::shared_ptr<Block> node) {}
    virtual void visitIfStmt(std::shared_ptr<IfStmt> node) {}
    virtual void visitReturnStmt(std::shared_ptr<ReturnStmt> node) {}
    virtual void visitExprStmt(std::shared_ptr<ExprStmt> node) {}
    virtual void visitLocalVarDecl(std::shared_ptr<LocalVarDecl> node) {}
    virtual void visitAssignStmt(std::shared_ptr<AssignStmt> node) {}
    virtual void visitWhileStmt(std::shared_ptr<WhileStmt> node) {}
    virtual void visitIntLiteral(std::shared_ptr<IntLiteral> node) {}
    virtual void visitVarRef(std::shared_ptr<VarRef> node) {}
    virtual void visitBinaryExpr(std::shared_ptr<BinaryExpr> node) {}
    virtual void visitCallExpr(std::shared_ptr<CallExpr> node) {}
    virtual void visitNewArray(std::shared_ptr<NewArray> node) {}
    virtual void visitArrayIndex(std::shared_ptr<ArrayIndex> node) {}
    virtual void visitNotExpr(std::shared_ptr<NotExpr> node) {}
};

}
#endif
