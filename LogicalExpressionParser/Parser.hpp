
// LogicalExpressionParser
// Parser.hpp

#ifndef LOGICAL_EXPRESSION_PARSER_PARSER_HPP
#define LOGICAL_EXPRESSION_PARSER_PARSER_HPP

#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <vector>

class Token;
class TokenStream;
class BaseAST;
class ExpressionAST;
class ConstantAST;
class VariableAST;

class Parser {
public:
    Parser(const std::shared_ptr<TokenStream>& tokenStream) : mTokenStream(tokenStream) { }
    virtual ~Parser() { }

    virtual std::shared_ptr<BaseAST> Parse() { return nullptr; }
    virtual std::shared_ptr<ConstantAST> VisitConstant();
    virtual std::shared_ptr<VariableAST> VisitVariable();

protected:
    std::shared_ptr<TokenStream> mTokenStream;
};

class InfixParser final : public Parser {
public:
    InfixParser(const std::shared_ptr<TokenStream>& tokenStream) : Parser(tokenStream) { }
    ~InfixParser() { }

    std::shared_ptr<BaseAST> Parse() override;

private:
    std::shared_ptr<BaseAST> VisitFactor();
    std::shared_ptr<BaseAST> VisitExpression();
    std::shared_ptr<BaseAST> VisitAndOrExpression();
    std::shared_ptr<BaseAST> VisitNotExpression();
};

class PrefixParser final : public Parser {
public:
    PrefixParser(const std::shared_ptr<TokenStream>& tokenStream) : Parser(tokenStream) { }
    ~PrefixParser() { }

    std::shared_ptr<BaseAST> Parse() override;

private:
    std::shared_ptr<BaseAST> VisitThenOrEqExpression();
    std::shared_ptr<BaseAST> VisitAndOrExpression();
    std::shared_ptr<BaseAST> VisitNotExpression();
};

#endif // LOGICAL_EXPRESSION_PARSER_PARSER_HPP
