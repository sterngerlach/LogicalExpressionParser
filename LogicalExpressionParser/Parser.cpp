
// LogicalExpressionParser
// Parser.cpp

#include "Parser.hpp"
#include "AST.hpp"
#include "Token.hpp"

//
// Parserクラス
//

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

//
// InfixParserクラス
//

std::shared_ptr<BaseAST> InfixParser::Parse()
{
    assert(this->mTokenStream != nullptr);
    
    std::shared_ptr<BaseAST> logicalExprAST = this->VisitExpression();

    // 解析していないトークンが残っている場合はエラーを返す
    if (this->mTokenStream->CurrentToken() != nullptr)
        return nullptr;

    return logicalExprAST;
}

std::shared_ptr<BaseAST> InfixParser::VisitFactor()
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

std::shared_ptr<BaseAST> InfixParser::VisitExpression()
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

std::shared_ptr<BaseAST> InfixParser::VisitAndOrExpression()
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

std::shared_ptr<BaseAST> InfixParser::VisitNotExpression()
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

//
// PrefixParserクラス
//

std::shared_ptr<BaseAST> PrefixParser::Parse()
{
    assert(this->mTokenStream != nullptr);

    if (this->mTokenStream->CurrentToken() == nullptr)
        return nullptr;

    std::shared_ptr<BaseAST> logicalExprAST;

    switch (this->mTokenStream->CurrentToken()->Type()) {
        case TokenType::True:
        case TokenType::False:
            logicalExprAST = this->VisitConstant();
            break;
        case TokenType::Variable:
            logicalExprAST = this->VisitVariable();
            break;
        case TokenType::And:
        case TokenType::Or:
            logicalExprAST = this->VisitAndOrExpression();
            break;
        case TokenType::Not:
            logicalExprAST = this->VisitNotExpression();
            break;
        case TokenType::Then:
        case TokenType::Eq:
            logicalExprAST = this->VisitThenOrEqExpression();
            break;
        default:
            return nullptr;
    }

    // 解析していないトークンが残っている場合はエラーを返す
    if (this->mTokenStream->CurrentToken() != nullptr)
        return nullptr;

    return logicalExprAST;
}

std::shared_ptr<BaseAST> PrefixParser::VisitThenOrEqExpression()
{
    if (this->mTokenStream->CurrentToken() == nullptr)
        return nullptr;

    std::shared_ptr<ExpressionAST> exprAST = std::make_shared<ExpressionAST>();

    // 演算子のトークン
    if (this->mTokenStream->CurrentToken()->Type() == TokenType::Then ||
        this->mTokenStream->CurrentToken()->Type() == TokenType::Eq) {
        std::string op = (this->mTokenStream->CurrentToken()->Type() == TokenType::Then) ? "->" : "<->";

        // 次のトークンが無い場合はエラーを返す
        if (!this->mTokenStream->MoveNext())
            return nullptr;

        exprAST->SetOperator(op);
    } else {
        return nullptr;
    }

    // 左部分木
    switch (this->mTokenStream->CurrentToken()->Type()) {
        case TokenType::True:
        case TokenType::False:
            exprAST->SetLeft(this->VisitConstant());
            break;
        case TokenType::Variable:
            exprAST->SetLeft(this->VisitVariable());
            break;
        case TokenType::And:
        case TokenType::Or:
            exprAST->SetLeft(this->VisitAndOrExpression());
            break;
        case TokenType::Not:
            exprAST->SetLeft(this->VisitNotExpression());
            break;
        case TokenType::Then:
        case TokenType::Eq:
            exprAST->SetLeft(this->VisitThenOrEqExpression());
            break;
        default:
            return nullptr;
    }

    if (exprAST->Left() == nullptr)
        return nullptr;

    // 次のトークンが無い場合はエラーを返す
    if (this->mTokenStream->CurrentToken() == nullptr)
        return nullptr;

    // 右部分木
    switch (this->mTokenStream->CurrentToken()->Type()) {
        case TokenType::True:
        case TokenType::False:
            exprAST->SetRight(this->VisitConstant());
            break;
        case TokenType::Variable:
            exprAST->SetRight(this->VisitVariable());
            break;
        case TokenType::And:
        case TokenType::Or:
            exprAST->SetRight(this->VisitAndOrExpression());
            break;
        case TokenType::Not:
            exprAST->SetRight(this->VisitNotExpression());
            break;
        case TokenType::Then:
        case TokenType::Eq:
            exprAST->SetRight(this->VisitThenOrEqExpression());
            break;
        default:
            return nullptr;
    }

    if (exprAST->Right() == nullptr)
        return nullptr;

    return exprAST;
}

std::shared_ptr<BaseAST> PrefixParser::VisitAndOrExpression()
{
    if (this->mTokenStream->CurrentToken() == nullptr)
        return nullptr;

    std::shared_ptr<AndOrExpressionAST> andOrExprAST = std::make_shared<AndOrExpressionAST>();

    // 演算子のトークン
    if (this->mTokenStream->CurrentToken()->Type() == TokenType::And ||
        this->mTokenStream->CurrentToken()->Type() == TokenType::Or) {
        std::string op = (this->mTokenStream->CurrentToken()->Type() == TokenType::And) ? "∧" : "∨";

        // 次のトークンが無い場合はエラーを返す
        if (!this->mTokenStream->MoveNext())
            return nullptr;

        andOrExprAST->SetOperator(op);
    } else {
        return nullptr;
    }

    // 左部分木
    switch (this->mTokenStream->CurrentToken()->Type()) {
        case TokenType::True:
        case TokenType::False:
            andOrExprAST->SetLeft(this->VisitConstant());
            break;
        case TokenType::Variable:
            andOrExprAST->SetLeft(this->VisitVariable());
            break;
        case TokenType::And:
        case TokenType::Or:
            andOrExprAST->SetLeft(this->VisitAndOrExpression());
            break;
        case TokenType::Not:
            andOrExprAST->SetLeft(this->VisitNotExpression());
            break;
        case TokenType::Then:
        case TokenType::Eq:
            andOrExprAST->SetLeft(this->VisitThenOrEqExpression());
            break;
        default:
            return nullptr;
    }

    if (andOrExprAST->Left() == nullptr)
        return nullptr;

    // 次のトークンが無い場合はエラーを返す
    if (this->mTokenStream->CurrentToken() == nullptr)
        return nullptr;

    // 右部分木
    switch (this->mTokenStream->CurrentToken()->Type()) {
        case TokenType::True:
        case TokenType::False:
            andOrExprAST->SetRight(this->VisitConstant());
            break;
        case TokenType::Variable:
            andOrExprAST->SetRight(this->VisitVariable());
            break;
        case TokenType::And:
        case TokenType::Or:
            andOrExprAST->SetRight(this->VisitAndOrExpression());
            break;
        case TokenType::Not:
            andOrExprAST->SetRight(this->VisitNotExpression());
            break;
        case TokenType::Then:
        case TokenType::Eq:
            andOrExprAST->SetRight(this->VisitThenOrEqExpression());
            break;
        default:
            return nullptr;
    }

    if (andOrExprAST->Right() == nullptr)
        return nullptr;

    return andOrExprAST;
}

std::shared_ptr<BaseAST> PrefixParser::VisitNotExpression()
{
    if (this->mTokenStream->CurrentToken() == nullptr)
        return nullptr;

    // 演算子のトークン
    if (this->mTokenStream->CurrentToken()->Type() != TokenType::Not)
        return nullptr;

    // 次のトークンが無い場合はエラーを返す
    if (!this->mTokenStream->MoveNext())
        return nullptr;

    std::shared_ptr<NotExpressionAST> notExprAST = std::make_shared<NotExpressionAST>();

    // オペランド
    switch (this->mTokenStream->CurrentToken()->Type()) {
        case TokenType::True:
        case TokenType::False:
            notExprAST->SetExpr(this->VisitConstant());
            break;
        case TokenType::Variable:
            notExprAST->SetExpr(this->VisitVariable());
            break;
        case TokenType::And:
        case TokenType::Or:
            notExprAST->SetExpr(this->VisitAndOrExpression());
            break;
        case TokenType::Not:
            notExprAST->SetExpr(this->VisitNotExpression());
            break;
        case TokenType::Then:
        case TokenType::Eq:
            notExprAST->SetExpr(this->VisitThenOrEqExpression());
            break;
        default:
            return nullptr;
    }

    if (notExprAST->Expr() == nullptr)
        return nullptr;

    return notExprAST;
}

//
// PostfixParserクラス
//
std::shared_ptr<BaseAST> PostfixParser::Parse()
{
    assert(this->mTokenStream != nullptr);

    if (this->mTokenStream->CurrentToken() == nullptr)
        return nullptr;

    while (this->mTokenStream->CurrentToken() != nullptr) {
        switch (this->mTokenStream->CurrentToken()->Type()) {
            case TokenType::True:
            case TokenType::False:
            {
                this->mASTStack.push(this->VisitConstant());
                continue;
            }
            case TokenType::Variable:
            {
                this->mASTStack.push(this->VisitVariable());
                continue;
            }
            case TokenType::And:
            case TokenType::Or:
            {
                if (this->mASTStack.empty())
                    return nullptr;

                std::shared_ptr<BaseAST> rightExprAST = this->mASTStack.top();
                this->mASTStack.pop();

                if (this->mASTStack.empty())
                    return nullptr;

                std::shared_ptr<BaseAST> leftExprAST = this->mASTStack.top();
                this->mASTStack.pop();

                std::shared_ptr<AndOrExpressionAST> andOrExprAST = std::make_shared<AndOrExpressionAST>();

                andOrExprAST->SetOperator((this->mTokenStream->CurrentToken()->Type() == TokenType::And) ? "∧" : "∨");
                andOrExprAST->SetLeft(leftExprAST);
                andOrExprAST->SetRight(rightExprAST);

                this->mASTStack.push(andOrExprAST);
                break;
            }
            case TokenType::Not:
            {
                if (this->mASTStack.empty())
                    return nullptr;

                std::shared_ptr<BaseAST> exprAST = this->mASTStack.top();
                this->mASTStack.pop();

                std::shared_ptr<NotExpressionAST> notExprAST = std::make_shared<NotExpressionAST>();
                notExprAST->SetExpr(exprAST);

                this->mASTStack.push(notExprAST);
                break;
            }
            case TokenType::Then:
            case TokenType::Eq:
            {
                if (this->mASTStack.empty())
                    return nullptr;

                std::shared_ptr<BaseAST> rightExprAST = this->mASTStack.top();
                this->mASTStack.pop();

                if (this->mASTStack.empty())
                    return nullptr;

                std::shared_ptr<BaseAST> leftExprAST = this->mASTStack.top();
                this->mASTStack.pop();

                std::shared_ptr<ExpressionAST> exprAST = std::make_shared<ExpressionAST>();

                exprAST->SetOperator((this->mTokenStream->CurrentToken()->Type() == TokenType::Then) ? "->" : "<->");
                exprAST->SetLeft(leftExprAST);
                exprAST->SetRight(rightExprAST);

                this->mASTStack.push(exprAST);
                break;
            }
            default:
                return nullptr;
        }

        if (!this->mTokenStream->MoveNext())
            break;
    }

    // スタックには完成した抽象構文木が1つ積まれている
    if (this->mASTStack.empty())
        return nullptr;

    std::shared_ptr<BaseAST> logicalExprAST = this->mASTStack.top();
    this->mASTStack.pop();

    // スタックに積み残しがある場合はエラーを返す
    if (!this->mASTStack.empty())
        return nullptr;

    return logicalExprAST;
}
