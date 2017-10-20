
// LogicalExpressionParser
// Parser.hpp

#ifndef LOGICAL_EXPRESSION_PARSER_PARSER_HPP
#define LOGICAL_EXPRESSION_PARSER_PARSER_HPP

#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <vector>

class TokenStream;
class BaseAST;
class ExpressionAST;
class ConstantAST;
class VariableAST;

class Parser {
public:
    Parser(const std::shared_ptr<TokenStream>& tokenStream) : mTokenStream(tokenStream) { }
    ~Parser() { }

    std::shared_ptr<BaseAST> Parse();

private:
    std::shared_ptr<BaseAST> VisitFactor();
    std::shared_ptr<BaseAST> VisitExpression();
    std::shared_ptr<BaseAST> VisitAndOrExpression();
    std::shared_ptr<BaseAST> VisitNotExpression();
    std::shared_ptr<ConstantAST> VisitConstant();
    std::shared_ptr<VariableAST> VisitVariable();

private:
    std::shared_ptr<TokenStream> mTokenStream;
};

#endif // LOGICAL_EXPRESSION_PARSER_PARSER_HPP
