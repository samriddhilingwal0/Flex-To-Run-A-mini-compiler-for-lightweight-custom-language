#pragma once

#include "SymbolTable.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <memory>
#include <map>
#include <string>

class ASTNode;
class ProgramNode;
class BlockNode;

class CodeGenContext
{
public:
    llvm::LLVMContext llvmContext;
    llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;
    std::map<std::string, llvm::Value *> namedValues;
    std::map<std::string, std::string> symbolTable; // variable name → type ✅ NEW

    llvm::Function *currentFunction = nullptr;
    llvm::BasicBlock *breakBlock = nullptr;
    llvm::BasicBlock *continueBlock = nullptr;

    void setBreakBlock(llvm::BasicBlock *block) { breakBlock = block; }
    void setContinueBlock(llvm::BasicBlock *block) { continueBlock = block; }
    CodeGenContext()
        : builder(llvmContext), module(std::make_unique<llvm::Module>("Flec", llvmContext)) {}

    llvm::Type *getLLVMType(const std::string &typeName);
    llvm::Value *generateCode(ProgramNode *root);

    void pushBreakBlock(llvm::BasicBlock *block) { breakBlock = block; }
    void popBreakBlock() { breakBlock = nullptr; }

    void pushContinueBlock(llvm::BasicBlock *block) { continueBlock = block; }
    void popContinueBlock() { continueBlock = nullptr; }

    llvm::BasicBlock *getBreakBlock() const { return breakBlock; }
    llvm::BasicBlock *getContinueBlock() const { return continueBlock; }
};
