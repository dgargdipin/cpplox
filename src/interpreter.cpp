//
// Created by Dipin Garg on 30-01-2023.
//
#include "interpreter.h"
#include <string>
#include "LoxExceptions.h"
#include <stdexcept>
#include "Stmt.hpp"
#include <algorithm>
#include "utils.h"
#include <cmath>
#include "types.h"
#include "token.h"
#include "lox.h"
#include "Callable.h"
#include "Clock.h"
#include "LoxFunction.h"

using std::unique_ptr;
namespace Lox {

    Object Interpreter::evaluate(Expr *n) {
//            static Interpreter vis;
        n->accept(*this); // this call fills the return value
        return get_expr_value();
    }

    Object Interpreter::get_expr_value() {
        Object obj = *value;
        value.reset();
//            std::cout<<"RESETTING Value "<<get_string_repr(obj)<<std::endl;
        return obj;
    }

    void Interpreter::Return(Object value_) {
//            std::cout<<"SETTING Value "<<get_string_repr(value_)<<std::endl;
        value = std::make_unique<Object>(value_);
    }

    void Interpreter::interpret(VecUniquePtr<Stmt> &statements, bool print_expressions) {
        try {
            for (auto &stmt: statements.get()) {
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

    void Interpreter::visit(Var *stmt) {
        Object value;
        if (stmt->initializer) {
            value = evaluate(stmt->initializer.get());
        }
        environment->define(stmt->name.lexeme, value);

    }

    void Interpreter::visit(While *stmt) {
        try {
            while (isTruthy(evaluate(stmt->condition.get()))) {
                execute(stmt->body.get());
            }
        }
        catch (BreakException &e) {

        }
    }

    void Interpreter::visit(Logical *expr) {
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

    void Interpreter::visit(If *stmt) {

        auto condition = evaluate(stmt->condition.get());
        if (isTruthy(condition)) {
            execute(stmt->then_branch.get());
        } else {
            if (stmt->else_branch)execute(stmt->else_branch.get());
        }


    }

    void Interpreter::visit(Break *stmt) {
        throw BreakException();
    }

    void Interpreter::visit(Variable *expr) {
        Object val = environment->get(expr->name);
        RETURN(val);
//            RETURN(environment->get(expr->name));
    }

    void Interpreter::visit(Assign *expr) {
        Object value = evaluate(expr->value.get());
        environment->assign(expr->name, value);
        RETURN(value);
    }

    void Interpreter::visit(Expression *expr) {
        RETURN(evaluate(expr->expression.get()));
    }

    void Interpreter::visit(Print *stmt) {
        Object val = evaluate(stmt->expression.get());
        std::cout << get_string_repr(val) << std::endl;
    }

    void Interpreter::visit(Literal *expr) {
        RETURN(expr->value);
    }

    void Interpreter::visit(Grouping *expr) {
        RETURN(evaluate(expr->expression.get()));

    }

    void Interpreter::visit(Ternary *expr) {

        Object condition_object = evaluate(expr->condition.get());
        bool condition = isTruthy(condition_object);
        if (condition) {
            RETURN(evaluate(expr->left.get()));

        }
        RETURN(evaluate(expr->right.get()));

    }

    void Interpreter::visit(Nothing *expr) {
        throw std::runtime_error("Runtime error");
    }

    void Interpreter::check_number_operand(Token operator_token, Object &operand) {
        if (lox_object_type<double>(operand))return;
        throw RuntimeException(operator_token, "Operand must be a number");
    }

    void Interpreter::check_number_operands(Token operator_token, Object &left, Object &right) {
        if (lox_object_type<double>(left) && lox_object_type<double>(right))return;
        throw RuntimeException(operator_token, "Operands must be numbers");
    }

    bool Interpreter::isEqual(double x, double y) {
        const double epsilon = 1e-5 /* some small number such as 1e-5 */;
        return std::abs(x - y) <= epsilon * std::abs(x);
        // see Knuth section 4.2.2 pages 217-218
    }

    void Interpreter::check_not_zero(Token operator_token, double d) {
        if (isEqual(d, 0.00)) {
            throw RuntimeException(operator_token, "Division by 0 error");
        };
    }

    void Interpreter::visit(Unary *expr) {
        Object right = evaluate(expr->right.get());
        switch (expr->oper.type) {
            case MINUS:
                check_number_operand(expr->oper, right);
                RETURN(-std::any_cast<double>(right));

            case BANG:
            RETURN(!isTruthy(right));

        }
    }

    void Interpreter::visit(Block *stmt) {
        Environment *env = new Environment(this->environment);
        execute_block(stmt->statements, env);
    }

    void Interpreter::execute_block(VecUniquePtr<Stmt> &statements, Environment *env) {
//            std::cout<<"Executing block\n";
        Environment *previous = this->environment;


        this->environment = env;


        try {
            for (auto &stmt: statements.get()) {
                execute(stmt.get());
            }
        }
        catch (const BreakException &e) {
            this->environment = previous;
            throw e;
        }
        catch (const ReturnException &e) {
            this->environment = previous;
            throw e;
        }

        this->environment = previous;
//            std::cout<<"Exiting block\n";
    }

    void Interpreter::visit(Binary *expr) {
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

    bool Interpreter::isTruthy(Object val) {
        if (!val.has_value())
            return false;
        try {
            auto typed_val = std::any_cast<bool>(val);
            return typed_val;
        }
        catch (const std::bad_any_cast &e) {
        };
        return true;
    }

    bool Interpreter::isEqual(Object &val1, Object &val2) {
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

    bool Interpreter::isNull(Object &obj) {
        return !obj.has_value();
    }

    bool Interpreter::same_type(Object &obj1, Object &obj2) {
        return obj1.type() == obj2.type();
    }

    void Interpreter::execute(Stmt *stmt) {
        stmt->accept(*this);
    }

    Interpreter::Interpreter() {
        global = new Environment();
        environment = global;
        global_clock = (Callable *) (new Clock());

        global->define("clock", global_clock);
    }

    Interpreter::~Interpreter() {
        delete global;
        delete global_clock;
    }

    void Interpreter::visit(Call *expr) {
        Object callee = evaluate(expr->callee.get());
        std::vector<Object> arguments;
        for (auto &argument: expr->arguments.get()) {
            arguments.push_back(evaluate(argument.get()));
        }
        try {
            Callable *function = lox_object_cast<Callable *>(callee);
            if (arguments.size() != function->arity()) {
                throw RuntimeException(expr->paren, "Expected " +
                                                    to_string(function->arity()) + " arguments but got " +
                                                    to_string(arguments.size()) + ".");
            }

            RETURN(function->call(*this, arguments));
        }
        catch (std::bad_any_cast &e) {
            throw RuntimeException(expr->paren,
                                   "Can only call functions and classes.");

        }


    }

    void Interpreter::visit(Function *stmt) {
        LoxFunction *fn = new LoxFunction(stmt, environment);
        Callable *callable_fn = (Callable *) fn;
        environment->define(stmt->name.lexeme, callable_fn);

    }

    void Interpreter::visit(class Return *stmt) {
        Object value;
        if (stmt->value)value = evaluate(stmt->value.get());
        throw ReturnException(value);
    }

}