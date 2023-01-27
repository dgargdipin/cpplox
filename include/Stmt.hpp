#pragma once
#include<any>
#include<memory>
#include "types.h"
#include "scanner.h"
#include "Expr.hpp"
class Stmt;
class Expression;
class Print;
class Block;
class Var;
class If;
class While;
class StmtVisitor{
    public:
   virtual void visit(Expression *stmt)=0;
   virtual void visit(Print *stmt)=0;
   virtual void visit(Block *stmt)=0;
   virtual void visit(Var *stmt)=0;
   virtual void visit(If *stmt)=0;
   virtual void visit(While *stmt)=0;
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
class Block: public Stmt{
   public:
   Lox::VecUniquePtr<Stmt> statements;
   public:
 Block(Lox::VecUniquePtr<Stmt> statements):statements(statements){};
MAKE_VISITABLE_Stmt
};
class Var: public Stmt{
   public:
   Token name;
   std::unique_ptr<Expr > initializer;
   public:
 Var(Token name,std::unique_ptr<Expr >& initializer):name(name),initializer(std::move(initializer)){};
MAKE_VISITABLE_Stmt
};
class If: public Stmt{
   public:
   std::unique_ptr<Expr > condition;
   std::unique_ptr<Stmt > then_branch;
   std::unique_ptr<Stmt > else_branch;
   public:
 If(std::unique_ptr<Expr >& condition,std::unique_ptr<Stmt >& then_branch,std::unique_ptr<Stmt >& else_branch):condition(std::move(condition)),then_branch(std::move(then_branch)),else_branch(std::move(else_branch)){};
MAKE_VISITABLE_Stmt
};
class While: public Stmt{
   public:
   std::unique_ptr<Expr > condition;
   std::unique_ptr<Stmt > body;
   public:
 While(std::unique_ptr<Expr >& condition,std::unique_ptr<Stmt >& body):condition(std::move(condition)),body(std::move(body)){};
MAKE_VISITABLE_Stmt
};
