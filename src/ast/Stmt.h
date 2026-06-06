#ifndef AST_STMT_H_
#define AST_STMT_H_

#include <memory>
#include <vector>
#include "ast/TinyAstNode.h"
#include "ast/Expr.h"
#include "ast/Type.h"

namespace ast {

class TinyAstVisitor;

class Stmt : public TinyAstNode {
};

class Block : public Stmt, public std::enable_shared_from_this<Block> {
public:
    std::shared_ptr<std::vector<std::shared_ptr<Stmt>>> statements;
    Block(std::shared_ptr<std::vector<std::shared_ptr<Stmt>>> statements)
        : statements(statements) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

class IfStmt : public Stmt, public std::enable_shared_from_this<IfStmt> {
public:
    std::shared_ptr<Expr> cond;
    std::shared_ptr<Stmt> thenStmt;
    std::shared_ptr<Stmt> elseStmt;   // nullable
    IfStmt(std::shared_ptr<Expr> cond, std::shared_ptr<Stmt> thenStmt, std::shared_ptr<Stmt> elseStmt)
        : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

class ReturnStmt : public Stmt, public std::enable_shared_from_this<ReturnStmt> {
public:
    std::shared_ptr<Expr> value;   // nullable
    ReturnStmt(std::shared_ptr<Expr> value) : value(value) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

class ExprStmt : public Stmt, public std::enable_shared_from_this<ExprStmt> {
public:
    std::shared_ptr<Expr> expr;
    ExprStmt(std::shared_ptr<Expr> expr) : expr(expr) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

class LocalVarDecl : public Stmt, public std::enable_shared_from_this<LocalVarDecl> {
public:
    Type type;
    std::string name;
    std::shared_ptr<Expr> init;   // nullable
    int slotIndex = -1;           // sema annotation
    LocalVarDecl(Type type, const std::string& name, std::shared_ptr<Expr> init)
        : type(type), name(name), init(init) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

class AssignStmt : public Stmt, public std::enable_shared_from_this<AssignStmt> {
public:
    bool isArray;                 // false: x = v ; true: a[i] = v
    std::string name;             // variable name (the array var name if isArray)
    std::shared_ptr<Expr> index;  // non-null if isArray
    std::shared_ptr<Expr> value;
    int slotIndex = -1;           // sema annotation
    AssignStmt(const std::string& name, std::shared_ptr<Expr> value)
        : isArray(false), name(name), index(nullptr), value(value) {}
    AssignStmt(const std::string& name, std::shared_ptr<Expr> index, std::shared_ptr<Expr> value)
        : isArray(true), name(name), index(index), value(value) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

class WhileStmt : public Stmt, public std::enable_shared_from_this<WhileStmt> {
public:
    std::shared_ptr<Expr> cond;
    std::shared_ptr<Stmt> body;
    WhileStmt(std::shared_ptr<Expr> cond, std::shared_ptr<Stmt> body)
        : cond(cond), body(body) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

}
#endif
