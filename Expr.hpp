#pragma once
#include<any>
#include "scanner.h"
template <typename T>
class Expr;
template <typename T>
class Binary;
template <typename T>
class Grouping;
template <typename T>
class Literal;
template <typename T>
class Unary;
template<typename T> class Visitor{
    public:
   virtual T visitBinaryExpr(Binary<T>* expr)=0;
   virtual T visitGroupingExpr(Grouping<T>* expr)=0;
   virtual T visitLiteralExpr(Literal<T>* expr)=0;
   virtual T visitUnaryExpr(Unary<T>* expr)=0;
};
template<typename T>
class Expr{
 public:
   virtual T accept(Visitor<T>* visitor)=0;
};
template<typename T>
class Binary: public Expr<T>{
   public:
   Expr<T>* left;
   Token oper;
   Expr<T>* right;
   public:
 Binary(Expr<T>* left,Token oper,Expr<T>* right):left(left),oper(oper),right(right){};
   T accept(Visitor<T> * visitor)
  {       return visitor->visitBinaryExpr(this);
   }};
template<typename T>
class Grouping: public Expr<T>{
   public:
   Expr<T>* expression;
   public:
 Grouping(Expr<T>* expression):expression(expression){};
   T accept(Visitor<T> * visitor)
  {       return visitor->visitGroupingExpr(this);
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
   Expr<T>* right;
   public:
 Unary(Token oper,Expr<T>* right):oper(oper),right(right){};
   T accept(Visitor<T> * visitor)
  {       return visitor->visitUnaryExpr(this);
   }};
