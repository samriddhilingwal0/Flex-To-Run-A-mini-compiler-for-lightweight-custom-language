// codegen.cpp
#include "codegen.h"
#include "ast.h"
#include <llvm/IR/Verifier.h>
#include <iostream>

using namespace llvm;

llvm::Type *CodeGenContext::getLLVMType(const std::string &typeName)
{
    if (typeName == "int")
    {
        return llvm::Type::getInt32Ty(llvmContext);
    }
    else if (typeName == "float")
    {
        return llvm::Type::getFloatTy(llvmContext);
    }
    else if (typeName == "bool")
    {
        return llvm::Type::getInt1Ty(llvmContext);
    }
    else if (typeName == "string")
        return llvm::Type::getInt8Ty(llvmContext)->getPointerTo();

    // Add more types as needed
    return nullptr;
}

llvm::Value *CodeGenContext::generateCode(ProgramNode *root)
{
    if (!root)
    {
        std::cerr << "AST root is null.\n";
        return nullptr;
    }

    FunctionType *mainFuncType = FunctionType::get(Type::getInt32Ty(llvmContext), false);
    Function *mainFunction = Function::Create(mainFuncType, Function::ExternalLinkage, "main", module.get());
    BasicBlock *entry = BasicBlock::Create(llvmContext, "entry", mainFunction);
    builder.SetInsertPoint(entry);

    for (const auto &stmt : root->statements)
    {
        stmt->codegen(*this);
        if (builder.GetInsertBlock()->getTerminator())
        {
            // Current block has been terminated by stmt->codegen
            // You might want to create a new block to continue, or stop here.
            break;
        }
    }

    if (!builder.GetInsertBlock()->getTerminator())
    {
        builder.CreateRet(ConstantInt::get(Type::getInt32Ty(llvmContext), 0));
    }

    verifyFunction(*mainFunction);
    module->print(outs(), nullptr);

    return nullptr;
}

llvm::Value *ReturnStmtNode::codegen(CodeGenContext &context)
{
    // Implement return statement code generation
    return nullptr; // Replace with actual LLVM IR value
}
