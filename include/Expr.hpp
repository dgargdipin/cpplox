#pragma once
#include<any>
#include<memory>
#include "types.h"
#include "token.h"
class Expr;
class Binary;
class Grouping;
class Ternary;
class Literal;
class Unary;
class Nothing;
class Variable;
class Logical;
class Assign;
class ExprVisitor{
    public:
   virtual void visit(Binary *expr)=0;
   virtual void visit(Grouping *expr)=0;
   virtual void visit(Ternary *expr)=0;
   virtual void visit(Literal *expr)=0;
   virtual void visit(Unary *expr)=0;
   virtual void visit(Nothing *expr)=0;
   virtual void visit(Variable *expr)=0;
   virtual void visit(Logical *expr)=0;
   virtual void visit(Assign *expr)=0;
   virtual ~ExprVisitor()=default;
};
class Expr{
 public:
   virtual void accept(ExprVisitor& visitor)=0;
   virtual ~Expr()=default;
#define MAKE_VISITABLE_Expr virtual void accept(ExprVisitor& vis) override { vis.visit(this);}
};
class Binary: public Expr{
   public:
   std::unique_ptr<Expr > left;
   Token oper;
   std::unique_ptr<Expr > right;
   public:
 Binary(std::unique_ptr<Expr >& left,Token oper,std::unique_ptr<Expr >& right):left(std::move(left)),oper(oper),right(std::move(right)){};
MAKE_VISITABLE_Expr
};
class Grouping: public Expr{
   public:
   std::unique_ptr<Expr > expression;
   public:
 Grouping(std::unique_ptr<Expr >& expression):expression(std::move(expression)){};
MAKE_VISITABLE_Expr
};
class Ternary: public Expr{
   public:
   std::unique_ptr<Expr > condition;
   std::unique_ptr<Expr > left;
   std::unique_ptr<Expr > right;
   public:
 Ternary(std::unique_ptr<Expr >& condition,std::unique_ptr<Expr >& left,std::unique_ptr<Expr >& right):condition(std::move(condition)),left(std::move(left)),right(std::move(right)){};
MAKE_VISITABLE_Expr
};
class Literal: public Expr{
   public:
   std::any value;
   public:
 Literal(std::any value):value(value){};
MAKE_VISITABLE_Expr
};
class Unary: public Expr{
   public:
   Token oper;
   std::unique_ptr<Expr > right;
   public:
 Unary(Token oper,std::unique_ptr<Expr >& right):oper(oper),right(std::move(right)){};
MAKE_VISITABLE_Expr
};
class Nothing: public Expr{
   public:
   std::string nothing;
   public:
 Nothing(std::string nothing):nothing(nothing){};
MAKE_VISITABLE_Expr
};
class Variable: public Expr{
   public:
   Token name;
   public:
 Variable(Token name):name(name){};
MAKE_VISITABLE_Expr
};
class Logical: public Expr{
   public:
   std::unique_ptr<Expr > left;
   Token oper;
   std::unique_ptr<Expr > right;
   public:
 Logical(std::unique_ptr<Expr >& left,Token oper,std::unique_ptr<Expr >& right):left(std::move(left)),oper(oper),right(std::move(right)){};
MAKE_VISITABLE_Expr
};
class Assign: public Expr{
   public:
   Token name;
   std::unique_ptr<Expr > value;
   public:
 Assign(Token name,std::unique_ptr<Expr >& value):name(name),value(std::move(value)){};
MAKE_VISITABLE_Expr
};
