#include "Expr.hpp"
#include <string>
#include <any>
#include "scanner.h"
#include <iostream>
#include <typeinfo>

// #include <vector>
using std::string;

class ASTPrinter : public Visitor<string>
{
public:
    string print(Expr<string> *expr)
    {
        return expr->accept(this);
    }
    string parenthesize(string name, std::initializer_list<Expr<string> *> exprs)
    {
        string ans("(");
        ans += name;
        for (auto &expr : exprs)
        {
            ans += ' ';
            ans += expr->accept(this);
        }
        ans += ')';
        return ans;
    }
    string print(Expr<string> &expr)
    {
        return expr.accept(this);
    }
    string visitBinaryExpr(Binary<string> *expr)
    {
        return parenthesize(expr->oper.lexeme, {expr->left, expr->right});
        // return parenthesize(expr->oper.lexeme, {expr->left, expr->right});
    };
    string visitGroupingExpr(Grouping<string> *expr)
    {
        return parenthesize("group", {expr->expression});
    };
    string visitLiteralExpr(Literal<string> *expr)
    {
        try
        {

            double val = std::any_cast<double>(expr->value);

            return std::to_string(val);
        }
        catch (std::bad_any_cast &e)
        {
        }
        try
        {
            string val = std::any_cast<string>(expr->value);
            return val;
        }
        catch (std::bad_any_cast &e)
        {
            std::cout<<expr->value.type().name();

            return "nil";
        }
    };
    string visitUnaryExpr(Unary<string> *expr)
    {
        return parenthesize(expr->oper.lexeme, {expr->right});
    };
};

//int main()
//{
//    auto *expression = new Binary<string>(new Unary<string>(
//                                              Token(MINUS, "-", get_empty_literal(), 1), new Literal<string>((double)123)),
//                                          Token(STAR, "*", get_empty_literal(), 1), new Grouping<string>(new Literal<string>(45.67)));
//    std::cout << ASTPrinter().print(expression);
//}