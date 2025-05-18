// main.cpp
#include "ast_interface.h"
#include "ast.h"
#include "ast_printer.h"
#include "SymbolTable.h"

#include <iostream>

extern "C" int yyparse();

// extern ASTNodePtr astRoot; // Root of the AST
extern SymbolTable symbolTable;
extern FILE *yyin; // Input file

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: ./parser <source-file>\n";
        return 1;
    }
    std::cout << "hello just checking on you\n";
    yyin = fopen(argv[1], "r");
    if (!yyin)
    {
        std::cerr << "Could not open file " << argv[1] << "\n";
        return 1;
    }

    if (yyparse() == 0 && astRoot)
    {
        std::cout << "Parsed successfully!\nAST:\n";
        // astRoot->print();
        std::cout << "\n\nRunning semantic analysis...\n";
        try
        {
            std::string resultType = astRoot->analyze(symbolTable);
            std::cout << "Semantic analysis completed successfully.\n\n";
            symbolTable.print(); // Optional: shows declared variables and scopes
        }
        catch (const std::exception &e)
        {
            std::cerr << "Semantic error: " << e.what() << "\n";
        }
    }
    else
    {
        std::cerr << "Parsing failed.\n";
    }
    astRoot->print();

    fclose(yyin);
    return 0;
}
