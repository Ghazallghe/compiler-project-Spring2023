#include "CodeGen.h"
#include "Parser.h"
#include "Sema.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

// Define a command-line option for specifying the input expression.
static llvm::cl::opt<std::string>
    Input(llvm::cl::Positional,
          llvm::cl::desc("<input expression>"),
          llvm::cl::init(""));

// The main function of the program.
int main(int argc, const char **argv)
{
    // Initialize the LLVM framework.
    llvm::InitLLVM X(argc, argv);

    // Parse command-line options.
    llvm::cl::ParseCommandLineOptions(argc, argv, "GSM - the expression compiler\n");

    // Create a lexer object and initialize it with the input expression.
    Lexer Lex(Input);

    // Create a parser object and initialize it with the lexer.
    Parser Parser(Lex);

    // Parse the input expression and generate an abstract syntax tree (AST).
    AST *Tree = Parser.parse();

    // Check if parsing was successful or if there were any syntax errors.
    if (!Tree || Parser.hasError())
    {
        llvm::errs() << "Syntax errors occurred\n";
        return 1;
    }

    // Perform semantic analysis on the AST.
    Sema Semantic;
    if (Semantic.semantic(Tree))
    {
        llvm::errs() << "Semantic errors occurred\n";
        return 1;
    }

    // Generate code for the AST using a code generator.
    CodeGen CodeGenerator;
    CodeGenerator.compile(Tree);

    // The program executed successfully.
    return 0;
}
