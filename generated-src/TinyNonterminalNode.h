#ifndef _TINY_TINYNONTERMINALNODE_H
#define _TINY_TINYNONTERMINALNODE_H

#include <memory>
#include <vector>
#include "TinySyntaxNode.h"

namespace tiny {
class TinyNonterminalNode : public TinySyntaxNode, public std::enable_shared_from_this<TinyNonterminalNode> {
public:
    unsigned int productionId;

    std::shared_ptr<std::vector<std::shared_ptr<TinySyntaxNode>>> rhs;

    TinyNonterminalNode(unsigned int productionId, std::shared_ptr<std::vector<std::shared_ptr<TinySyntaxNode>>> rhs) : productionId(productionId), rhs(rhs) {
    }

    virtual ~TinyNonterminalNode() {
    }

    virtual void accept(std::shared_ptr<TinySyntaxNodeVisitor> visitor) {
        visitor->dispatch(shared_from_this());
    }
};

}

#endif
