
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

        // 字句解析
        std::shared_ptr<TokenStream> tokenStream = Lexer(logicalExpr);
        
        if (tokenStream == nullptr) {
            std::cout << "Lexical analysis failed.\n";
            continue;
        }

        // Infix Expressionと仮定して構文解析
        std::cout << "Parsing infix expression...\n";
        std::shared_ptr<InfixParser> infixExprParser = std::make_shared<InfixParser>(tokenStream);
        std::shared_ptr<BaseAST> exprAST = infixExprParser->Parse();

        // Infix Expressionと仮定して構文解析が上手くいかない場合
        if (exprAST == nullptr) {
            // トークンを巻き戻す
            tokenStream->SetCurrentIndex(0U);
            
            // Prefix Expressionと仮定して構文解析
            std::cout << "Parsing prefix expression...\n";
            std::shared_ptr<PrefixParser> prefixExprParser = std::make_shared<PrefixParser>(tokenStream);
            exprAST = prefixExprParser->Parse();

            // Prefix Expressionと仮定して構文解析が上手くいかない場合
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
