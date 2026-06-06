#ifndef AST_TYPE_H_
#define AST_TYPE_H_

namespace ast {

// M1: only int. Enum leaves room for double/char/void/pointers later.
struct Type {
    enum Kind { INT, INT_ARRAY };
    Kind kind = INT;
};

}
#endif
