
// LogicalExpressionParser
// Main.cpp

#include <cstdlib>

#include "AST.hpp"
#include "Token.hpp"
#include "Parser.hpp"

int main(int argc, char** argv)
{
    std::shared_ptr<ASTPrinter> astPrinter = std::make_shared<ASTPrinter>();
    std::string logicalExpr;
    
    while (true) {
        std::cout << "Enter logical expression (infix or prefix)> ";

        if (!std::getline(std::cin, logicalExpr))
            break;

        if (logicalExpr == "")
            break;

        // ������
        std::shared_ptr<TokenStream> tokenStream = Lexer(logicalExpr);
        
        if (tokenStream == nullptr) {
            std::cout << "Lexical analysis failed.\n";
            continue;
        }

        // Infix Expression�Ɖ��肵�č\�����
        std::cout << "Parsing infix expression...\n";
        std::shared_ptr<InfixParser> infixExprParser = std::make_shared<InfixParser>(tokenStream);
        std::shared_ptr<BaseAST> exprAST = infixExprParser->Parse();

        // Infix Expression�Ɖ��肵�č\����͂���肭�����Ȃ��ꍇ
        if (exprAST == nullptr) {
            // �g�[�N���������߂�
            tokenStream->SetCurrentIndex(0U);
            
            // Prefix Expression�Ɖ��肵�č\�����
            std::cout << "Parsing prefix expression...\n";
            std::shared_ptr<PrefixParser> prefixExprParser = std::make_shared<PrefixParser>(tokenStream);
            exprAST = prefixExprParser->Parse();

            // Prefix Expression�Ɖ��肵�č\����͂���肭�����Ȃ��ꍇ
            if (exprAST == nullptr) {
                std::cout << "Parse failed.\n";
                continue;
            }
        }

        astPrinter->Print(exprAST);
        astPrinter->Print(exprAST, true);
    }

    std::getchar();

    return EXIT_SUCCESS;
}
