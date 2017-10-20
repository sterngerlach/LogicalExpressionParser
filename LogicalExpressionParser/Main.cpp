
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
        std::cout << "Enter logical expression> ";

        if (!std::getline(std::cin, logicalExpr))
            break;

        if (logicalExpr == "")
            break;

        // š‹å‰ğÍ
        std::shared_ptr<TokenStream> tokenStream = Lexer(logicalExpr);
        // tokenStream->PrintTokens();

        // \•¶‰ğÍ
        std::shared_ptr<Parser> logicalExprParser = std::make_shared<Parser>(tokenStream);
        std::shared_ptr<BaseAST> exprAST = logicalExprParser->Parse();

        if (exprAST != nullptr) {
            astPrinter->Print(exprAST);
            astPrinter->Print(exprAST, true);
        } else {
            std::cout << "Parse failed.\n";
        }
    }

    std::getchar();

    return EXIT_SUCCESS;
}
