#pragma once
#include<any>
#include<memory>
#include "scanner.h"
template <typename T>
class Expr;
template <typename T>
class Binary;
template <typename T>
class Grouping;
template <typename T>
class Ternary;
template <typename T>
class Literal;
template <typename T>
class Unary;
template<typename T> class Visitor{
    public:
   virtual T visitBinaryExpr(Binary<T>*expr)=0;
   virtual T visitGroupingExpr(Grouping<T>*expr)=0;
   virtual T visitTernaryExpr(Ternary<T>*expr)=0;
   virtual T visitLiteralExpr(Literal<T>*expr)=0;
   virtual T visitUnaryExpr(Unary<T>*expr)=0;
};
template<typename T>
class Expr{
 public:
   virtual T accept(Visitor<T>* visitor)=0;
};
template<typename T>
class Binary: public Expr<T>{
   public:
   std::unique_ptr<Expr<T> > left;
   Token oper;
   std::unique_ptr<Expr<T> > right;
   public:
 Binary(std::unique_ptr<Expr<T> >& left,Token oper,std::unique_ptr<Expr<T> >& right):left(std::move(left)),oper(oper),right(std::move(right)){};
   T accept(Visitor<T> * visitor)
  {       return visitor->visitBinaryExpr(this);
   }};
template<typename T>
class Grouping: public Expr<T>{
   public:
   std::unique_ptr<Expr<T> > expression;
   public:
 Grouping(std::unique_ptr<Expr<T> >& expression):expression(std::move(expression)){};
   T accept(Visitor<T> * visitor)
  {       return visitor->visitGroupingExpr(this);
   }};
template<typename T>
class Ternary: public Expr<T>{
   public:
   std::unique_ptr<Expr<T> > condition;
   std::unique_ptr<Expr<T> > left;
   std::unique_ptr<Expr<T> > right;
   public:
 Ternary(std::unique_ptr<Expr<T> >& condition,std::unique_ptr<Expr<T> >& left,std::unique_ptr<Expr<T> >& right):condition(std::move(condition)),left(std::move(left)),right(std::move(right)){};
   T accept(Visitor<T> * visitor)
  {       return visitor->visitTernaryExpr(this);
   }};
template<typename T>
class Literal: public Expr<T>{
   public:
   std::any value;
   public:
 Literal(std::any value):value(value){};
   T accept(Visitor<T> * visitor)
  {       return visitor->visitLiteralExpr(this);
   }};
template<typename T>
class Unary: public Expr<T>{
   public:
   Token oper;
   std::unique_ptr<Expr<T> > right;
   public:
 Unary(Token oper,std::unique_ptr<Expr<T> >& right):oper(oper),right(std::move(right)){};
   T accept(Visitor<T> * visitor)
  {       return visitor->visitUnaryExpr(this);
   }};
