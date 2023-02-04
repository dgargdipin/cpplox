#pragma once


#include "Environment.h"
#include "Stmt.hpp"
#include "Callable.h"

namespace Lox {
    class Callable;

    class Interpreter : public ExprVisitor, StmtVisitor {
        std::unique_ptr<Object> value; //value for exprvisitor
//        Object value;
        void execute(Stmt *stmt);

        virtual void visit(Return *stmt);

        Callable *global_clock;
    public:
        Environment *global, *environment;
#define RETURN(ret) Return(ret);return

        Interpreter();

        ~Interpreter();

        Object evaluate(Expr *n);

        //unsafe if value doesnt contain any value
        Object get_expr_value();

        void Return(Object value_);


        void interpret(VecUniquePtr<Stmt> &statements, bool print_expressions);

        void visit(Var *stmt);

        void visit(While *stmt);

        void visit(Logical *expr);

        void visit(Function *stmt);

        void visit(If *stmt);

        void visit(Break *stmt);

        void visit(Variable *expr);

        void visit(Assign *expr);

        void visit(Expression *expr);

        void visit(Print *stmt);

        void visit(Literal *expr);;

        void visit(Grouping *expr);;

        void visit(Ternary *expr);

        void visit(Call *expr);

        void visit(Nothing *expr);

        void check_number_operand(Token operator_token, Object &operand);

        void check_number_operands(Token operator_token, Object &left, Object &right);

        bool isEqual(double x, double y);

        void check_not_zero(Token operator_token, double d);

        void visit(Unary *expr);

        void visit(Block *stmt);

        void execute_block(VecUniquePtr<Stmt> &statements, Environment *env);

        void visit(Binary *expr);

        bool isTruthy(Object val);;

        bool isEqual(Object &val1, Object &val2);

        bool isNull(Object &obj);


        bool same_type(Object &obj1, Object &obj2);


    };


}