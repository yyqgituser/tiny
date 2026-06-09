#ifndef BUILDER_TINYNODEBUILDER_H_
#define BUILDER_TINYNODEBUILDER_H_

#include <memory>
#include <string>
#include <vector>

#include "TinySyntaxNodeVisitor.h"
#include "TinyTerminalNode.h"
#include "TinyNonterminalNode.h"

#include "ast/Decl.h"
#include "ast/Stmt.h"
#include "ast/Expr.h"
#include "ast/Type.h"

namespace builder {

// Converts a parser syntax tree (generic NT/T nodes) into the tiny semantic AST.
// Same pattern as hila's HilaNodeBuilder: override visit{Production}, use member
// "result slots" filled by visiting children, consume them after.
class TinyNodeBuilder : public tiny::TinySyntaxNodeVisitor,
                        public std::enable_shared_from_this<TinyNodeBuilder> {
public:
    TinyNodeBuilder();
    ~TinyNodeBuilder() override;

    std::shared_ptr<ast::Program> getResult() { return result; }

    void handleTerminal(std::shared_ptr<tiny::TinyTerminalNode> node) override;

    void visitProgram1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitProgram2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitFunctionDefinition1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitFunctionDefinition2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitType1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitType2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitParameterList1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitParameterList2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitParameter1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitBlock1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitBlock2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitStatementList1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitStatementList2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitStatement1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitStatement2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitOpenStatement1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitOpenStatement2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitOpenStatement3(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitCloseStatement1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitCloseStatement2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitCloseStatement3(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitSimpleStatement1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitSimpleStatement2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitSimpleStatement3(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitSimpleStatement4(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitSimpleStatement5(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitReturnStatement1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitReturnStatement2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitExpressionStatement1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitLocalVarDecl1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitLocalVarDecl2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitAssignStatement1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitLValue1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitLValue2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitExpression1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitLogicalOrExpression1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitLogicalOrExpression2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitLogicalAndExpression1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitLogicalAndExpression2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitEqualityExpression1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitEqualityExpression2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitEqualityExpression3(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitRelationalExpression1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitRelationalExpression2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitRelationalExpression3(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitRelationalExpression4(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitRelationalExpression5(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitAdditiveExpression1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitAdditiveExpression2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitAdditiveExpression3(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitMultiplicativeExpression1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitMultiplicativeExpression2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitMultiplicativeExpression3(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitMultiplicativeExpression4(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitPrimaryExpression1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitPrimaryExpression2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitPrimaryExpression3(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitPrimaryExpression4(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitPrimaryExpression5(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitPrimaryExpression6(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitPrimaryExpression7(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitCallExpression1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitCallExpression2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitArgumentList1(std::shared_ptr<tiny::TinyNonterminalNode> node) override;
    void visitArgumentList2(std::shared_ptr<tiny::TinyNonterminalNode> node) override;

private:
    std::shared_ptr<ast::Program> result;

    // result slots (consumed immediately by callers)
    std::shared_ptr<tiny::TinyTerminalNode> terminal;
    ast::Type type;
    std::shared_ptr<ast::Expr> expr;
    std::shared_ptr<ast::Stmt> stmt;
    std::shared_ptr<ast::Block> block;
    std::shared_ptr<ast::FunctionDef> function;
    std::shared_ptr<ast::Param> param;
    std::shared_ptr<std::vector<std::shared_ptr<ast::FunctionDef>>> functionList;
    std::shared_ptr<std::vector<std::shared_ptr<ast::Stmt>>> stmtList;
    std::shared_ptr<std::vector<std::shared_ptr<ast::Param>>> paramList;
    std::shared_ptr<std::vector<std::shared_ptr<ast::Expr>>> argList;

    std::string tokenText(std::shared_ptr<tiny::TinyTerminalNode> t);

    // LValue temp slots
    bool lvalueIsArray;
    std::string lvalueName;
    std::shared_ptr<ast::Expr> lvalueIndex;
};

}
#endif
