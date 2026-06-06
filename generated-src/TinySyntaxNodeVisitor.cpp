#include "TinySyntaxNodeVisitor.h"
#include "TinyNonterminalNode.h"
#include "TinyTerminalNode.h"

namespace tiny {

TinySyntaxNodeVisitor::TinySyntaxNodeVisitor() {
}

TinySyntaxNodeVisitor::~TinySyntaxNodeVisitor() {
}

void (TinySyntaxNodeVisitor::*TinySyntaxNodeVisitor::VISIT_HANDLERS[60])(std::shared_ptr<TinyNonterminalNode> node) ={
  &TinySyntaxNodeVisitor::visitProgram1,
  &TinySyntaxNodeVisitor::visitProgram2,
  &TinySyntaxNodeVisitor::visitFunctionDefinition1,
  &TinySyntaxNodeVisitor::visitFunctionDefinition2,
  &TinySyntaxNodeVisitor::visitType1,
  &TinySyntaxNodeVisitor::visitType2,
  &TinySyntaxNodeVisitor::visitParameterList1,
  &TinySyntaxNodeVisitor::visitParameterList2,
  &TinySyntaxNodeVisitor::visitParameter1,
  &TinySyntaxNodeVisitor::visitBlock1,
  &TinySyntaxNodeVisitor::visitBlock2,
  &TinySyntaxNodeVisitor::visitStatementList1,
  &TinySyntaxNodeVisitor::visitStatementList2,
  &TinySyntaxNodeVisitor::visitStatement1,
  &TinySyntaxNodeVisitor::visitStatement2,
  &TinySyntaxNodeVisitor::visitOpenStatement1,
  &TinySyntaxNodeVisitor::visitOpenStatement2,
  &TinySyntaxNodeVisitor::visitOpenStatement3,
  &TinySyntaxNodeVisitor::visitCloseStatement1,
  &TinySyntaxNodeVisitor::visitCloseStatement2,
  &TinySyntaxNodeVisitor::visitCloseStatement3,
  &TinySyntaxNodeVisitor::visitSimpleStatement1,
  &TinySyntaxNodeVisitor::visitSimpleStatement2,
  &TinySyntaxNodeVisitor::visitSimpleStatement3,
  &TinySyntaxNodeVisitor::visitSimpleStatement4,
  &TinySyntaxNodeVisitor::visitSimpleStatement5,
  &TinySyntaxNodeVisitor::visitReturnStatement1,
  &TinySyntaxNodeVisitor::visitReturnStatement2,
  &TinySyntaxNodeVisitor::visitExpressionStatement1,
  &TinySyntaxNodeVisitor::visitLocalVarDecl1,
  &TinySyntaxNodeVisitor::visitLocalVarDecl2,
  &TinySyntaxNodeVisitor::visitAssignStatement1,
  &TinySyntaxNodeVisitor::visitLValue1,
  &TinySyntaxNodeVisitor::visitLValue2,
  &TinySyntaxNodeVisitor::visitExpression1,
  &TinySyntaxNodeVisitor::visitEqualityExpression1,
  &TinySyntaxNodeVisitor::visitEqualityExpression2,
  &TinySyntaxNodeVisitor::visitEqualityExpression3,
  &TinySyntaxNodeVisitor::visitRelationalExpression1,
  &TinySyntaxNodeVisitor::visitRelationalExpression2,
  &TinySyntaxNodeVisitor::visitRelationalExpression3,
  &TinySyntaxNodeVisitor::visitRelationalExpression4,
  &TinySyntaxNodeVisitor::visitRelationalExpression5,
  &TinySyntaxNodeVisitor::visitAdditiveExpression1,
  &TinySyntaxNodeVisitor::visitAdditiveExpression2,
  &TinySyntaxNodeVisitor::visitAdditiveExpression3,
  &TinySyntaxNodeVisitor::visitMultiplicativeExpression1,
  &TinySyntaxNodeVisitor::visitMultiplicativeExpression2,
  &TinySyntaxNodeVisitor::visitMultiplicativeExpression3,
  &TinySyntaxNodeVisitor::visitMultiplicativeExpression4,
  &TinySyntaxNodeVisitor::visitPrimaryExpression1,
  &TinySyntaxNodeVisitor::visitPrimaryExpression2,
  &TinySyntaxNodeVisitor::visitPrimaryExpression3,
  &TinySyntaxNodeVisitor::visitPrimaryExpression4,
  &TinySyntaxNodeVisitor::visitPrimaryExpression5,
  &TinySyntaxNodeVisitor::visitPrimaryExpression6,
  &TinySyntaxNodeVisitor::visitCallExpression1,
  &TinySyntaxNodeVisitor::visitCallExpression2,
  &TinySyntaxNodeVisitor::visitArgumentList1,
  &TinySyntaxNodeVisitor::visitArgumentList2
};

void TinySyntaxNodeVisitor::dispatch(std::shared_ptr<TinyNonterminalNode> node) {
  (this->*VISIT_HANDLERS[node->productionId])(node);
}

void TinySyntaxNodeVisitor::handleTerminal(std::shared_ptr<TinyTerminalNode> node) {
}

void TinySyntaxNodeVisitor::visitProgram1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitProgram2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitFunctionDefinition1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitFunctionDefinition2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitType1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitType2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitParameterList1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitParameterList2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitParameter1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitBlock1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitBlock2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitStatementList1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitStatementList2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitStatement1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitStatement2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitOpenStatement1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitOpenStatement2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitOpenStatement3(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitCloseStatement1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitCloseStatement2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitCloseStatement3(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitSimpleStatement1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitSimpleStatement2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitSimpleStatement3(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitSimpleStatement4(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitSimpleStatement5(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitReturnStatement1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitReturnStatement2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitExpressionStatement1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitLocalVarDecl1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitLocalVarDecl2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitAssignStatement1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitLValue1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitLValue2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitExpression1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitEqualityExpression1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitEqualityExpression2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitEqualityExpression3(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitRelationalExpression1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitRelationalExpression2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitRelationalExpression3(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitRelationalExpression4(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitRelationalExpression5(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitAdditiveExpression1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitAdditiveExpression2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitAdditiveExpression3(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitMultiplicativeExpression1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitMultiplicativeExpression2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitMultiplicativeExpression3(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitMultiplicativeExpression4(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitPrimaryExpression1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitPrimaryExpression2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitPrimaryExpression3(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitPrimaryExpression4(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitPrimaryExpression5(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitPrimaryExpression6(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitCallExpression1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitCallExpression2(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitArgumentList1(std::shared_ptr<TinyNonterminalNode> node) {
}

void TinySyntaxNodeVisitor::visitArgumentList2(std::shared_ptr<TinyNonterminalNode> node) {
}
}

