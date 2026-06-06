#ifndef AST_DECL_H_
#define AST_DECL_H_

#include <memory>
#include <string>
#include <vector>
#include "ast/TinyAstNode.h"
#include "ast/Type.h"
#include "ast/Stmt.h"

namespace ast {

class TinyAstVisitor;

class Param : public TinyAstNode, public std::enable_shared_from_this<Param> {
public:
    Type type;
    std::string name;
    Param(Type type, const std::string& name) : type(type), name(name) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

class FunctionDef : public TinyAstNode, public std::enable_shared_from_this<FunctionDef> {
public:
    Type returnType;
    std::string name;
    std::shared_ptr<std::vector<std::shared_ptr<Param>>> params;
    std::shared_ptr<Block> body;
    int slotCount = 0;            // sema annotation: total slots (params + locals)
    std::shared_ptr<std::vector<Type>> slotTypes;   // sema: each slot's type
    FunctionDef(Type returnType, const std::string& name,
                std::shared_ptr<std::vector<std::shared_ptr<Param>>> params,
                std::shared_ptr<Block> body)
        : returnType(returnType), name(name), params(params), body(body) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

class Program : public TinyAstNode, public std::enable_shared_from_this<Program> {
public:
    std::shared_ptr<std::vector<std::shared_ptr<FunctionDef>>> functions;
    Program(std::shared_ptr<std::vector<std::shared_ptr<FunctionDef>>> functions)
        : functions(functions) {}
    void accept(std::shared_ptr<TinyAstVisitor> visitor) override;
};

}
#endif
