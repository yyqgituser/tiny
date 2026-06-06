#ifndef AST_EXPR_H_
#define AST_EXPR_H_

#include <memory>
#include <string>
#include <vector>
#include "ast/TinyAstNode.h"
#include "ast/Type.h"

namespace ast {

class TinyAstVisitor;

class Expr : public TinyAstNode {
public:
    Type exprType;   // filled by sema
};

class IntLiteral : public Expr, public std::enable_shared_from_this<IntLiteral> {
public:
    int value;
    IntLiteral(int value) : value(value) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

class VarRef : public Expr, public std::enable_shared_from_this<VarRef> {
public:
    std::string name;
    int slotIndex = -1;   // sema: resolved variable slot (param or local)
    VarRef(const std::string& name) : name(name) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

class BinaryExpr : public Expr, public std::enable_shared_from_this<BinaryExpr> {
public:
    enum Op { LT, GT, LE, GE, EQ_EQ, NE, ADD, SUB, MUL, DIV, MOD };
    Op op;
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
    BinaryExpr(Op op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right)
        : op(op), left(left), right(right) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

class CallExpr : public Expr, public std::enable_shared_from_this<CallExpr> {
public:
    enum ResolvedKind { UNRESOLVED, USER, NATIVE };
    std::string callee;
    std::shared_ptr<std::vector<std::shared_ptr<Expr>>> args;
    // sema annotations:
    ResolvedKind resolvedKind = UNRESOLVED;
    int calleeParamCount = -1;   // resolved function's parameter count
    CallExpr(const std::string& callee, std::shared_ptr<std::vector<std::shared_ptr<Expr>>> args)
        : callee(callee), args(args) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

class NewArray : public Expr, public std::enable_shared_from_this<NewArray> {
public:
    std::shared_ptr<Expr> size;
    NewArray(std::shared_ptr<Expr> size) : size(size) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

class ArrayIndex : public Expr, public std::enable_shared_from_this<ArrayIndex> {
public:
    std::string arrayName;
    std::shared_ptr<Expr> index;
    int slotIndex = -1;   // sema: array variable's slot
    ArrayIndex(const std::string& arrayName, std::shared_ptr<Expr> index)
        : arrayName(arrayName), index(index) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

}
#endif
