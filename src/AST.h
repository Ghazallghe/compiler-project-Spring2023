#ifndef AST_H
#define AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

// Forward declarations of classes used in the AST
class AST;
class Expr;
class GSM;
class Factor;
class BinaryOp;
class Assignment;
class Declaration;

// ASTVisitor class defines a visitor pattern to traverse the AST
class ASTVisitor
{
public:
  // Virtual visit functions for each AST node type
  virtual void visit(AST &) {}               // Visit the base AST node
  virtual void visit(Expr &) {}              // Visit the expression node
  virtual void visit(GSM &) = 0;             // Visit the group of expressions node
  virtual void visit(Factor &) = 0;          // Visit the factor node
  virtual void visit(BinaryOp &) = 0;        // Visit the binary operation node
  virtual void visit(Assignment &) = 0;      // Visit the assignment expression node
  virtual void visit(Declaration &) = 0;     // Visit the variable declaration node
};

// AST class serves as the base class for all AST nodes
class AST
{
public:
  virtual ~AST() {}
  virtual void accept(ASTVisitor &V) = 0;    // Accept a visitor for traversal
};

// Expr class represents an expression in the AST
class Expr : public AST
{
public:
  Expr() {}
};

// GSM class represents a group of expressions in the AST
class GSM : public Expr
{
  using ExprVector = llvm::SmallVector<Expr *>;

private:
  ExprVector exprs;                          // Stores the list of expressions

public:
  GSM(llvm::SmallVector<Expr *> exprs) : exprs(exprs) {}

  llvm::SmallVector<Expr *> getExprs() { return exprs; }

  ExprVector::const_iterator begin() { return exprs.begin(); }

  ExprVector::const_iterator end() { return exprs.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// Factor class represents a factor in the AST (either an identifier or a number)
class Factor : public Expr
{
public:
  enum ValueKind
  {
    Ident,
    Number
  };

private:
  ValueKind Kind;                            // Stores the kind of factor (identifier or number)
  llvm::StringRef Val;                       // Stores the value of the factor

public:
  Factor(ValueKind Kind, llvm::StringRef Val) : Kind(Kind), Val(Val) {}

  ValueKind getKind() { return Kind; }

  llvm::StringRef getVal() { return Val; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// BinaryOp class represents a binary operation in the AST (plus, minus, multiplication, division)
class BinaryOp : public Expr
{
public:
  enum Operator
  {
    Plus,
    Minus,
    Mul,
    Div
  };

private:
  Expr *Left;                               // Left-hand side expression
  Expr *Right;                              // Right-hand side expression
  Operator Op;                              // Operator of the binary operation

public:
  BinaryOp(Operator Op, Expr *L, Expr *R) : Op(Op), Left(L), Right(R) {}

  Expr *getLeft() { return Left; }

  Expr *getRight() { return Right; }

  Operator getOperator() { return Op; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// Assignment class represents an assignment expression in the AST
class Assignment : public Expr
{
private:
  Factor *Left;                             // Left-hand side factor (identifier)
  Expr *Right;                              // Right-hand side expression

public:
  Assignment(Factor *L, Expr *R) : Left(L), Right(R) {}

  Factor *getLeft() { return Left; }

  Expr *getRight() { return Right; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// Declaration class represents a variable declaration with an initializer in the AST
class Declaration : public Expr
{
  using VarVector = llvm::SmallVector<llvm::StringRef, 8>;
  VarVector Vars;                           // Stores the list of variables
  Expr *E;                                  // Expression serving as the initializer

public:
  Declaration(llvm::SmallVector<llvm::StringRef, 8> Vars, Expr *E) : Vars(Vars), E(E) {}

  VarVector::const_iterator begin() { return Vars.begin(); }

  VarVector::const_iterator end() { return Vars.end(); }

  Expr *getExpr() { return E; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

#endif
