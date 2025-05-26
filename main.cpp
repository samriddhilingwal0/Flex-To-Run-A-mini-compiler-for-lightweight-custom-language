#include "ast_interface.h"
#include "codegen.h"
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <iostream>
#include <fstream>
#include <exception>

bool semanticError = false;

extern "C" {
    int yyparse();
}

extern FILE* yyin;
extern std::unique_ptr<ProgramNode> astRoot;

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source file>\n";
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        std::cerr << "Could not open file: " << argv[1] << "\n";
        return 1;
    }

    if (yyparse() != 0) {
        std::cerr << "Parsing failed.\n";
        fclose(yyin);
        return 1;
    }

    if (astRoot) {
        try {
            std::cout << "Parsed successfully.\n";
            std::cout << "Running semantic analysis...\n";

            // Assuming semantic analysis is done here:
            std::string resultType = astRoot->analyze(symbolTable);
            if (semanticError || resultType == "Error") {
                std::cerr << "Semantic analysis failed. Aborting.\n";
                fclose(yyin);
                return 1;
            }

            CodeGenContext context;
            context.generateCode(astRoot.get());

            std::error_code EC;
            llvm::raw_fd_ostream outFile("output.ll", EC, llvm::sys::fs::OF_None);
            if (EC) {
                std::cerr << "Error opening output file: " << EC.message() << "\n";
                fclose(yyin);
                return 1;
            }
            context.module->print(outFile, nullptr);
            outFile.close();

            std::cout << "LLVM IR written to output.ll\n";
            std::cout << "Run it using: lli output.ll\n";

        } catch (const std::exception &e) {
            std::cerr << "Semantic error: " << e.what() << "\n";
            fclose(yyin);
            return 1;
        }
    } else {
        std::cerr << "AST root is null.\n";
        fclose(yyin);
        return 1;
    }

    fclose(yyin);
    return 0;
}
