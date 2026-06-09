#include "codegen/CodeGen.h"

#include <stdexcept>
#include <vector>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

using namespace std;
using namespace llvm;
using namespace ast;

namespace codegen {

CodeGen::CodeGen(TargetContext& targetContext)
    : targetContext(targetContext), context(targetContext.getLLVMContext()) {}

CodeGen::~CodeGen() = default;

llvm::Type* CodeGen::mapType(const ast::Type& t) {
    switch (t.kind) {
        case ast::Type::INT: return llvm::Type::getInt32Ty(context);
        case ast::Type::INT_ARRAY: return llvm::PointerType::get(context, 0);
    }
    throw runtime_error("codegen: unknown type kind");
}

FunctionType* CodeGen::functionType(shared_ptr<FunctionDef> fn) {
    llvm::Type* retType = mapType(fn->returnType);
    vector<llvm::Type*> paramTypes;
    for (auto& p : *fn->params) {
        paramTypes.push_back(mapType(p->type));
    }
    return FunctionType::get(retType, paramTypes, false);
}

unique_ptr<Module> CodeGen::generate(shared_ptr<Program> program) {
    module = make_unique<Module>("tiny", context);
    module->setTargetTriple(targetContext.getTargetTriple());
    module->setDataLayout(targetContext.getTargetMachine().createDataLayout());
    builder = make_unique<IRBuilder<>>(context);

    declareFunctions(program);
    for (auto& fn : *program->functions) {
        fn->accept(shared_from_this());
    }

    if (verifyModule(*module, &errs())) {
        throw runtime_error("codegen: verifyModule failed");
    }

    return move(module);
}

void CodeGen::declareFunctions(shared_ptr<Program> program) {
    for (auto& fn : *program->functions) {
        FunctionType* ft = functionType(fn);
        Function* f = Function::Create(ft, Function::ExternalLinkage, fn->name, module.get());
        userFunctions[fn->name] = f;
    }
}

GlobalVariable* CodeGen::getOrCreateNativeSlot(const string& name) {
    auto it = nativePtrs.find(name);
    if (it != nativePtrs.end()) {
        return it->second;
    }
    FunctionType* fnType;
    if (name == "malloc") {
        // ptr malloc(i64)
        fnType = FunctionType::get(PointerType::get(context, 0),
                                   {llvm::Type::getInt64Ty(context)}, false);
    } else {
        // print_int : i32(i32)
        llvm::Type* i32 = llvm::Type::getInt32Ty(context);
        fnType = FunctionType::get(i32, {i32}, false);
    }
    PointerType* fnPtrType = PointerType::get(fnType, 0);
    auto* gv = new GlobalVariable(
        *module, fnPtrType, /*isConstant=*/false,
        GlobalValue::ExternalLinkage, /*Initializer=*/nullptr, name);
    nativePtrs[name] = gv;
    nativeTypes[name] = fnType;
    return gv;
}

bool CodeGen::currentBlockTerminated() {
    BasicBlock* bb = builder->GetInsertBlock();
    return bb && bb->getTerminator() != nullptr;
}

void CodeGen::visitFunctionDef(shared_ptr<FunctionDef> node) {
    Function* fn = userFunctions[node->name];
    currentFunction = fn;
    BasicBlock* entry = BasicBlock::Create(context, "entry", fn);
    builder->SetInsertPoint(entry);

    // allocate a slot for every variable (params + locals), typed.
    slots.assign(node->slotCount, nullptr);
    for (int i = 0; i < node->slotCount; i++) {
        slots[i] = builder->CreateAlloca(mapType((*node->slotTypes)[i]));
    }
    // store incoming params into their slots (params are slots 0..N-1)
    for (size_t i = 0; i < node->params->size(); i++) {
        builder->CreateStore(fn->getArg(static_cast<unsigned>(i)),
                             slots[static_cast<int>(i)]);
    }

    node->body->accept(shared_from_this());
    currentFunction = nullptr;
}

void CodeGen::visitBlock(shared_ptr<Block> node) {
    for (auto& s : *node->statements) {
        // POC: stop emitting after a terminator (e.g. statements after return
        // are unreachable). Avoids inserting into a terminated block.
        if (currentBlockTerminated()) {
            break;
        }
        s->accept(shared_from_this());
    }
}

void CodeGen::visitIfStmt(shared_ptr<IfStmt> node) {
    node->cond->accept(shared_from_this());
    Value* cond = value;
    value = nullptr;

    Function* fn = currentFunction;
    BasicBlock* thenBB = BasicBlock::Create(context, "then", fn);
    BasicBlock* mergeBB = BasicBlock::Create(context, "merge", fn);
    BasicBlock* elseBB = node->elseStmt ? BasicBlock::Create(context, "else", fn) : mergeBB;

    builder->CreateCondBr(cond, thenBB, elseBB);

    // then
    builder->SetInsertPoint(thenBB);
    node->thenStmt->accept(shared_from_this());
    if (!currentBlockTerminated()) {
        builder->CreateBr(mergeBB);
    }

    // else
    if (node->elseStmt) {
        builder->SetInsertPoint(elseBB);
        node->elseStmt->accept(shared_from_this());
        if (!currentBlockTerminated()) {
            builder->CreateBr(mergeBB);
        }
    }

    builder->SetInsertPoint(mergeBB);
}

void CodeGen::visitWhileStmt(shared_ptr<WhileStmt> node) {
    Function* fn = currentFunction;
    BasicBlock* condBB = BasicBlock::Create(context, "while.cond", fn);
    BasicBlock* bodyBB = BasicBlock::Create(context, "while.body", fn);
    BasicBlock* exitBB = BasicBlock::Create(context, "while.exit", fn);

    builder->CreateBr(condBB);

    builder->SetInsertPoint(condBB);
    node->cond->accept(shared_from_this());
    Value* c = value;
    value = nullptr;
    builder->CreateCondBr(c, bodyBB, exitBB);

    builder->SetInsertPoint(bodyBB);
    node->body->accept(shared_from_this());
    if (!currentBlockTerminated()) {
        builder->CreateBr(condBB);
    }

    builder->SetInsertPoint(exitBB);
}

void CodeGen::visitReturnStmt(shared_ptr<ReturnStmt> node) {
    if (node->value) {
        node->value->accept(shared_from_this());
        builder->CreateRet(value);
        value = nullptr;
    } else {
        builder->CreateRetVoid();
    }
}

void CodeGen::visitExprStmt(shared_ptr<ExprStmt> node) {
    node->expr->accept(shared_from_this());
    value = nullptr;
}

void CodeGen::visitIntLiteral(shared_ptr<IntLiteral> node) {
    value = ConstantInt::get(llvm::Type::getInt32Ty(context), node->value);
}

void CodeGen::visitVarRef(shared_ptr<VarRef> node) {
    llvm::Type* t = mapType(node->exprType);
    value = builder->CreateLoad(t, slots[node->slotIndex]);
}

void CodeGen::visitBinaryExpr(shared_ptr<BinaryExpr> node) {
    // AND/OR need short-circuit evaluation (conditional branches),
    // not the usual "eval both sides then combine" pattern.
    if (node->op == BinaryExpr::AND) {
        Function* fn = currentFunction;
        BasicBlock* rhsBB = BasicBlock::Create(context, "and.rhs", fn);
        BasicBlock* mergeBB = BasicBlock::Create(context, "and.merge", fn);

        node->left->accept(shared_from_this());
        Value* lhs = toBool(value); value = nullptr;
        BasicBlock* lhsBB = builder->GetInsertBlock();
        builder->CreateCondBr(lhs, rhsBB, mergeBB);

        builder->SetInsertPoint(rhsBB);
        node->right->accept(shared_from_this());
        Value* rhs = toBool(value); value = nullptr;
        BasicBlock* rhsEndBB = builder->GetInsertBlock();
        builder->CreateBr(mergeBB);

        builder->SetInsertPoint(mergeBB);
        PHINode* phi = builder->CreatePHI(llvm::Type::getInt1Ty(context), 2);
        phi->addIncoming(ConstantInt::getFalse(context), lhsBB);
        phi->addIncoming(rhs, rhsEndBB);
        value = phi;
        return;
    }
    if (node->op == BinaryExpr::OR) {
        Function* fn = currentFunction;
        BasicBlock* rhsBB = BasicBlock::Create(context, "or.rhs", fn);
        BasicBlock* mergeBB = BasicBlock::Create(context, "or.merge", fn);

        node->left->accept(shared_from_this());
        Value* lhs = toBool(value); value = nullptr;
        BasicBlock* lhsBB = builder->GetInsertBlock();
        builder->CreateCondBr(lhs, mergeBB, rhsBB);

        builder->SetInsertPoint(rhsBB);
        node->right->accept(shared_from_this());
        Value* rhs = toBool(value); value = nullptr;
        BasicBlock* rhsEndBB = builder->GetInsertBlock();
        builder->CreateBr(mergeBB);

        builder->SetInsertPoint(mergeBB);
        PHINode* phi = builder->CreatePHI(llvm::Type::getInt1Ty(context), 2);
        phi->addIncoming(ConstantInt::getTrue(context), lhsBB);
        phi->addIncoming(rhs, rhsEndBB);
        value = phi;
        return;
    }

    node->left->accept(shared_from_this());
    Value* l = value;
    value = nullptr;
    node->right->accept(shared_from_this());
    Value* r = value;
    value = nullptr;
    switch (node->op) {
        case BinaryExpr::LT:    value = builder->CreateICmpSLT(l, r); break;
        case BinaryExpr::GT:    value = builder->CreateICmpSGT(l, r); break;
        case BinaryExpr::LE:    value = builder->CreateICmpSLE(l, r); break;
        case BinaryExpr::GE:    value = builder->CreateICmpSGE(l, r); break;
        case BinaryExpr::EQ_EQ: value = builder->CreateICmpEQ(l, r); break;
        case BinaryExpr::NE:    value = builder->CreateICmpNE(l, r); break;
        case BinaryExpr::ADD:   value = builder->CreateAdd(l, r); break;
        case BinaryExpr::SUB:   value = builder->CreateSub(l, r); break;
        case BinaryExpr::MUL:   value = builder->CreateMul(l, r); break;
        case BinaryExpr::DIV:   value = builder->CreateSDiv(l, r); break;
        case BinaryExpr::MOD:   value = builder->CreateSRem(l, r); break;
        default: throw runtime_error("codegen: unknown binary op");
    }
}

void CodeGen::visitCallExpr(shared_ptr<CallExpr> node) {
    vector<Value*> args;
    for (auto& a : *node->args) {
        a->accept(shared_from_this());
        args.push_back(value);
        value = nullptr;
    }

    if (node->resolvedKind == CallExpr::NATIVE) {
        GlobalVariable* gv = getOrCreateNativeSlot(node->callee);
        FunctionType* fnType = nativeTypes[node->callee];
        PointerType* fnPtrType = cast<PointerType>(gv->getValueType());
        Value* fp = builder->CreateLoad(fnPtrType, gv);
        value = builder->CreateCall(fnType, fp, args);
    } else {
        Function* fn = userFunctions[node->callee];
        value = builder->CreateCall(fn, args);
    }
}

void CodeGen::visitLocalVarDecl(shared_ptr<LocalVarDecl> node) {
    if (node->init) {
        node->init->accept(shared_from_this());
        builder->CreateStore(value, slots[node->slotIndex]);
        value = nullptr;
    }
    // no init: alloca left undefined (M2.1: no uninit check)
}

void CodeGen::visitAssignStmt(shared_ptr<AssignStmt> node) {
    if (!node->isArray) {
        node->value->accept(shared_from_this());
        builder->CreateStore(value, slots[node->slotIndex]);
        value = nullptr;
    } else {
        llvm::Type* ptrTy = llvm::PointerType::get(context, 0);
        Value* arrPtr = builder->CreateLoad(ptrTy, slots[node->slotIndex]);
        node->index->accept(shared_from_this());
        Value* idx = value; value = nullptr;
        node->value->accept(shared_from_this());
        Value* v = value; value = nullptr;
        llvm::Type* i32 = llvm::Type::getInt32Ty(context);
        Value* elemPtr = builder->CreateGEP(i32, arrPtr, {idx});
        builder->CreateStore(v, elemPtr);
    }
}

void CodeGen::visitNewArray(shared_ptr<NewArray> node) {
    node->size->accept(shared_from_this());
    Value* n = value; value = nullptr;
    Value* n64 = builder->CreateSExt(n, llvm::Type::getInt64Ty(context));
    Value* bytes = builder->CreateMul(n64,
        ConstantInt::get(llvm::Type::getInt64Ty(context), 4));
    GlobalVariable* gv = getOrCreateNativeSlot("malloc");
    FunctionType* ft = nativeTypes["malloc"];
    PointerType* fpt = cast<PointerType>(gv->getValueType());
    Value* fp = builder->CreateLoad(fpt, gv);
    value = builder->CreateCall(ft, fp, {bytes});
}

void CodeGen::visitArrayIndex(shared_ptr<ArrayIndex> node) {
    llvm::Type* ptrTy = llvm::PointerType::get(context, 0);
    Value* arrPtr = builder->CreateLoad(ptrTy, slots[node->slotIndex]);
    node->index->accept(shared_from_this());
    Value* idx = value; value = nullptr;
    llvm::Type* i32 = llvm::Type::getInt32Ty(context);
    Value* elemPtr = builder->CreateGEP(i32, arrPtr, {idx});
    value = builder->CreateLoad(i32, elemPtr);
}

void CodeGen::visitNotExpr(shared_ptr<NotExpr> node) {
    node->operand->accept(shared_from_this());
    Value* b = toBool(value); value = nullptr;
    value = builder->CreateXor(b, ConstantInt::getTrue(context));
}

Value* CodeGen::toBool(Value* v) {
    if (v->getType()->isIntegerTy(1)) return v;
    return builder->CreateICmpNE(v, ConstantInt::get(v->getType(), 0));
}

}
