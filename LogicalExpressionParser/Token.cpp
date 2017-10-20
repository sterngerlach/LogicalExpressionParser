
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

    // ����ȏ�O�ɃC���f�b�N�X��߂����Ƃ��ł��Ȃ�
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
                    // ���p�X�y�[�X�̏ꍇ�̓X�L�b�v
                    continue;
                } else if (std::isalpha(currentChar) || currentChar == '_') {
                    // �A���t�@�x�b�g�܂��̓A���_�[�o�[�ŊJ�n���鎯�ʎq
                    state = LexerState::Identifier;

                    // ���݂̕����ɂ��Ă�����x������ʂ�
                    --i;
                } else if (currentChar == '(') {
                    // ���ۊ���
                    tokenStream->AddToken(new Token("(", TokenType::LeftParenthesis));
                } else if (currentChar == ')') {
                    // �E�ۊ���
                    tokenStream->AddToken(new Token(")", TokenType::RightParenthesis));
                } else if (currentChar == '-' && nextChar == '>') {
                    // �Ȃ��('->')�̋L��
                    tokenStream->AddToken(new Token("->", TokenType::Then));

                    // ���̕����ɂ��Ă��������ς񂾂̂�, �C���f�b�N�X��1��ɐi�߂���
                    ++i;
                } else if (currentChar == '<') {
                    // ���l('<->')�̋L��
                    state = LexerState::Eq;
                } else {
                    // ����ȊO�̕����̏ꍇ�̓G���[
                    std::cout << "Invalid character: \'" << currentChar << "\'\n";
                    return nullptr;
                }
                break;
            case LexerState::Identifier:
                if (std::isalnum(currentChar) || currentChar == '_') {
                    // �A���t�@�x�b�g, ����, �A���_�[�o�[�ł���Ƃ��͎��ʎq�ɒǉ�
                    tokenText += currentChar;
                } else {
                    // �A���t�@�x�b�g, ����, �A���_�[�o�[�ł���Ƃ�, ���ʎq�̏I���

                    // ���ʎq�����ɑ������邩�𒲂�, �g�[�N�����쐬���Ēǉ�
                    if (tokenText == "True" || tokenText == "true" || tokenText == "T" || tokenText == "t") {
                        // �^ (True, true, T, t�̂����ꂩ)
                        tokenStream->AddToken(new Token(tokenText, TokenType::True));
                    } else if (tokenText == "False" || tokenText == "false" || tokenText == "F" || tokenText == "f") {
                        // �U (False, false, F, f�̂����ꂩ)
                        tokenStream->AddToken(new Token(tokenText, TokenType::False));
                    } else if (tokenText == "And" || tokenText == "and") {
                        // ���� (And, and�̂����ꂩ)
                        tokenStream->AddToken(new Token(tokenText, TokenType::And));
                    } else if (tokenText == "Or" || tokenText == "or") {
                        // �܂��� (Or, or�̂����ꂩ)
                        tokenStream->AddToken(new Token(tokenText, TokenType::Or));
                    } else if (tokenText == "Not" || tokenText == "not") {
                        // �� (Not, not�̂����ꂩ)
                        tokenStream->AddToken(new Token(tokenText, TokenType::Not));
                    } else {
                        // ����ȊO�̏ꍇ�͑S�ĕϐ�(����ύ�)�Ƃ݂Ȃ�
                        tokenStream->AddToken(new Token(tokenText, TokenType::Variable));
                    }

                    tokenText = "";
                    state = LexerState::None;

                    // ���ʎq�ł͂Ȃ����݂̕����ɂ��Ă�����x������ʂ�
                    --i;
                }
                break;
            case LexerState::Eq:
                if (currentChar == '-' && nextChar == '>') {
                    // ���l('<->')�̋L���̍ŏ���1�����ɂ��Ă͏������ς�ł���̂�, �c���2�������m�F
                    tokenStream->AddToken(new Token("<->", TokenType::Eq));

                    // ���̕����ɂ��Ă��������ς񂾂̂�, �C���f�b�N�X��1��ɐi�߂���
                    ++i;
                } else {
                    // ���l('<->')�̋L���ł͂Ȃ������̂ŃG���[��Ԃ�
                    if (currentChar != '-')
                        std::cout << "Invalid character: \'" << currentChar << "\'\n";

                    if (nextChar != '>')
                        std::cout << "Invalid character: \'" << nextChar << "\'\n";

                    return nullptr;
                }
            default:
                // �����͂��L�蓾�Ȃ���Ԃɂ���̂ŃG���[��Ԃ�
                std::cout << "Illegal lexer state.\n";
                return nullptr;
        }
    }

    // �Ō�ɖ�����\�����ʂȃg�[�N����ǉ�
    // tokenStream->AddToken(new Token("", TokenType::End));

    return std::shared_ptr<TokenStream>(tokenStream);
}
