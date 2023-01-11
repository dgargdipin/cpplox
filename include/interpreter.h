#pragma once

#include <any>
#include <string>
#include "Expr.hpp"
#include "RuntimeException.h"
#include <stdexcept>
#include <algorithm>
#include <typeindex>
#include <cassert>
#include "utils.h"

namespace Lox {

    typedef std::any Object;

    class Interpreter : public Visitor {
        std::unique_ptr<Object> value;
//        Object value;
    public:
        #define RETURN(ret) Return(ret);return
        Interpreter() {
            std::cout << "Called constructor to interpreter" << std::endl;
        }

        Object evaluate(Expr *n) {
//            static Interpreter vis;
            n->accept(*this); // this call fills the return value
            Object * obj = value.release();
            return *obj;
        }

        void Return(Object value_) {
            value = std::make_unique<Object>(value_);
        }


        std::string get_string_repr(Object &obj) {
            if (instanceof<double>(obj)) {
                return to_string(std::any_cast<double>(obj));
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

        void interpret(std::unique_ptr<Expr> expr) {
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
            RETURN(expr->value);
        };

        void visit(Grouping *expr) {
            RETURN(evaluate(expr->expression.get()));

        };

        void visit(Ternary *expr) {

            Object condition_object = evaluate(expr->condition.get());
            bool condition = isTruthy(condition_object);
            if (condition) {
                RETURN(evaluate(expr->left.get()));

            }
            RETURN(evaluate(expr->right.get()));

        }

        void visit(Nothing *expr) {
            throw std::runtime_error("Runtime error");
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
                    RETURN(-std::any_cast<double>(right));

                case BANG:
                    RETURN(!isTruthy(right));

            }
        }

        void visit(Binary *expr) {
            Object left = evaluate(expr->left.get());
            Object right = evaluate(expr->right.get());
            switch (expr->oper.type) {
                case COMMA: {
                    RETURN(right);

                }
                case MINUS: {
                    check_number_operands(expr->oper, left, right);
                    RETURN(std::any_cast<double>(left) - std::any_cast<double>(right));

                }
                case SLASH: {
                    check_number_operands(expr->oper, left, right);
                    RETURN(std::any_cast<double>(left) / std::any_cast<double>(right));

                }
                case STAR: {
                    check_number_operands(expr->oper, left, right);
                    RETURN(std::any_cast<double>(left) * std::any_cast<double>(right));

                }
                case GREATER: {
                    check_number_operands(expr->oper, left, right);
                    RETURN(std::any_cast<double>(left) > std::any_cast<double>(right));

                }
                case GREATER_EQUAL: {
                    check_number_operands(expr->oper, left, right);
                    RETURN(std::any_cast<double>(left) >= std::any_cast<double>(right));

                }
                case LESS: {
                    check_number_operands(expr->oper, left, right);
                    RETURN(std::any_cast<double>(left) < std::any_cast<double>(right));

                }
                case LESS_EQUAL:
                    check_number_operands(expr->oper, left, right);
                    RETURN(std::any_cast<double>(left) <= std::any_cast<double>(right));

                case BANG_EQUAL:
                    RETURN(!isEqual(left, right));

                case EQUAL_EQUAL:
                    RETURN(isEqual(left, right));

                case PLUS:
                {
                    if(instanceof<std::string>(left)){
                        auto left_str=std::any_cast<std::string>(left);
                        if(instanceof<std::string>(right)){
                            auto right_str=std::any_cast<std::string>(right);
                            RETURN(left_str+right_str);
                        }
                        auto right_double=std::any_cast<double>(right);
                        RETURN(left_str+to_string(right_double));
                    }
                    auto left_double=std::any_cast<double>(left);
                    if(instanceof<std::string>(right)){
                        auto right_str=std::any_cast<std::string>(right);
                        RETURN(to_string(left_double)+right_str);
                    }
                    auto right_double=std::any_cast<double>(right);
                     RETURN(left_double+right_double);
                }
            }

            RETURN({});

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