#ifndef _TINY_TINYTERMINALNODE_H
#define _TINY_TINYTERMINALNODE_H

#include <memory>
#include "TinyToken.h"
#include "TinySyntaxNode.h"

namespace tiny {
class TinyTerminalNode : public TinySyntaxNode, public std::enable_shared_from_this<TinyTerminalNode> {
public:
    std::shared_ptr<TinyToken> token;

    TinyTerminalNode(std::shared_ptr<TinyToken> token) : token(token) {
    }

    virtual ~TinyTerminalNode() {
    }

    virtual void accept(std::shared_ptr<TinySyntaxNodeVisitor> visitor) {
        visitor->handleTerminal(shared_from_this());
    }
};

}

#endif
