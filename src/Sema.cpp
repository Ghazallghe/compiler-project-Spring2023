#include "Sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace {
class InputCheck : public ASTVisitor {
  llvm::StringSet<> Scope;
  bool HasError;

  enum ErrorType { Twice, Not };

  void error(ErrorType ET, llvm::StringRef V) {
    llvm::errs() << "Variable " << V << " "
                 << (ET == Twice ? "already" : "not")
                 << " declared\n";
    HasError = true;
  }

public:
  InputCheck() : HasError(false) {}

  bool hasError() { return HasError; }

  virtual void visit(GSM &Node) override { };

  virtual void visit(Factor &Node) override {
    if (Node.getKind() == Factor::Ident) {
      if (Scope.find(Node.getVal()) == Scope.end())
        error(Not, Node.getVal());
    }
  };

  virtual void visit(BinaryOp &Node) override {
    if (Node.getLeft())
      Node.getLeft()->accept(*this);
    else
      HasError = true;
    if (Node.getRight())
      Node.getRight()->accept(*this);
    else
      HasError = true;
  };

  virtual void visit(Assignment &Node) override {
    Factor *dest = Node.getLeft();

    dest->accept(*this);

    if (dest->getKind() == Factor::Number) {
        llvm::errs() << "Assignment destination must be an identifier.";
        HasError = true;
    }

    if (dest->getKind() == Factor::Ident) {
      if (Scope.find(dest->getVal()) == Scope.end())
        error(Not, dest->getVal());
    }

    if (Node.getRight())
      Node.getRight()->accept(*this);
  };

  virtual void visit(Declaration &Node) override {
    for (auto I = Node.begin(), E = Node.end(); I != E;
         ++I) {
      if (!Scope.insert(*I).second)
        error(Twice, *I);
    }
    if (Node.getExpr())
      Node.getExpr()->accept(*this);
    else
      HasError = true;
  };
};
}

bool Sema::semantic(AST *Tree) {
  if (!Tree)
    return false;
  InputCheck Check;
  Tree->accept(Check);
  return Check.hasError();
}