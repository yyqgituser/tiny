#ifndef _TINY_TINYSYNTAXNODEVISITOR_H
#define _TINY_TINYSYNTAXNODEVISITOR_H

#include <memory>

namespace tiny {

class TinyNonterminalNode;
class TinyTerminalNode;

class TinySyntaxNodeVisitor {
private:
	static void (TinySyntaxNodeVisitor::*VISIT_HANDLERS[])(std::shared_ptr<TinyNonterminalNode> node);

public:
  TinySyntaxNodeVisitor();

  virtual ~TinySyntaxNodeVisitor();

  void dispatch(std::shared_ptr<TinyNonterminalNode> node);

  virtual void handleTerminal(std::shared_ptr<TinyTerminalNode> node);

  virtual void visitProgram1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitProgram2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitFunctionDefinition1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitFunctionDefinition2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitType1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitType2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitParameterList1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitParameterList2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitParameter1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitBlock1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitBlock2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitStatementList1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitStatementList2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitStatement1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitStatement2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitOpenStatement1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitOpenStatement2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitOpenStatement3(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitCloseStatement1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitCloseStatement2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitCloseStatement3(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitSimpleStatement1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitSimpleStatement2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitSimpleStatement3(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitSimpleStatement4(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitSimpleStatement5(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitReturnStatement1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitReturnStatement2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitExpressionStatement1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitLocalVarDecl1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitLocalVarDecl2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitAssignStatement1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitLValue1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitLValue2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitExpression1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitEqualityExpression1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitEqualityExpression2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitEqualityExpression3(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitRelationalExpression1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitRelationalExpression2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitRelationalExpression3(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitRelationalExpression4(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitRelationalExpression5(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitAdditiveExpression1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitAdditiveExpression2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitAdditiveExpression3(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitMultiplicativeExpression1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitMultiplicativeExpression2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitMultiplicativeExpression3(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitMultiplicativeExpression4(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitPrimaryExpression1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitPrimaryExpression2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitPrimaryExpression3(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitPrimaryExpression4(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitPrimaryExpression5(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitPrimaryExpression6(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitCallExpression1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitCallExpression2(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitArgumentList1(std::shared_ptr<TinyNonterminalNode> node);

  virtual void visitArgumentList2(std::shared_ptr<TinyNonterminalNode> node);
};

}

#endif
