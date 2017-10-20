
// LogicalExpressionParser
// Parser.cpp

#include "Parser.hpp"
#include "AST.hpp"
#include "Token.hpp"

std::shared_ptr<BaseAST> Parser::Parse()
{
    assert(this->mTokenStream != nullptr);
    
    std::shared_ptr<BaseAST> logicalExprAST = this->VisitExpression();

    // 解析していないトークンが残っている場合はエラーを返す
    if (this->mTokenStream->CurrentToken() != nullptr)
        return nullptr;

    return logicalExprAST;
}

std::shared_ptr<BaseAST> Parser::VisitFactor()
{
    /*
    <Factor> ::= 
        <Constant> |
        <Variable> |
        '(' <Expression> ')'
    */

    if (this->mTokenStream->CurrentToken() == nullptr)
        return nullptr;

    if ((this->mTokenStream->CurrentToken()->Type() == TokenType::True) ||
        (this->mTokenStream->CurrentToken()->Type() == TokenType::False))
        return this->VisitConstant();

    if (this->mTokenStream->CurrentToken()->Type() == TokenType::Variable)
        return this->VisitVariable();

    if (this->mTokenStream->CurrentToken()->Type() == TokenType::LeftParenthesis) {
        if (!this->mTokenStream->MoveNext())
            return nullptr;

        std::shared_ptr<BaseAST> exprAST = this->VisitExpression();

        if (exprAST == nullptr)
            return nullptr;

        // 対応する右括弧が存在しない (既にトークンの末尾に達している)
        if (this->mTokenStream->CurrentToken() == nullptr)
            return nullptr;

        if (this->mTokenStream->CurrentToken()->Type() != TokenType::RightParenthesis)
            return nullptr;

        this->mTokenStream->MoveNext();

        return std::make_shared<FactorAST>(exprAST);
    }

    return nullptr;
}

std::shared_ptr<BaseAST> Parser::VisitExpression()
{
    /*
    <Expression> ::=
        <AndOrExpression> |
        <AndOrExpression> '->' <AndOrExpression> |
        <AndOrExpression> '<->' <AndOrExpression>
    */

    std::shared_ptr<BaseAST> leftAndOrExprAST = this->VisitAndOrExpression();

    if (leftAndOrExprAST == nullptr)
        return nullptr;

    if (this->mTokenStream->CurrentToken() == nullptr)
        return leftAndOrExprAST;

    if ((this->mTokenStream->CurrentToken()->Type() == TokenType::Then) ||
        (this->mTokenStream->CurrentToken()->Type() == TokenType::Eq)) {
        std::string op = (this->mTokenStream->CurrentToken()->Type() == TokenType::Then) ? "->" : "<->";

        if (!this->mTokenStream->MoveNext())
            return nullptr;

        std::shared_ptr<BaseAST> rightAndOrExprAST = this->VisitAndOrExpression();

        if (rightAndOrExprAST == nullptr)
            return nullptr;

        return std::make_shared<ExpressionAST>(leftAndOrExprAST, rightAndOrExprAST, op);
    } else {
        return leftAndOrExprAST;
    }
}

std::shared_ptr<BaseAST> Parser::VisitAndOrExpression()
{
    /*
    <AndOrExpression> ::=
        <NotExpression> |
        <NotExpression> 'And' <NotExpression> |
        <NotExpression> 'Or' <NotExpression>
    */

    std::shared_ptr<BaseAST> leftNotExprAST = this->VisitNotExpression();

    if (leftNotExprAST == nullptr)
        return nullptr;

    if (this->mTokenStream->CurrentToken() == nullptr)
        return leftNotExprAST;

    if ((this->mTokenStream->CurrentToken()->Type() == TokenType::And) ||
        (this->mTokenStream->CurrentToken()->Type() == TokenType::Or)) {
        std::string op = (this->mTokenStream->CurrentToken()->Type() == TokenType::And) ? "∧" : "∨";

        if (!this->mTokenStream->MoveNext())
            return nullptr;

        std::shared_ptr<BaseAST> rightNotExprAST = this->VisitNotExpression();

        if (rightNotExprAST == nullptr)
            return nullptr;

        return std::make_shared<AndOrExpressionAST>(leftNotExprAST, rightNotExprAST, op);
    } else {
        return leftNotExprAST;
    }
}

std::shared_ptr<BaseAST> Parser::VisitNotExpression()
{
    /*
    <NotExpression> ::=
        <Factor> |
        'Not' <Factor>
    */

    if (this->mTokenStream->CurrentToken() == nullptr)
        return nullptr;

    if (this->mTokenStream->CurrentToken()->Type() == TokenType::Not) {
        if (!this->mTokenStream->MoveNext())
            return nullptr;

        std::shared_ptr<BaseAST> factorAST = this->VisitFactor();

        if (factorAST == nullptr)
            return nullptr;

        return std::make_shared<NotExpressionAST>(factorAST);
    } else {
        return this->VisitFactor();
    }
}

std::shared_ptr<ConstantAST> Parser::VisitConstant()
{
    /*
    <Constant> ::= 'T' | 'F'
    */

    if (this->mTokenStream->CurrentToken() == nullptr)
        return nullptr;

    // 現在のトークンが定数であると仮定している
    if ((this->mTokenStream->CurrentToken()->Type() != TokenType::True) &&
        (this->mTokenStream->CurrentToken()->Type() != TokenType::False))
        return nullptr;

    // トークンが定数なのに値を含まない場合はエラーを返す
    if (!this->mTokenStream->CurrentToken()->HasValue())
        return nullptr;

    // Constantを作成
    std::shared_ptr<ConstantAST> newAST = std::make_shared<ConstantAST>(this->mTokenStream->CurrentToken()->Value());

    // トークンを1つ次に進める
    this->mTokenStream->MoveNext();

    return newAST;
}

std::shared_ptr<VariableAST> Parser::VisitVariable()
{
    /*
    <Variable> ::= 'P' | 'Q' | 'R' | 'P1' | 'P2' | ...
    */

    if (this->mTokenStream->CurrentToken() == nullptr)
        return nullptr;

    // 現在のトークンが変数であると仮定している
    if (this->mTokenStream->CurrentToken()->Type() != TokenType::Variable)
        return nullptr;

    // トークンが変数なのに変数名が無い場合はエラーを返す
    if (this->mTokenStream->CurrentToken()->Text().empty())
        return nullptr;

    // Variableを作成
    std::shared_ptr<VariableAST> newAST = std::make_shared<VariableAST>(this->mTokenStream->CurrentToken()->Text());

    // トークンを1つ次に進める
    this->mTokenStream->MoveNext();

    return newAST;
}
