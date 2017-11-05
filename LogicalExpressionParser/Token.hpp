
// LogicalExpressionParser
// Token.hpp

#ifndef LOGICAL_EXPRESSION_PARSER_TOKEN_HPP
#define LOGICAL_EXPRESSION_PARSER_TOKEN_HPP

#pragma once

#include <cassert>
#include <cstdint>
#include <cctype>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

enum class TokenType {
    True,
    False,
    Variable,
    And,
    Or,
    Not,
    Then,
    Eq,
    LeftParenthesis,
    RightParenthesis,
    End
};

class Token {
public:
    Token(const std::string& text, TokenType tokenType);
    Token() = delete;
    ~Token() = default;

    inline const std::string& Text() const { return this->mText; }
    inline TokenType Type() const { return this->mType; }
    inline bool HasValue() const { return this->mValue.has_value(); }
    inline bool Value() const { return this->mValue.value(); }

    friend std::ostream& operator<<(std::ostream& os, const Token& token);

private:
    std::string	            mText;
    TokenType               mType;
    std::optional<bool>     mValue;
};

class TokenStream {
public:
    TokenStream() : mCurrentIndex(0U) { }
    ~TokenStream() = default;

    inline bool HasMoreTokens() const { return this->mCurrentIndex < this->mTokens.size() - 1; }
    inline void SetCurrentIndex(std::size_t index) { this->mCurrentIndex = index; }

    const std::shared_ptr<Token> CurrentToken() const;
    bool MoveNext();
    bool MovePrevious();
    bool MoveBack(std::size_t times);
    void AddToken(Token* newToken);
    void PrintTokens() const;

private:
    std::vector<std::shared_ptr<Token>> mTokens;
    std::size_t mCurrentIndex;
};

std::shared_ptr<TokenStream> Lexer(std::string& logicalExpr);

#endif // LOGICAL_EXPRESSION_PARSER_TOKEN_HPP
