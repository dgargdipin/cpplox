#pragma once


#include <string>
#include "Expr.hpp"
#include "RuntimeException.h"
#include <stdexcept>
#include <algorithm>
#include <typeindex>
#include <cassert>
#include "utils.h"
#include <cmath>
#include "types.h"
#include "Environment.h"

namespace Lox {

    class Interpreter : public ExprVisitor, StmtVisitor {
        std::unique_ptr<Object> value; //value for exprvisitor
//        Object value;
        void execute(Stmt *stmt) {
            stmt->accept(*this);
        }

        Environment *environment;
    public:
#define RETURN(ret) Return(ret);return

        Interpreter() {
            environment = new Environment();
        }

        ~Interpreter() {
            delete environment;
        }

        Object evaluate(Expr *n) {
//            static Interpreter vis;
            n->accept(*this); // this call fills the return value
            return get_expr_value();
        }

        //unsafe if value doesnt contain any value
        Object get_expr_value() {
            Object obj = *value;
            value.reset();
//            std::cout<<"RESETTING Value "<<get_string_repr(obj)<<std::endl;
            return obj;
        }

        void Return(Object value_) {
//            std::cout<<"SETTING Value "<<get_string_repr(value_)<<std::endl;
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

        void interpret(const std::vector<unique_ptr<Stmt> > &statements, bool print_expressions) {
            try {
                for (auto &stmt: statements) {
                    execute(stmt.get());
                    if (print_expressions && value) {
                        auto val = get_expr_value();
                        std::cout << get_string_repr(val) << std::endl;
                    }
                }


            }
            catch (RuntimeException &e) {
                Lox::runtime_error(e);
            }
        }

        void visit(Var *stmt) {
            Object value;
            if (stmt->initializer) {
                value = evaluate(stmt->initializer.get());
            }
            environment->define(stmt->name.lexeme, value);

        }

        void visit(Variable *expr) {
            RETURN(environment->get(expr->name));
        }

        void visit(Assign *expr) {
            Object value = evaluate(expr->value.get());
            environment->assign(expr->name, value);
            RETURN(value);
        }

        //        Object evaluate(Expr *expr) {
//            return expr->accept(this);
//        }
        void visit(Expression *expr) {
            RETURN(evaluate(expr->expression.get()));
        }

        void visit(Print *stmt) {
            Object val = evaluate(stmt->expression.get());
            std::cout << get_string_repr(val) << std::endl;
        }

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

        bool isEqual(double x, double y) {
            const double epsilon = 1e-5 /* some small number such as 1e-5 */;
            return std::abs(x - y) <= epsilon * std::abs(x);
            // see Knuth section 4.2.2 pages 217-218
        }

        void check_not_zero(Token operator_token, double d) {
            if (isEqual(d, 0.00)) {
                throw RuntimeException(operator_token, "Division by 0 error");
            };
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

        void visit(Block *stmt) {
            Environment env(this->environment);
            execute_block(stmt->statements, env);
        }

        void execute_block(VecUniquePtr<Stmt> &statements, Environment &env) {
            Environment *previous = this->environment;

            try {
                this->environment = &env;
                for (auto &stmt: statements.get()) {
                    execute(stmt.get());
                }
            }
            catch (...) {
                this->environment = previous;
            }
            this->environment = previous;
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

                    double double_right = std::any_cast<double>(right);
                    check_not_zero(expr->oper, double_right);
                    RETURN(std::any_cast<double>(left) / double_right);

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

                case PLUS: {
                    if (instanceof<std::string>(left)) {
                        auto left_str = std::any_cast<std::string>(left);
                        if (instanceof<std::string>(right)) {
                            auto right_str = std::any_cast<std::string>(right);
                            RETURN(left_str + right_str);
                        }
                        auto right_double = std::any_cast<double>(right);
                        RETURN(left_str + to_string(right_double));
                    }
                    auto left_double = std::any_cast<double>(left);
                    if (instanceof<std::string>(right)) {
                        auto right_str = std::any_cast<std::string>(right);
                        RETURN(to_string(left_double) + right_str);
                    }
                    auto right_double = std::any_cast<double>(right);
                    RETURN(left_double + right_double);
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