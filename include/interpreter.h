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

        void visit(While *stmt) {
            try {
                while (isTruthy(evaluate(stmt->condition.get()))) {
                    execute(stmt->body.get());
                }
            }
            catch (BreakException &e) {

            }
        }

        void visit(Logical *expr) {
            Object left = evaluate(expr->left.get());
            if (expr->oper.type == OR) {
                if (isTruthy(left)) {
                    RETURN(left);
                }
            } else {
                if (!isTruthy(left)) {
                    RETURN(left);
                }
            }
            RETURN(evaluate(expr->right.get()));

        }

        void visit(If *stmt) {

            auto condition = evaluate(stmt->condition.get());
            if (isTruthy(condition)) {
                execute(stmt->then_branch.get());
            } else {
                if(stmt->else_branch)execute(stmt->else_branch.get());
            }


        }

        void visit(Break *stmt) {
            throw BreakException();
        }

        void visit(Variable *expr) {
            Object val = environment->get(expr->name);
            RETURN(val);
//            RETURN(environment->get(expr->name));
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
            if (lox_object_type<double>(operand))return;
            throw RuntimeException(operator_token, "Operand must be a number");
        }

        void check_number_operands(Token operator_token, Object &left, Object &right) {
            if (lox_object_type<double>(left) && lox_object_type<double>(right))return;
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
            Environment *env = new Environment(this->environment);
            execute_block(stmt->statements, env);
            delete env;
        }

        void execute_block(VecUniquePtr<Stmt> &statements, Environment *env) {
//            std::cout<<"Executing block\n";
            Environment *previous = this->environment;


            this->environment = env;


            try {
                for (auto &stmt: statements.get()) {
                    execute(stmt.get());
                }
            }
            catch (BreakException &e) {
                this->environment = previous;
                throw e;
            }

            this->environment = previous;
//            std::cout<<"Exiting block\n";
        }

//        while(A)
//        B;
//        {
//
//        }

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
                    if (lox_object_type<std::string>(left)) {
                        auto left_str = std::any_cast<std::string>(left);
                        if (lox_object_type<std::string>(right)) {
                            auto right_str = std::any_cast<std::string>(right);
                            RETURN(left_str + right_str);
                        }
                        auto right_double = std::any_cast<double>(right);
                        RETURN(left_str + to_string(right_double));
                    }
                    auto left_double = std::any_cast<double>(left);
                    if (lox_object_type<std::string>(right)) {
                        auto right_str = std::any_cast<std::string>(right);
                        RETURN(to_string(left_double) + right_str);
                    }
                    auto right_double = std::any_cast<double>(right);
                    RETURN(left_double + right_double);
                }
            }

            RETURN({});

        }

        bool isTruthy(Object val) {
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
            if (lox_object_type<double>(val1)) {
                return std::any_cast<double>(val1) == std::any_cast<double>(val2);
            }
            if (lox_object_type<bool>(val1)) {
                return std::any_cast<bool>(val1) == std::any_cast<bool>(val2);
            }
            if (lox_object_type<std::string>(val1)) {
                return std::any_cast<std::string>(val1) == std::any_cast<std::string>(val2);
            }
            throw std::runtime_error("Unexpected types");
        }

        bool isNull(Object &obj) {
            return !obj.has_value();
        }


        bool same_type(Object &obj1, Object &obj2) {
            return obj1.type() == obj2.type();
        }


    };


}