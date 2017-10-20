
// LogicalExpressionParser
// Token.cpp

#include "Token.hpp"

Token::Token(const std::string& text, TokenType tokenType) :
    mText(text),
    mType(tokenType),
    mValue(
        tokenType == TokenType::True ? std::optional<bool>{ true } : 
        tokenType == TokenType::False ? std::optional<bool>{ false } :
        std::nullopt)
{
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << token.Text() << ' ';
    return os;
}

const std::shared_ptr<Token> TokenStream::CurrentToken() const
{
    return (this->mCurrentIndex >= this->mTokens.size()) ? nullptr : this->mTokens[this->mCurrentIndex];
}

bool TokenStream::MoveNext()
{
    std::size_t tokenSize = this->mTokens.size();

    if (this->mCurrentIndex >= tokenSize - 1) {
        this->mCurrentIndex = tokenSize;
        return false;
    }

    ++this->mCurrentIndex;

    return true;
}

bool TokenStream::MovePrevious()
{
    return this->MoveBack(1);
}

bool TokenStream::MoveBack(std::size_t times)
{
    std::size_t tokenSize = this->mTokens.size();

    // これ以上前にインデックスを戻すことができない
    if (times > tokenSize || times > this->mCurrentIndex) {
        this->mCurrentIndex = 0;
        return false;
    }

    this->mCurrentIndex -= times;

    return false;
}

void TokenStream::AddToken(Token* newToken)
{
    this->mTokens.push_back(std::shared_ptr<Token>(newToken));
}

void TokenStream::PrintTokens() const
{
    std::ostringstream strStream;

    for (const auto& token : this->mTokens)
        strStream << *token;

    std::cout << strStream.str() << '\n';
}

enum class LexerState {
    None,
    Identifier,
    Eq
};

std::shared_ptr<TokenStream> Lexer(std::string& logicalExpr)
{
    TokenStream* tokenStream = new(std::nothrow) TokenStream();

    if (tokenStream == nullptr) {
        std::cout << "Failed to create TokenStream object.\n";
        return nullptr;
    }

    LexerState state = LexerState::None;

    std::string tokenText;
    char currentChar;
    char nextChar;

    logicalExpr += ' ';

    std::size_t inputSize = logicalExpr.size();

    for (size_t i = 0; i < inputSize; ++i) {
        currentChar = logicalExpr[i];
        nextChar = (i + 1) < inputSize ? logicalExpr[i + 1] : '\0';

        switch (state) {
            case LexerState::None:
                if (std::isspace(currentChar)) {
                    // 半角スペースの場合はスキップ
                    continue;
                } else if (std::isalpha(currentChar) || currentChar == '_') {
                    // アルファベットまたはアンダーバーで開始する識別子
                    state = LexerState::Identifier;

                    // 現在の文字についてもう一度処理を通す
                    --i;
                } else if (currentChar == '(') {
                    // 左丸括弧
                    tokenStream->AddToken(new Token("(", TokenType::LeftParenthesis));
                } else if (currentChar == ')') {
                    // 右丸括弧
                    tokenStream->AddToken(new Token(")", TokenType::RightParenthesis));
                } else if (currentChar == '-' && nextChar == '>') {
                    // ならば('->')の記号
                    tokenStream->AddToken(new Token("->", TokenType::Then));

                    // 次の文字についても処理が済んだので, インデックスを1つ先に進められる
                    ++i;
                } else if (currentChar == '<') {
                    // 同値('<->')の記号
                    state = LexerState::Eq;
                } else {
                    // それ以外の文字の場合はエラー
                    std::cout << "Invalid character: \'" << currentChar << "\'\n";
                    return nullptr;
                }
                break;
            case LexerState::Identifier:
                if (std::isalnum(currentChar) || currentChar == '_') {
                    // アルファベット, 数字, アンダーバーであるときは識別子に追加
                    tokenText += currentChar;
                } else {
                    // アルファベット, 数字, アンダーバーであるとき, 識別子の終わり

                    // 識別子が何に相当するかを調べ, トークンを作成して追加
                    if (tokenText == "True" || tokenText == "true" || tokenText == "T" || tokenText == "t") {
                        // 真 (True, true, T, tのいずれか)
                        tokenStream->AddToken(new Token(tokenText, TokenType::True));
                    } else if (tokenText == "False" || tokenText == "false" || tokenText == "F" || tokenText == "f") {
                        // 偽 (False, false, F, fのいずれか)
                        tokenStream->AddToken(new Token(tokenText, TokenType::False));
                    } else if (tokenText == "And" || tokenText == "and") {
                        // かつ (And, andのいずれか)
                        tokenStream->AddToken(new Token(tokenText, TokenType::And));
                    } else if (tokenText == "Or" || tokenText == "or") {
                        // または (Or, orのいずれか)
                        tokenStream->AddToken(new Token(tokenText, TokenType::Or));
                    } else if (tokenText == "Not" || tokenText == "not") {
                        // 非 (Not, notのいずれか)
                        tokenStream->AddToken(new Token(tokenText, TokenType::Not));
                    } else {
                        // それ以外の場合は全て変数(命題変項)とみなす
                        tokenStream->AddToken(new Token(tokenText, TokenType::Variable));
                    }

                    tokenText = "";
                    state = LexerState::None;

                    // 識別子ではない現在の文字についてもう一度処理を通す
                    --i;
                }
                break;
            case LexerState::Eq:
                if (currentChar == '-' && nextChar == '>') {
                    // 同値('<->')の記号の最初の1文字については処理が済んでいるので, 残りの2文字を確認
                    tokenStream->AddToken(new Token("<->", TokenType::Eq));

                    // 次の文字についても処理が済んだので, インデックスを1つ先に進められる
                    ++i;
                } else {
                    // 同値('<->')の記号ではなかったのでエラーを返す
                    if (currentChar != '-')
                        std::cout << "Invalid character: \'" << currentChar << "\'\n";

                    if (nextChar != '>')
                        std::cout << "Invalid character: \'" << nextChar << "\'\n";

                    return nullptr;
                }
            default:
                // 字句解析が有り得ない状態にあるのでエラーを返す
                std::cout << "Illegal lexer state.\n";
                return nullptr;
        }
    }

    // 最後に末尾を表す特別なトークンを追加
    // tokenStream->AddToken(new Token("", TokenType::End));

    return std::shared_ptr<TokenStream>(tokenStream);
}
