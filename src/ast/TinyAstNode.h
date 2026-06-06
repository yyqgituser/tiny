#ifndef AST_TINYASTNODE_H_
#define AST_TINYASTNODE_H_

#include <memory>

namespace ast {

class TinyAstVisitor;

class TinyAstNode {
public:
    TinyAstNode() {}
    virtual ~TinyAstNode() {}
    virtual void accept(std::shared_ptr<TinyAstVisitor> visitor) = 0;
};

}
#endif
