
// LogicalExpressionParser
// AST.cpp

#include "AST.hpp"

void ASTPrinter::Print(const std::shared_ptr<BaseAST>& logicalExprAST, Notation notation) const
{
    assert(logicalExprAST != nullptr);
    assert(logicalExprAST->Type() != ASTType::Base);

    switch (notation) {
        case Notation::Infix:
            std::cout << "Infix Expression: ";
            this->PrintInfixNotation(logicalExprAST);
            break;
        case Notation::Prefix:
            std::cout << "Prefix Expression: ";
            this->PrintPrefixNotation(logicalExprAST);
            break;
        case Notation::Postfix:
            std::cout << "Postfix Expression: ";
            this->PrintPostfixNotation(logicalExprAST);
            break;
    }
    
    std::cout << '\n';
}

void ASTPrinter::PrintInfixNotation(const std::shared_ptr<BaseAST>& logicalExprAST) const
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
                this->PrintInfixNotation(factorAST->Expr());
            
            return;
        }
        case ASTType::NotExpression:
        {
            // Not
            std::shared_ptr<NotExpressionAST> notExprAST = std::dynamic_pointer_cast<NotExpressionAST>(logicalExprAST);

            if (notExprAST != nullptr) {
                std::cout << "( ";
                std::cout << "￢ ";
                this->PrintInfixNotation(notExprAST->Expr());
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
                this->PrintInfixNotation(andOrExprAST->Left());
                std::cout << andOrExprAST->Operator() << ' ';
                this->PrintInfixNotation(andOrExprAST->Right());
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
                this->PrintInfixNotation(exprAST->Left());
                std::cout << exprAST->Operator() << ' ';
                this->PrintInfixNotation(exprAST->Right());
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

void ASTPrinter::PrintPostfixNotation(const std::shared_ptr<BaseAST>& logicalExprAST) const
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
                this->PrintPostfixNotation(factorAST->Expr());

            return;
        }
        case ASTType::NotExpression:
        {
            // Not
            std::shared_ptr<NotExpressionAST> notExprAST = std::dynamic_pointer_cast<NotExpressionAST>(logicalExprAST);

            if (notExprAST != nullptr) {
                this->PrintPostfixNotation(notExprAST->Expr());
                std::cout << "￢ ";
            }

            return;
        }
        case ASTType::AndOrExpression:
        {
            // AndまたはOr
            std::shared_ptr<AndOrExpressionAST> andOrExprAST = std::dynamic_pointer_cast<AndOrExpressionAST>(logicalExprAST);

            if (andOrExprAST != nullptr) {
                this->PrintPostfixNotation(andOrExprAST->Left());
                std::cout << ' ';
                this->PrintPostfixNotation(andOrExprAST->Right());
                std::cout << andOrExprAST->Operator() << ' ';
            }

            return;
        }
        case ASTType::Expression:
        {
            // ThenまたはEq
            std::shared_ptr<ExpressionAST> exprAST = std::dynamic_pointer_cast<ExpressionAST>(logicalExprAST);

            if (exprAST != nullptr) {
                this->PrintPostfixNotation(exprAST->Left());
                std::cout << ' ';
                this->PrintPostfixNotation(exprAST->Right());
                std::cout << exprAST->Operator() << ' ';
            }

            return;
        }
    }

    return;
}
