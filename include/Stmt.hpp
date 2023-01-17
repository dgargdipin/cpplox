#pragma once
#include<any>
#include<memory>
#include "scanner.h"
#include "Expr.hpp"
class Stmt;
class Expression;
class Print;
class StmtVisitor{
    public:
   virtual void visit(Expression *stmt)=0;
   virtual void visit(Print *stmt)=0;
   virtual ~StmtVisitor()=default;
};
class Stmt{
 public:
   virtual void accept(StmtVisitor& visitor)=0;
   virtual ~Stmt()=default;
#define MAKE_VISITABLE_Stmt virtual void accept(StmtVisitor& vis) override { vis.visit(this);}
};
class Expression: public Stmt{
   public:
   std::unique_ptr<Expr > expression;
   public:
 Expression(std::unique_ptr<Expr >& expression):expression(std::move(expression)){};
MAKE_VISITABLE_Stmt
};
class Print: public Stmt{
   public:
   std::unique_ptr<Expr > expression;
   public:
 Print(std::unique_ptr<Expr >& expression):expression(std::move(expression)){};
MAKE_VISITABLE_Stmt
};
