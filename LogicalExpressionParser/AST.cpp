
// LogicalExpressionParser
// AST.cpp

#include "AST.hpp"

void ASTPrinter::Print(const std::shared_ptr<BaseAST>& logicalExprAST, bool prefixNotation) const
{
    assert(logicalExprAST != nullptr);
    assert(logicalExprAST->Type() != ASTType::Base);

    if (!prefixNotation)
        this->PrintTree(logicalExprAST);
    else
        this->PrintPrefixNotation(logicalExprAST);

    std::cout << '\n';
}

void ASTPrinter::PrintTree(const std::shared_ptr<BaseAST>& logicalExprAST) const
{
    switch (logicalExprAST->Type()) {
        case ASTType::Constant:
        {
            // 定数
            std::shared_ptr<ConstantAST> constAST = std::dynamic_pointer_cast<ConstantAST>(logicalExprAST);

            if (constAST != nullptr)
                std::cout << std::boolalpha << constAST->Value() << ' ';

            return;
        }
        case ASTType::Variable:
        {
            // 変数
            std::shared_ptr<VariableAST> variableAST = std::dynamic_pointer_cast<VariableAST>(logicalExprAST);

            if (variableAST != nullptr)
                std::cout << variableAST->Name() << ' ';

            return;
        }
        case ASTType::Factor:
        {
            // 括弧つきの論理式
            std::shared_ptr<FactorAST> factorAST = std::dynamic_pointer_cast<FactorAST>(logicalExprAST);

            if (factorAST != nullptr) {
                this->PrintTree(factorAST->Expr());
            }
            
            return;
        }
        case ASTType::NotExpression:
        {
            // Not
            std::shared_ptr<NotExpressionAST> notExprAST = std::dynamic_pointer_cast<NotExpressionAST>(logicalExprAST);

            if (notExprAST != nullptr) {
                std::cout << "( ";
                std::cout << "￢ ";
                this->PrintTree(notExprAST->Expr());
                std::cout << ") ";
            }

            return;
        }
        case ASTType::AndOrExpression:
        {
            // AndまたはOr
            std::shared_ptr<AndOrExpressionAST> andOrExprAST = std::dynamic_pointer_cast<AndOrExpressionAST>(logicalExprAST);

            if (andOrExprAST != nullptr) {
                std::cout << "( ";
                this->PrintTree(andOrExprAST->Left());
                std::cout << andOrExprAST->Operator() << ' ';
                this->PrintTree(andOrExprAST->Right());
                std::cout << ") ";
            }

            return;
        }
        case ASTType::Expression:
        {
            // ThenまたはEq
            std::shared_ptr<ExpressionAST> exprAST = std::dynamic_pointer_cast<ExpressionAST>(logicalExprAST);

            if (exprAST != nullptr) {
                std::cout << "( ";
                this->PrintTree(exprAST->Left());
                std::cout << exprAST->Operator() << ' ';
                this->PrintTree(exprAST->Right());
                std::cout << ") ";
            }

            return;
        }
    }

    return;
}

void ASTPrinter::PrintPrefixNotation(const std::shared_ptr<BaseAST>& logicalExprAST) const
{
    switch (logicalExprAST->Type()) {
        case ASTType::Constant:
        {
            // 定数
            std::shared_ptr<ConstantAST> constAST = std::dynamic_pointer_cast<ConstantAST>(logicalExprAST);

            if (constAST != nullptr)
                std::cout << std::boolalpha << constAST->Value() << ' ';

            return;
        }
        case ASTType::Variable:
        {
            // 変数
            std::shared_ptr<VariableAST> variableAST = std::dynamic_pointer_cast<VariableAST>(logicalExprAST);

            if (variableAST != nullptr)
                std::cout << variableAST->Name() << ' ';

            return;
        }
        case ASTType::Factor:
        {
            // 括弧つきの論理式
            std::shared_ptr<FactorAST> factorAST = std::dynamic_pointer_cast<FactorAST>(logicalExprAST);

            if (factorAST != nullptr)
                this->PrintPrefixNotation(factorAST->Expr());

            return;
        }
        case ASTType::NotExpression:
        {
            // Not
            std::shared_ptr<NotExpressionAST> notExprAST = std::dynamic_pointer_cast<NotExpressionAST>(logicalExprAST);

            if (notExprAST != nullptr) {
                std::cout << "￢ ";
                this->PrintPrefixNotation(notExprAST->Expr());
            }

            return;
        }
        case ASTType::AndOrExpression:
        {
            // AndまたはOr
            std::shared_ptr<AndOrExpressionAST> andOrExprAST = std::dynamic_pointer_cast<AndOrExpressionAST>(logicalExprAST);

            if (andOrExprAST != nullptr) {
                std::cout << andOrExprAST->Operator() << ' ';
                this->PrintPrefixNotation(andOrExprAST->Left());
                std::cout << ' ';
                this->PrintPrefixNotation(andOrExprAST->Right());
            }

            return;
        }
        case ASTType::Expression:
        {
            // ThenまたはEq
            std::shared_ptr<ExpressionAST> exprAST = std::dynamic_pointer_cast<ExpressionAST>(logicalExprAST);

            if (exprAST != nullptr) {
                std::cout << exprAST->Operator() << ' ';
                this->PrintPrefixNotation(exprAST->Left());
                std::cout << ' ';
                this->PrintPrefixNotation(exprAST->Right());
            }

            return;
        }
    }

    return;
}
