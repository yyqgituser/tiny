#ifndef SEMA_FUNCTIONSIGNATURE_H_
#define SEMA_FUNCTIONSIGNATURE_H_

#include <string>

namespace sema {

// A resolvable function: either a user-defined function or a builtin native.
struct FunctionSignature {
    enum Kind { USER, NATIVE };
    std::string name;
    int paramCount;
    Kind kind;
};

}
#endif
