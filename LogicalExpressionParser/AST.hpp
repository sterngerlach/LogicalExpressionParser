
// LogicalExpressionParser
// AST.hpp

#ifndef LOGICAL_EXPRESSION_PARSER_AST_HPP
#define LOGICAL_EXPRESSION_PARSER_AST_HPP

#pragma once

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

/*
äáå ÇÃè»ó™ãKë•Çä‹ÇﬂÇΩò_óùéÆÇÃï∂ñ@ (EBNF)
<Constant> ::= 'T' | 'F'
<Variable> ::= 'P' | 'Q' | 'R' | 'P1' | 'P2' | ...
<Factor> ::=
    <Constant> |
    <Variable> |
    '(' <Expression> ')'
<NotExpression> ::=
    <Factor> |
    'Not' <Factor>
<AndOrExpression> ::=
    <NotExpression> |
    <NotExpression> 'And' <NotExpression> |
    <NotExpression> 'Or' <NotExpression>
<Expression> ::=
    <AndOrExpression> |
    <AndOrExpression> '->' <AndOrExpression> |
    <AndOrExpression> '<->' <AndOrExpression>
 */

enum class ASTType {
    Base,
    Factor,
    NotExpression,
    AndOrExpression,
    Expression,
    Constant,
    Variable
};

class BaseAST {
public:
    BaseAST(ASTType type) : mType(type) { }
    virtual ~BaseAST() { }

    inline ASTType Type() const { return this->mType; }

private:
    ASTType mType;
};

class FactorAST : public BaseAST {
public:
    FactorAST(const std::shared_ptr<BaseAST>& expr) :
        BaseAST(ASTType::Factor), mExpr(expr) { }
    FactorAST() :
        BaseAST(ASTType::Factor) { }
    ~FactorAST() { }

    inline const std::shared_ptr<BaseAST>& Expr() const { return this->mExpr; }
    void SetExpr(const std::shared_ptr<BaseAST>& exprAST) { this->mExpr = exprAST; }

private:
    std::shared_ptr<BaseAST> mExpr;
};

class NotExpressionAST : public BaseAST {
public:
    NotExpressionAST(const std::shared_ptr<BaseAST>& expr) :
        BaseAST(ASTType::NotExpression), mExpr(expr) { }
    NotExpressionAST() :
        BaseAST(ASTType::NotExpression) { }
    ~NotExpressionAST() { }

    inline const std::shared_ptr<BaseAST>& Expr() const { return this->mExpr; }
    void SetExpr(const std::shared_ptr<BaseAST>& exprAST) { this->mExpr = exprAST; }

private:
    std::shared_ptr<BaseAST> mExpr;
};

class AndOrExpressionAST : public BaseAST {
public:
    AndOrExpressionAST(const std::shared_ptr<BaseAST>& left, const std::shared_ptr<BaseAST>& right, const std::string& op) :
        BaseAST(ASTType::AndOrExpression), mLeft(left), mRight(right), mOperator(op) { }
    AndOrExpressionAST() :
        BaseAST(ASTType::AndOrExpression) { }
    ~AndOrExpressionAST() { }

    inline const std::shared_ptr<BaseAST>& Left() const { return this->mLeft; }
    inline const std::shared_ptr<BaseAST>& Right() const { return this->mRight; }
    inline const std::string& Operator() const { return this->mOperator; }

    void SetLeft(const std::shared_ptr<BaseAST>& leftExprAST) { this->mLeft = leftExprAST; }
    void SetRight(const std::shared_ptr<BaseAST>& rightExprAST) { this->mRight = rightExprAST; }
    void SetOperator(const std::string& op) { this->mOperator = op; }

private:
    std::shared_ptr<BaseAST> mLeft;
    std::shared_ptr<BaseAST> mRight;
    std::string mOperator;
};

class ExpressionAST : public BaseAST {
public:
    ExpressionAST(const std::shared_ptr<BaseAST>& left, const std::shared_ptr<BaseAST>& right, const std::string& op) :
        BaseAST(ASTType::Expression), mLeft(left), mRight(right), mOperator(op) { }
    ExpressionAST() :
        BaseAST(ASTType::Expression) { }
    ~ExpressionAST() { }

    inline const std::shared_ptr<BaseAST>& Left() const { return this->mLeft; }
    inline const std::shared_ptr<BaseAST>& Right() const { return this->mRight; }
    inline const std::string& Operator() const { return this->mOperator; }

    void SetLeft(const std::shared_ptr<BaseAST>& leftExprAST) { this->mLeft = leftExprAST; }
    void SetRight(const std::shared_ptr<BaseAST>& rightExprAST) { this->mRight = rightExprAST; }
    void SetOperator(const std::string& op) { this->mOperator = op; }

private:
    std::shared_ptr<BaseAST> mLeft;
    std::shared_ptr<BaseAST> mRight;
    std::string mOperator;
};

class ConstantAST : public BaseAST {
public:
    ConstantAST(bool value) : BaseAST(ASTType::Constant), mValue(value) { }
    ConstantAST() : BaseAST(ASTType::Constant) { }
    ~ConstantAST() { }

    inline bool Value() const { return this->mValue; }
    void SetValue(bool value) { this->mValue = value; }

private:
    bool mValue;
};

class VariableAST : public BaseAST {
public:
    VariableAST(const std::string& name) : BaseAST(ASTType::Variable), mName(name) { }
    VariableAST() : BaseAST(ASTType::Variable) { }
    ~VariableAST() { }

    inline const std::string& Name() const { return this->mName; }
    void SetName(const std::string& name) { this->mName = name; }

private:
    std::string mName;
};

enum class Notation {
    Infix,
    Prefix,
    Postfix
};

class ASTPrinter {
public:
    ASTPrinter() = default;
    ~ASTPrinter() = default;

    void Print(const std::shared_ptr<BaseAST>& logicalExprAST, Notation notation) const;

private:
    void PrintInfixNotation(const std::shared_ptr<BaseAST>& logicalExprAST) const;
    void PrintPrefixNotation(const std::shared_ptr<BaseAST>& logicalExprAST) const;
    void PrintPostfixNotation(const std::shared_ptr<BaseAST>& logicalExprAST) const;
};

#endif // LOGICAL_EXPRESSION_PARSER_AST_HPP
