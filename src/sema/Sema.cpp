#include "sema/Sema.h"

#include <stdexcept>

using namespace std;
using namespace ast;

namespace sema {

Sema::Sema() {}

void Sema::registerBuiltins() {
    functions["print_int"] = FunctionSignature{"print_int", 1, FunctionSignature::NATIVE};
}

void Sema::analyze(shared_ptr<Program> program) {
    registerBuiltins();
    collectSignatures(program);
    program->accept(shared_from_this());
}

void Sema::collectSignatures(shared_ptr<Program> program) {
    for (auto& fn : *program->functions) {
        if (functions.find(fn->name) != functions.end()) {
            throw runtime_error("sema: duplicate function '" + fn->name + "'");
        }
        functions[fn->name] = FunctionSignature{
            fn->name, static_cast<int>(fn->params->size()), FunctionSignature::USER};
    }
}

int Sema::declareVar(const string& name, const Type& type) {
    auto& top = scopes.back();
    if (top.find(name) != top.end()) {
        throw runtime_error("sema: duplicate variable '" + name + "' in scope");
    }
    int slot = nextSlot++;
    top[name] = VarInfo{slot, type};
    slotTypesAcc.push_back(type);
    return slot;
}

Sema::VarInfo Sema::lookupVar(const string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) return found->second;
    }
    throw runtime_error("sema: undefined variable '" + name + "'");
}

void Sema::visitProgram(shared_ptr<Program> node) {
    for (auto& fn : *node->functions) {
        fn->accept(shared_from_this());
    }
}

void Sema::visitFunctionDef(shared_ptr<FunctionDef> node) {
    scopes.clear();
    nextSlot = 0;
    slotTypesAcc.clear();
    currentFn = node;
    scopes.push_back({});                 // function-level scope (holds params)
    for (auto& p : *node->params) {
        declareVar(p->name, p->type);     // params get slots 0..N-1
    }
    node->body->accept(shared_from_this());
    node->slotCount = nextSlot;           // total slots
    node->slotTypes = make_shared<vector<Type>>(slotTypesAcc);
    currentFn = nullptr;
}

void Sema::visitBlock(shared_ptr<Block> node) {
    scopes.push_back({});
    for (auto& s : *node->statements) {
        s->accept(shared_from_this());
    }
    scopes.pop_back();
}

void Sema::visitIfStmt(shared_ptr<IfStmt> node) {
    node->cond->accept(shared_from_this());
    node->thenStmt->accept(shared_from_this());
    if (node->elseStmt) {
        node->elseStmt->accept(shared_from_this());
    }
}

void Sema::visitWhileStmt(shared_ptr<WhileStmt> node) {
    node->cond->accept(shared_from_this());
    node->body->accept(shared_from_this());
}

void Sema::visitReturnStmt(shared_ptr<ReturnStmt> node) {
    if (node->value) {
        node->value->accept(shared_from_this());
    }
}

void Sema::visitExprStmt(shared_ptr<ExprStmt> node) {
    node->expr->accept(shared_from_this());
}

void Sema::visitIntLiteral(shared_ptr<IntLiteral> node) {
    node->exprType = Type{Type::INT};
}

void Sema::visitVarRef(shared_ptr<VarRef> node) {
    auto info = lookupVar(node->name);
    node->slotIndex = info.slot;
    node->exprType = info.type;
}

void Sema::visitBinaryExpr(shared_ptr<BinaryExpr> node) {
    node->left->accept(shared_from_this());
    node->right->accept(shared_from_this());
    if (node->left->exprType.kind != Type::INT || node->right->exprType.kind != Type::INT) {
        throw runtime_error("sema: binary operator requires int operands");
    }
    node->exprType = Type{Type::INT};
}

void Sema::visitCallExpr(shared_ptr<CallExpr> node) {
    auto it = functions.find(node->callee);
    if (it == functions.end()) {
        throw runtime_error("sema: undefined function '" + node->callee + "'");
    }
    const FunctionSignature& sig = it->second;
    int argCount = static_cast<int>(node->args->size());
    if (argCount != sig.paramCount) {
        throw runtime_error("sema: function '" + node->callee + "' expects "
            + to_string(sig.paramCount) + " args but got " + to_string(argCount));
    }
    node->resolvedKind = (sig.kind == FunctionSignature::NATIVE)
        ? CallExpr::NATIVE : CallExpr::USER;
    node->calleeParamCount = sig.paramCount;
    for (auto& a : *node->args) {
        a->accept(shared_from_this());
    }
    node->exprType = Type{Type::INT};
}

void Sema::visitLocalVarDecl(shared_ptr<LocalVarDecl> node) {
    // init is evaluated before the variable is in scope (cannot reference itself)
    if (node->init) {
        node->init->accept(shared_from_this());
        if (node->init->exprType.kind != node->type.kind) {
            throw runtime_error("sema: initializer type mismatch for '" + node->name + "'");
        }
    }
    node->slotIndex = declareVar(node->name, node->type);
}

void Sema::visitAssignStmt(shared_ptr<AssignStmt> node) {
    auto info = lookupVar(node->name);
    node->slotIndex = info.slot;
    if (!node->isArray) {
        node->value->accept(shared_from_this());
        if (node->value->exprType.kind != info.type.kind) {
            throw runtime_error("sema: assignment type mismatch for '" + node->name + "'");
        }
    } else {
        if (info.type.kind != Type::INT_ARRAY) {
            throw runtime_error("sema: '" + node->name + "' is not an array");
        }
        node->index->accept(shared_from_this());
        if (node->index->exprType.kind != Type::INT) {
            throw runtime_error("sema: array index must be int");
        }
        node->value->accept(shared_from_this());
        if (node->value->exprType.kind != Type::INT) {
            throw runtime_error("sema: array element assignment must be int");
        }
    }
}

void Sema::visitNewArray(shared_ptr<NewArray> node) {
    node->size->accept(shared_from_this());
    if (node->size->exprType.kind != Type::INT) {
        throw runtime_error("sema: array size must be int");
    }
    node->exprType = Type{Type::INT_ARRAY};
}

void Sema::visitArrayIndex(shared_ptr<ArrayIndex> node) {
    auto info = lookupVar(node->arrayName);
    if (info.type.kind != Type::INT_ARRAY) {
        throw runtime_error("sema: '" + node->arrayName + "' is not an array");
    }
    node->slotIndex = info.slot;
    node->index->accept(shared_from_this());
    if (node->index->exprType.kind != Type::INT) {
        throw runtime_error("sema: array index must be int");
    }
    node->exprType = Type{Type::INT};
}

void Sema::visitNotExpr(shared_ptr<NotExpr> node) {
    node->operand->accept(shared_from_this());
    if (node->operand->exprType.kind != Type::INT) {
        throw runtime_error("sema: '!' requires int operand");
    }
    node->exprType = Type{Type::INT};
}

}
