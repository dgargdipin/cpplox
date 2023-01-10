#pragma once
#include<any>
#include<memory>
#include "scanner.h"
class Expr;
class Binary;
class Grouping;
class Ternary;
class Literal;
class Unary;
class Nothing;
class Visitor{
    public:
   virtual void visit(Binary *expr)=0;
   virtual void visit(Grouping *expr)=0;
   virtual void visit(Ternary *expr)=0;
   virtual void visit(Literal *expr)=0;
   virtual void visit(Unary *expr)=0;
   virtual void visit(Nothing *expr)=0;
};
class Expr{
 public:
   virtual void accept(Visitor& visitor)=0;
};
#define MAKE_VISITABLE virtual void accept(Visitor& vis) override { vis.visit(this);}
class Binary: public Expr{
   public:
   std::unique_ptr<Expr > left;
   Token oper;
   std::unique_ptr<Expr > right;
   public:
 Binary(std::unique_ptr<Expr >& left,Token oper,std::unique_ptr<Expr >& right):left(std::move(left)),oper(oper),right(std::move(right)){};
MAKE_VISITABLE
};
class Grouping: public Expr{
   public:
   std::unique_ptr<Expr > expression;
   public:
 Grouping(std::unique_ptr<Expr >& expression):expression(std::move(expression)){};
MAKE_VISITABLE
};
class Ternary: public Expr{
   public:
   std::unique_ptr<Expr > condition;
   std::unique_ptr<Expr > left;
   std::unique_ptr<Expr > right;
   public:
 Ternary(std::unique_ptr<Expr >& condition,std::unique_ptr<Expr >& left,std::unique_ptr<Expr >& right):condition(std::move(condition)),left(std::move(left)),right(std::move(right)){};
MAKE_VISITABLE
};
class Literal: public Expr{
   public:
   std::any value;
   public:
 Literal(std::any value):value(value){};
MAKE_VISITABLE
};
class Unary: public Expr{
   public:
   Token oper;
   std::unique_ptr<Expr > right;
   public:
 Unary(Token oper,std::unique_ptr<Expr >& right):oper(oper),right(std::move(right)){};
MAKE_VISITABLE
};
class Nothing: public Expr{
   public:
   std::string nothing;
   public:
 Nothing(std::string nothing):nothing(nothing){};
MAKE_VISITABLE
};
template<typename VisitorImpl, typename VisitablePtr, typename ResultType>
    class ValueGetter
    {
    public:
        static ResultType evaluate(VisitablePtr n)
        {
            static VisitorImpl vis;
            n->accept(vis); // this call fills the return value
            return vis.value;
        }

        void Return(ResultType value_)
        {
            value = value_;
        }
    private:
        ResultType value;
    };
    