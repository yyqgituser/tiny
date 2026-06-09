#include "builder/TinyNodeBuilder.h"

#include <stdexcept>

#include "TinyToken.h"

using namespace std;
using namespace tiny;
using namespace ast;

namespace builder {

TinyNodeBuilder::TinyNodeBuilder() {}
TinyNodeBuilder::~TinyNodeBuilder() {}

string TinyNodeBuilder::tokenText(shared_ptr<TinyTerminalNode> t) {
    // M1 sources are ASCII.
    string r;
    for (char32_t c : t->token->text) r.push_back(static_cast<char>(c));
    return r;
}

void TinyNodeBuilder::handleTerminal(shared_ptr<TinyTerminalNode> node) {
    terminal = node;
}

// ---- Program (left-recursive list, naive collect) ----
void TinyNodeBuilder::visitProgram1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // FunctionDefinition -> function
    auto list = make_shared<vector<shared_ptr<FunctionDef>>>();
    list->push_back(function);
    functionList = list;
    result = make_shared<Program>(functionList);
}

void TinyNodeBuilder::visitProgram2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // Program -> functionList (recursion)
    auto list = functionList;
    node->rhs->at(1)->accept(shared_from_this());   // FunctionDefinition -> function
    list->push_back(function);
    functionList = list;
    result = make_shared<Program>(functionList);
}

// ---- FunctionDefinition ----
void TinyNodeBuilder::visitFunctionDefinition1(shared_ptr<TinyNonterminalNode> node) {
    // Type IDENTIFIER LPAREN RPAREN Block
    node->rhs->at(0)->accept(shared_from_this());   // Type -> type
    Type retType = type;
    node->rhs->at(1)->accept(shared_from_this());   // IDENTIFIER (terminal)
    string name = tokenText(terminal);
    node->rhs->at(4)->accept(shared_from_this());   // Block -> block
    auto emptyParams = make_shared<vector<shared_ptr<Param>>>();
    function = make_shared<FunctionDef>(retType, name, emptyParams, block);
}

void TinyNodeBuilder::visitFunctionDefinition2(shared_ptr<TinyNonterminalNode> node) {
    // Type IDENTIFIER LPAREN ParameterList RPAREN Block
    node->rhs->at(0)->accept(shared_from_this());   // Type
    Type retType = type;
    node->rhs->at(1)->accept(shared_from_this());   // IDENTIFIER
    string name = tokenText(terminal);
    node->rhs->at(3)->accept(shared_from_this());   // ParameterList -> paramList
    auto params = paramList;
    node->rhs->at(5)->accept(shared_from_this());   // Block -> block
    function = make_shared<FunctionDef>(retType, name, params, block);
}

// ---- Type ----
void TinyNodeBuilder::visitType1(shared_ptr<TinyNonterminalNode> node) {
    type = Type{Type::INT};
}
void TinyNodeBuilder::visitType2(shared_ptr<TinyNonterminalNode> node) {
    // INT LBRACK RBRACK
    type = Type{Type::INT_ARRAY};
}

// ---- ParameterList (left-recursive) ----
void TinyNodeBuilder::visitParameterList1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // Parameter -> param
    auto list = make_shared<vector<shared_ptr<Param>>>();
    list->push_back(param);
    paramList = list;
}

void TinyNodeBuilder::visitParameterList2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // ParameterList -> paramList
    auto list = paramList;
    node->rhs->at(2)->accept(shared_from_this());   // Parameter -> param
    list->push_back(param);
    paramList = list;
}

void TinyNodeBuilder::visitParameter1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // Type
    Type pType = type;
    node->rhs->at(1)->accept(shared_from_this());   // IDENTIFIER
    param = make_shared<Param>(pType, tokenText(terminal));
}

// ---- Block ----
void TinyNodeBuilder::visitBlock1(shared_ptr<TinyNonterminalNode> node) {
    block = make_shared<Block>(make_shared<vector<shared_ptr<Stmt>>>());
}

void TinyNodeBuilder::visitBlock2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(1)->accept(shared_from_this());   // StatementList -> stmtList
    block = make_shared<Block>(stmtList);
}

// ---- StatementList (left-recursive) ----
void TinyNodeBuilder::visitStatementList1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // Statement -> stmt
    auto list = make_shared<vector<shared_ptr<Stmt>>>();
    list->push_back(stmt);
    stmtList = list;
}

void TinyNodeBuilder::visitStatementList2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // StatementList -> stmtList
    auto list = stmtList;
    node->rhs->at(1)->accept(shared_from_this());   // Statement -> stmt
    list->push_back(stmt);
    stmtList = list;
}

// ---- Statement (pass-through) ----
void TinyNodeBuilder::visitStatement1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // OpenStatement -> stmt
}
void TinyNodeBuilder::visitStatement2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // CloseStatement -> stmt
}

// ---- Open/Close statements -> unified IfStmt ----
void TinyNodeBuilder::visitOpenStatement1(shared_ptr<TinyNonterminalNode> node) {
    // IF LPAREN Expression RPAREN Statement   (no else)
    node->rhs->at(2)->accept(shared_from_this());   // Expression -> expr
    auto cond = expr;
    node->rhs->at(4)->accept(shared_from_this());   // Statement -> stmt
    auto thenS = stmt;
    stmt = make_shared<IfStmt>(cond, thenS, nullptr);
}

void TinyNodeBuilder::visitOpenStatement2(shared_ptr<TinyNonterminalNode> node) {
    // IF LPAREN Expression RPAREN CloseStatement ELSE OpenStatement
    node->rhs->at(2)->accept(shared_from_this());   // Expression
    auto cond = expr;
    node->rhs->at(4)->accept(shared_from_this());   // CloseStatement -> stmt
    auto thenS = stmt;
    node->rhs->at(6)->accept(shared_from_this());   // OpenStatement -> stmt
    auto elseS = stmt;
    stmt = make_shared<IfStmt>(cond, thenS, elseS);
}

void TinyNodeBuilder::visitCloseStatement1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // SimpleStatement -> stmt
}

void TinyNodeBuilder::visitCloseStatement2(shared_ptr<TinyNonterminalNode> node) {
    // IF LPAREN Expression RPAREN CloseStatement ELSE CloseStatement
    node->rhs->at(2)->accept(shared_from_this());   // Expression
    auto cond = expr;
    node->rhs->at(4)->accept(shared_from_this());   // CloseStatement -> stmt
    auto thenS = stmt;
    node->rhs->at(6)->accept(shared_from_this());   // CloseStatement -> stmt
    auto elseS = stmt;
    stmt = make_shared<IfStmt>(cond, thenS, elseS);
}

void TinyNodeBuilder::visitOpenStatement3(shared_ptr<TinyNonterminalNode> node) {
    // WHILE LPAREN Expression RPAREN OpenStatement
    node->rhs->at(2)->accept(shared_from_this());   // Expression -> expr
    auto cond = expr;
    node->rhs->at(4)->accept(shared_from_this());   // OpenStatement -> stmt
    stmt = make_shared<WhileStmt>(cond, stmt);
}

void TinyNodeBuilder::visitCloseStatement3(shared_ptr<TinyNonterminalNode> node) {
    // WHILE LPAREN Expression RPAREN CloseStatement
    node->rhs->at(2)->accept(shared_from_this());   // Expression -> expr
    auto cond = expr;
    node->rhs->at(4)->accept(shared_from_this());   // CloseStatement -> stmt
    stmt = make_shared<WhileStmt>(cond, stmt);
}

// ---- SimpleStatement (pass-through) ----
void TinyNodeBuilder::visitSimpleStatement1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // ReturnStatement -> stmt
}
void TinyNodeBuilder::visitSimpleStatement2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // ExpressionStatement -> stmt
}
void TinyNodeBuilder::visitSimpleStatement3(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // Block -> block
    stmt = block;
}
void TinyNodeBuilder::visitSimpleStatement4(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // LocalVarDecl -> stmt
}
void TinyNodeBuilder::visitSimpleStatement5(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // AssignStatement -> stmt
}

// ---- Return ----
void TinyNodeBuilder::visitReturnStatement1(shared_ptr<TinyNonterminalNode> node) {
    stmt = make_shared<ReturnStmt>(nullptr);
}
void TinyNodeBuilder::visitReturnStatement2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(1)->accept(shared_from_this());   // Expression -> expr
    stmt = make_shared<ReturnStmt>(expr);
}

// ---- ExpressionStatement ----
void TinyNodeBuilder::visitExpressionStatement1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // Expression -> expr
    stmt = make_shared<ExprStmt>(expr);
}

// ---- LocalVarDecl ----
void TinyNodeBuilder::visitLocalVarDecl1(shared_ptr<TinyNonterminalNode> node) {
    // Type IDENTIFIER SEMI
    node->rhs->at(0)->accept(shared_from_this());   // Type -> type
    Type t = type;
    node->rhs->at(1)->accept(shared_from_this());   // IDENTIFIER
    stmt = make_shared<LocalVarDecl>(t, tokenText(terminal), nullptr);
}
void TinyNodeBuilder::visitLocalVarDecl2(shared_ptr<TinyNonterminalNode> node) {
    // Type IDENTIFIER EQ Expression SEMI
    node->rhs->at(0)->accept(shared_from_this());   // Type
    Type t = type;
    node->rhs->at(1)->accept(shared_from_this());   // IDENTIFIER
    string name = tokenText(terminal);
    node->rhs->at(3)->accept(shared_from_this());   // Expression -> expr
    stmt = make_shared<LocalVarDecl>(t, name, expr);
}

// ---- AssignStatement ----
void TinyNodeBuilder::visitAssignStatement1(shared_ptr<TinyNonterminalNode> node) {
    // LValue EQ Expression SEMI
    node->rhs->at(0)->accept(shared_from_this());   // LValue -> fills lvalue slots
    node->rhs->at(2)->accept(shared_from_this());   // Expression -> expr
    if (lvalueIsArray) {
        stmt = make_shared<AssignStmt>(lvalueName, lvalueIndex, expr);
    } else {
        stmt = make_shared<AssignStmt>(lvalueName, expr);
    }
}

// ---- LValue ----
void TinyNodeBuilder::visitLValue1(shared_ptr<TinyNonterminalNode> node) {
    // IDENTIFIER
    node->rhs->at(0)->accept(shared_from_this());
    lvalueIsArray = false;
    lvalueName = tokenText(terminal);
    lvalueIndex = nullptr;
}
void TinyNodeBuilder::visitLValue2(shared_ptr<TinyNonterminalNode> node) {
    // IDENTIFIER LBRACK Expression RBRACK
    node->rhs->at(0)->accept(shared_from_this());   // IDENTIFIER
    lvalueName = tokenText(terminal);
    node->rhs->at(2)->accept(shared_from_this());   // Expression -> expr (index)
    lvalueIsArray = true;
    lvalueIndex = expr;
}

// ---- Expression layers (pass-through unless real operator) ----
void TinyNodeBuilder::visitExpression1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // LogicalOrExpression -> expr
}

// ---- LogicalOrExpression (pass-through / binary OR) ----
void TinyNodeBuilder::visitLogicalOrExpression1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // LogicalAndExpression -> expr
}
void TinyNodeBuilder::visitLogicalOrExpression2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // LogicalOrExpression -> expr
    auto left = expr;
    node->rhs->at(2)->accept(shared_from_this());   // LogicalAndExpression -> expr
    expr = make_shared<BinaryExpr>(BinaryExpr::OR, left, expr);
}

// ---- LogicalAndExpression (pass-through / binary AND) ----
void TinyNodeBuilder::visitLogicalAndExpression1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // EqualityExpression -> expr
}
void TinyNodeBuilder::visitLogicalAndExpression2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // LogicalAndExpression -> expr
    auto left = expr;
    node->rhs->at(2)->accept(shared_from_this());   // EqualityExpression -> expr
    expr = make_shared<BinaryExpr>(BinaryExpr::AND, left, expr);
}

void TinyNodeBuilder::visitEqualityExpression1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // RelationalExpression -> expr
}
void TinyNodeBuilder::visitEqualityExpression2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());
    auto left = expr;
    node->rhs->at(2)->accept(shared_from_this());
    expr = make_shared<BinaryExpr>(BinaryExpr::EQ_EQ, left, expr);
}
void TinyNodeBuilder::visitEqualityExpression3(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());
    auto left = expr;
    node->rhs->at(2)->accept(shared_from_this());
    expr = make_shared<BinaryExpr>(BinaryExpr::NE, left, expr);
}

void TinyNodeBuilder::visitRelationalExpression1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // AdditiveExpression -> expr
}
void TinyNodeBuilder::visitRelationalExpression2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // RelationalExpression -> expr
    auto left = expr;
    node->rhs->at(2)->accept(shared_from_this());   // AdditiveExpression -> expr
    auto right = expr;
    expr = make_shared<BinaryExpr>(BinaryExpr::LT, left, right);
}
void TinyNodeBuilder::visitRelationalExpression3(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());
    auto left = expr;
    node->rhs->at(2)->accept(shared_from_this());
    expr = make_shared<BinaryExpr>(BinaryExpr::GT, left, expr);
}
void TinyNodeBuilder::visitRelationalExpression4(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());
    auto left = expr;
    node->rhs->at(2)->accept(shared_from_this());
    expr = make_shared<BinaryExpr>(BinaryExpr::LE, left, expr);
}
void TinyNodeBuilder::visitRelationalExpression5(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());
    auto left = expr;
    node->rhs->at(2)->accept(shared_from_this());
    expr = make_shared<BinaryExpr>(BinaryExpr::GE, left, expr);
}

void TinyNodeBuilder::visitAdditiveExpression1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // MultiplicativeExpression -> expr
}
void TinyNodeBuilder::visitAdditiveExpression2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // AdditiveExpression -> expr
    auto left = expr;
    node->rhs->at(2)->accept(shared_from_this());   // MultiplicativeExpression -> expr
    auto right = expr;
    expr = make_shared<BinaryExpr>(BinaryExpr::ADD, left, right);
}
void TinyNodeBuilder::visitAdditiveExpression3(std::shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // AdditiveExpression -> expr
    auto left = expr;
    node->rhs->at(2)->accept(shared_from_this());   // MultiplicativeExpression -> expr
    auto right = expr;
    expr = make_shared<BinaryExpr>(BinaryExpr::SUB, left, right);
}

void TinyNodeBuilder::visitMultiplicativeExpression1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // PrimaryExpression -> expr
}
void TinyNodeBuilder::visitMultiplicativeExpression2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // MultiplicativeExpression -> expr
    auto left = expr;
    node->rhs->at(2)->accept(shared_from_this());   // PrimaryExpression -> expr
    expr = make_shared<BinaryExpr>(BinaryExpr::MUL, left, expr);
}
void TinyNodeBuilder::visitMultiplicativeExpression3(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // MultiplicativeExpression -> expr
    auto left = expr;
    node->rhs->at(2)->accept(shared_from_this());   // PrimaryExpression -> expr
    expr = make_shared<BinaryExpr>(BinaryExpr::DIV, left, expr);
}
void TinyNodeBuilder::visitMultiplicativeExpression4(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // MultiplicativeExpression -> expr
    auto left = expr;
    node->rhs->at(2)->accept(shared_from_this());   // PrimaryExpression -> expr
    expr = make_shared<BinaryExpr>(BinaryExpr::MOD, left, expr);
}

// ---- PrimaryExpression ----
void TinyNodeBuilder::visitPrimaryExpression1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // DECIMAL_INT (terminal)
    expr = make_shared<IntLiteral>(stoi(tokenText(terminal)));
}
void TinyNodeBuilder::visitPrimaryExpression2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // IDENTIFIER (terminal)
    expr = make_shared<VarRef>(tokenText(terminal));
}
void TinyNodeBuilder::visitPrimaryExpression3(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(1)->accept(shared_from_this());   // LPAREN Expression RPAREN -> expr
}
void TinyNodeBuilder::visitPrimaryExpression4(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // CallExpression -> expr
}
void TinyNodeBuilder::visitPrimaryExpression5(shared_ptr<TinyNonterminalNode> node) {
    // NEW INT LBRACK Expression RBRACK
    node->rhs->at(3)->accept(shared_from_this());   // Expression -> expr (size)
    expr = make_shared<NewArray>(expr);
}
void TinyNodeBuilder::visitPrimaryExpression6(shared_ptr<TinyNonterminalNode> node) {
    // IDENTIFIER LBRACK Expression RBRACK
    node->rhs->at(0)->accept(shared_from_this());   // IDENTIFIER
    string name = tokenText(terminal);
    node->rhs->at(2)->accept(shared_from_this());   // Expression -> expr (index)
    expr = make_shared<ArrayIndex>(name, expr);
}
void TinyNodeBuilder::visitPrimaryExpression7(shared_ptr<TinyNonterminalNode> node) {
    // BANG PrimaryExpression
    node->rhs->at(1)->accept(shared_from_this());   // PrimaryExpression -> expr
    expr = make_shared<NotExpr>(expr);
}

// ---- CallExpression ----
void TinyNodeBuilder::visitCallExpression1(shared_ptr<TinyNonterminalNode> node) {
    // IDENTIFIER LPAREN RPAREN
    node->rhs->at(0)->accept(shared_from_this());   // IDENTIFIER
    string callee = tokenText(terminal);
    expr = make_shared<CallExpr>(callee, make_shared<vector<shared_ptr<Expr>>>());
}
void TinyNodeBuilder::visitCallExpression2(shared_ptr<TinyNonterminalNode> node) {
    // IDENTIFIER LPAREN ArgumentList RPAREN
    node->rhs->at(0)->accept(shared_from_this());   // IDENTIFIER
    string callee = tokenText(terminal);
    node->rhs->at(2)->accept(shared_from_this());   // ArgumentList -> argList
    expr = make_shared<CallExpr>(callee, argList);
}

// ---- ArgumentList (left-recursive) ----
void TinyNodeBuilder::visitArgumentList1(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // Expression -> expr
    auto list = make_shared<vector<shared_ptr<Expr>>>();
    list->push_back(expr);
    argList = list;
}
void TinyNodeBuilder::visitArgumentList2(shared_ptr<TinyNonterminalNode> node) {
    node->rhs->at(0)->accept(shared_from_this());   // ArgumentList -> argList
    auto list = argList;
    node->rhs->at(2)->accept(shared_from_this());   // Expression -> expr
    list->push_back(expr);
    argList = list;
}

}
