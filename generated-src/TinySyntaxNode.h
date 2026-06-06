#ifndef _TINY_TINYSYNTAXNODE_H
#define _TINY_TINYSYNTAXNODE_H

#include <memory>
#include "TinySyntaxNodeVisitor.h"

namespace tiny {
class TinySyntaxNode {
public:
    TinySyntaxNode() {
    }

    virtual ~TinySyntaxNode() {
    }

    virtual void accept(std::shared_ptr<TinySyntaxNodeVisitor> visitor) = 0;
};

}

#endif
