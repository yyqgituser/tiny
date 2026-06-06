#include "ast/Expr.h"
#include "ast/Stmt.h"
#include "ast/Decl.h"
#include "ast/TinyAstVisitor.h"

namespace ast {

void IntLiteral::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitIntLiteral(shared_from_this()); }
void VarRef::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitVarRef(shared_from_this()); }
void BinaryExpr::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitBinaryExpr(shared_from_this()); }
void CallExpr::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitCallExpr(shared_from_this()); }
void NewArray::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitNewArray(shared_from_this()); }
void ArrayIndex::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitArrayIndex(shared_from_this()); }

void Block::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitBlock(shared_from_this()); }
void IfStmt::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitIfStmt(shared_from_this()); }
void ReturnStmt::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitReturnStmt(shared_from_this()); }
void ExprStmt::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitExprStmt(shared_from_this()); }
void LocalVarDecl::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitLocalVarDecl(shared_from_this()); }
void AssignStmt::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitAssignStmt(shared_from_this()); }
void WhileStmt::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitWhileStmt(shared_from_this()); }

void Param::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitParam(shared_from_this()); }
void FunctionDef::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitFunctionDef(shared_from_this()); }
void Program::accept(std::shared_ptr<TinyAstVisitor> v) { v->visitProgram(shared_from_this()); }

}
