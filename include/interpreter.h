#pragma once

#include <any>
#include <string>
#include "Expr.hpp"
#include "RuntimeException.h"
#include <stdexcept>
#include <algorithm>
#include <typeindex>
#include <cassert>

namespace Lox {

    typedef std::any Object;

class Interpreter : public ValueGetter<Interpreter,Expr*,Object>, public Visitor {

    public:
//    virtual void Visit(Number& n)
//    {
//        // Store the return value in the ValueGetter member
//        Return(n.value);
//    }
//
//    virtual void Visit(Plus& n)
//    {
//        // GetValue visits children and returns computed values
//        Return(GetValue(n.left) + GetValue(n.right));
//    }

    std::string get_string_repr(Object &obj) {
            if (instanceof<double>(obj)) {
                return std::to_string(std::any_cast<double>(obj));
                //TODO remove ending .0000s in double string repr
            }
            if (instanceof<bool>(obj)) {
                bool bool_obj = std::any_cast<bool>(obj);
                if (bool_obj)
                    return "True";
                return "False";
            }
            if (instanceof<std::string>(obj)) {
                return std::any_cast<std::string>(obj);
            }
            //TODO error handling when getting string repr of object
            assert(false);
        }

        void interpret(std::unique_ptr<Expr > expr) {
            try {
                Object value = evaluate(expr.get());
                std::cout << get_string_repr(value) << std::endl;
            }
            catch (RuntimeException &e) {
                Lox::runtime_error(e);
            }
        }

//        Object evaluate(Expr *expr) {
//            return expr->accept(this);
//        }

        void visit(Literal *expr) {
            Return(expr->value);
        };

        void visit(Grouping *expr) {
            Return(evaluate(expr->expression.get()));
        };

        void visit(Ternary *expr) {
            
            Object condition_object=evaluate(expr->condition.get());
            bool condition = isTruthy(condition_object);
            if(condition){
                Return(evaluate(expr->left.get()));
            }
            Return(evaluate(expr->right.get()));
        }

        void visit(Nothing *expr){
        }

        void check_number_operand(Token operator_token, Object &operand) {
            if (instanceof<double>(operand))return;
            throw RuntimeException(operator_token, "Operand must be a number");
        }

        void check_number_operands(Token operator_token, Object &left, Object &right) {
            if (instanceof<double>(left) && instanceof<double>(right))return;
            throw RuntimeException(operator_token, "Operands must be numbers");
        }

        void visit(Unary *expr) {
            Object right = evaluate(expr->right.get());
            switch (expr->oper.type) {
                case MINUS:
                    check_number_operand(expr->oper, right);
                    Return(-std::any_cast<double>(right));
                case BANG:
                    Return(!isTruthy(right));
            }
        }

        void visit(Binary *expr) {
            Object left = evaluate(expr->left.get());
            Object right = evaluate(expr->right.get());
            switch (expr->oper.type) {
                case COMMA: {
                    Return(right);
                }
                case MINUS: {
                    check_number_operands(expr->oper, left, right);
                    Return(std::any_cast<double>(left) - std::any_cast<double>(right));
                }
                case SLASH: {
                    check_number_operands(expr->oper, left, right);
                    Return(std::any_cast<double>(left) / std::any_cast<double>(right));
                }
                case STAR: {
                    check_number_operands(expr->oper, left, right);
                    Return(std::any_cast<double>(left) * std::any_cast<double>(right));
                }
                case GREATER: {
                    check_number_operands(expr->oper, left, right);
                    Return(std::any_cast<double>(left) > std::any_cast<double>(right));
                }
                case GREATER_EQUAL: {
                    check_number_operands(expr->oper, left, right);
                    Return(std::any_cast<double>(left) >= std::any_cast<double>(right));
                }
                case LESS: {
                    check_number_operands(expr->oper, left, right);
                    Return(std::any_cast<double>(left) < std::any_cast<double>(right));
                }
                case LESS_EQUAL:
                    check_number_operands(expr->oper, left, right);
                    Return(std::any_cast<double>(left) <= std::any_cast<double>(right));
                case BANG_EQUAL:
                    Return(!isEqual(left, right));
                case EQUAL_EQUAL:
                    Return(isEqual(left, right));
                case PLUS:
                    if (instanceof<std::string>(left) && instanceof<std::string>(right)) {
                        Return(std::any_cast<std::string>(left) + std::any_cast<std::string>(right));
                    } else if (instanceof<double>(left) && instanceof<double>(right)) {
                        Return(std::any_cast<double>(left) + std::any_cast<double>(right));
                    }
                    throw RuntimeException(expr->oper, "Operands must be two numbers or two strings.");
            }

            Return({});
        }

        bool isTruthy(Object &val) {
            if (!val.has_value())
                return false;
            try {
                auto typed_val = std::any_cast<bool>(val);
                return typed_val;
            }
            catch (const std::bad_any_cast &e) {
            };
            return true;
        };

        bool isEqual(Object &val1, Object &val2) {
            if (isNull(val1) && isNull(val2))
                return true;
            if (isNull(val1))
                return false;
            if (!same_type(val1, val2))
                return false;
            if (instanceof<double>(val1)) {
                return std::any_cast<double>(val1) == std::any_cast<double>(val2);
            }
            if (instanceof<bool>(val1)) {
                return std::any_cast<bool>(val1) == std::any_cast<bool>(val2);
            }
            if (instanceof<std::string>(val1)) {
                return std::any_cast<std::string>(val1) == std::any_cast<std::string>(val2);
            }
            throw std::runtime_error("Unexpected types");
        }

        bool isNull(Object &obj) {
            return !obj.has_value();
        }

        template<typename T>
        bool instanceof(Object &obj) {
            return obj.type() == typeid(T);
        }

        bool same_type(Object &obj1, Object &obj2) {
            return obj1.type() == obj2.type();
        }


    };
}