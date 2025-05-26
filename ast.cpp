#include "ast.h"
#include "codegen.h"
#include "ast_interface.h"
#include "SymbolTable.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/DerivedTypes.h> // For llvm::PointerType
#include <iostream>
#include <memory>

using namespace std;

unique_ptr<ProgramNode> astRoot = nullptr;

//---Symanitc Analysis---

string BreakNode::analyze(SymbolTable &symbols)
{
    if (symbols.loopDepth == 0)
    {
        cerr << "Semantic Error at line " << line << ": 'stop' used outside of loop.\n";
    }
    return "void";
}

string ContinueNode::analyze(SymbolTable &symbols)
{
    if (symbols.loopDepth == 0)
    {
        cerr << "Semantic Error at line " << line << ": 'skip' used outside of loop.\n";
    }
    return "void";
}

string LiteralNode::analyze(SymbolTable &symbols)
{
    switch (type)
    {
    case Type::Int:
        return "int";
    case Type::Float:
        return "float";
    case Type::String:
        return "string";
    case Type::Char:
        return "char";
    case Type::Bool:
        return "bool";
    }
    return "unknown";
}

string IdentifierNode::analyze(SymbolTable &symbols)
{
    try
    {
        const Symbol &result = symbols.lookup(name);
        type = result.type;
        return result.type;
    }
    catch (const runtime_error &e)
    {
        cerr << "Error: " << e.what() << "\n";
        return "error";
    }
}

string DeclarationNode::analyze(SymbolTable &symbols)
{
    string exprType = expr->analyze(symbols);
    if (exprType != typeName)
    {
        cerr << "Type mismatch in declaration of '" << identifier
             << "': expected " << typeName << ", got " << exprType << "\n";
    }
    symbols.declare(identifier, typeName, lineNumber);
    return "void";
}

string AssignmentNode::analyze(SymbolTable &symbols)
{
    try
    {
        const Symbol &declaredSymbol = symbols.lookup(name);
        string valueType = value->analyze(symbols);

        if (declaredSymbol.type != valueType)
        {
            cerr << "Type mismatch in assignment to '" << name
                 << "': expected " << declaredSymbol.type << ", got " << valueType << "\n";
        }

        return "void";
    }
    catch (const runtime_error &e)
    {
        cerr << "Error: " << e.what() << "\n";
        return "error";
    }
}

string InputStmtNode::analyze(SymbolTable &symbols)
{
    try
    {
        symbols.lookup(varName); // already declared, do nothing
    }
    catch (const std::runtime_error &)
    {
        // Not declared yet, declare as default type
        symbols.declare(varName, "int", lineNumber);
    }
    return "void";
}

string BinaryExprNode::analyze(SymbolTable &symbols)
{
    string leftType = left->analyze(symbols);
    string rightType = right->analyze(symbols);

    if (leftType != rightType)
    {
        cerr << "Type mismatch in binary expression: " << leftType << " vs " << rightType << "\n";
        return "error";
    }

    switch (op)
    {
    case Op::Add:
    case Op::Sub:
    case Op::Mul:
    case Op::Div:
        return (leftType == "int" || leftType == "float") ? leftType : "error";

    case Op::Eq:
    case Op::Neq:
    case Op::Lt:
    case Op::Gt:
    case Op::Leq:
    case Op::Geq:
        return "bool";

    case Op::And:
    case Op::Or:
        if (leftType != "bool")
            cerr << "Logical operators require boolean types\n";
        return "bool";
    }
    return "error";
}

string UnaryExprNode::analyze(SymbolTable &symbols)
{
    string operandType = operand->analyze(symbols);
    if (op == Op::Not && operandType != "bool")
    {
        cerr << "Error: 'not' operator requires a boolean operand\n";
        return "error";
    }
    if (op == Op::Minus && operandType != "int" && operandType != "float")
    {
        cerr << "Error: '-' operator requires an integer or float operand\n";
        return "error";
    }
    return operandType;
}

string BlockNode::analyze(SymbolTable &symbols)
{
    symbols.enterScope();
    for (const auto &stmt : statements)
    {
        stmt->analyze(symbols);
    }
    // symbols.print();
    symbols.exitScope();
    return "void";
}

string ProgramNode::analyze(SymbolTable &symbols)
{
    // symbols.enterScope();
    for (const auto &stmt : statements)
    {
        stmt->analyze(symbols);
    }
    // symbols.exitScope();
    return "void";
}

string IfStmtNode::analyze(SymbolTable &symbols)
{
    string condType = condition->analyze(symbols);
    if (condType != "bool")
    {
        cerr << "Line " << lineNumber << ": Condition in if statement must be of type 'bool', got '" << condType << "'\n";
    }

    // symbols.enterScope();
    thenBlock->analyze(symbols);
    // symbols.exitScope();

    if (elseBlock)
    {
        // symbols.enterScope();
        elseBlock->analyze(symbols);
        // symbols.exitScope();
    }

    return "void";
}

string RepeatStmtNode::analyze(SymbolTable &symbols)
{
    string condType = condition->analyze(symbols);
    if (condType != "bool")
    {
        cerr << "Line " << lineNumber << ": Condition in repeat statement must be of type 'bool', got '" << condType << "'\n";
    }

    symbols.enterLoop();
    // symbols.enterScope();
    body->analyze(symbols);
    // symbols.exitScope();
    symbols.exitLoop();

    return "void";
}

string ReturnStmtNode::analyze(SymbolTable &symbols)
{
    string exprType = expr->analyze(symbols);
    cout << "Line " << lineNumber << ": return " << exprType << "\n";
    // You can extend this later with function return type checking.
    return exprType;
}

string PrintStmtNode::analyze(SymbolTable &symbols)
{
    expr->analyze(symbols); // Analyze the expression being printed
    return "void";
}

string BuiltinCallNode::analyze(SymbolTable &symbols)
{

    return "unknown"; // You can update this later with proper return types
}

// -------------------- Codegen for LiteralNode --------------------
llvm::Value *LiteralNode::codegen(CodeGenContext &context)
{
    switch (type)
    {
    case Type::Int:
        return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context.llvmContext), stoi(value));
    case Type::Float:
        return llvm::ConstantFP::get(llvm::Type::getFloatTy(context.llvmContext), stof(value));
    case Type::Bool:
        return llvm::ConstantInt::get(llvm::Type::getInt1Ty(context.llvmContext), value == "true");
    case Type::String:
        return context.builder.CreateGlobalStringPtr(value);

    default:
        return nullptr;
    }
}

llvm::Value *IdentifierNode::codegen(CodeGenContext &context)
{
    llvm::Value *ptr = context.namedValues[name];
    if (!ptr)
    {
        std::cerr << "Error: Undefined variable '" << name << "'\n";
        return nullptr;
    }

    llvm::Type *type = nullptr;

    // Default to int32 for now, or fetch from your own symbol table if needed
    try
    {
        type = context.getLLVMType(this->type);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Lookup failed: " << e.what() << "\n";
        return nullptr;
    }

    if (!type)
    {
        std::cerr << "Error: Cannot determine type for '" << name << "'\n";
        return nullptr;
    }

    return context.builder.CreateLoad(type, ptr, name);
}

// llvm::BasicBlock creation helper is no longer needed

llvm::Value *BinaryExprNode::codegen(CodeGenContext &context)
{
    llvm::Value *L = left->codegen(context);
    llvm::Value *R = right->codegen(context);
    if (!L || !R)
        return nullptr;

    switch (op)
    {
    case Op::Add:
        return context.builder.CreateAdd(L, R, "addtmp");
    case Op::Sub:
        return context.builder.CreateSub(L, R, "subtmp");
    case Op::Mul:
        return context.builder.CreateMul(L, R, "multmp");
    case Op::Div:
        return context.builder.CreateSDiv(L, R, "divtmp");
    case Op::Eq:
        return context.builder.CreateICmpEQ(L, R, "eqtmp");
    case Op::Neq:
        return context.builder.CreateICmpNE(L, R, "netmp");
    case Op::Lt:
        return context.builder.CreateICmpSLT(L, R, "lttmp");
    case Op::Gt:
        return context.builder.CreateICmpSGT(L, R, "gttmp");
    case Op::Leq:
        return context.builder.CreateICmpSLE(L, R, "leqtmp");
    case Op::Geq:
        return context.builder.CreateICmpSGE(L, R, "geqtmp");
    case Op::And:
        return context.builder.CreateAnd(L, R, "andtmp");
    case Op::Or:
        return context.builder.CreateOr(L, R, "ortmp");
    default:
        return nullptr;
    }
}

llvm::Value *UnaryExprNode::codegen(CodeGenContext &context)
{
    llvm::Value *val = operand->codegen(context);
    if (!val)
        return nullptr;

    switch (op)
    {
    case Op::Not:
        return context.builder.CreateNot(val, "nottmp");
    case Op::Minus:
        return context.builder.CreateNeg(val, "negtmp");
    default:
        return nullptr;
    }
}

llvm::Value *DeclarationNode::codegen(CodeGenContext &context)
{
    llvm::Type *llvmType = context.getLLVMType(typeName);
    auto *alloca = context.builder.CreateAlloca(llvmType, nullptr, identifier);
    llvm::Value *initVal = expr->codegen(context);
    context.builder.CreateStore(initVal, alloca);
    context.namedValues[identifier] = alloca;
    return alloca;
}

llvm::Value *AssignmentNode::codegen(CodeGenContext &context)
{
    llvm::Value *ptr = context.namedValues[name];
    if (!ptr)
    {
        cerr << "Undefined variable: " << name << endl;
        return nullptr;
    }
    llvm::Value *val = value->codegen(context);
    context.builder.CreateStore(val, ptr);
    return val;
}

llvm::Value *PrintStmtNode::codegen(CodeGenContext &context)
{
    llvm::Value *val = expr->codegen(context);
    if (!val)
        return nullptr;

    // Declare printf if not already declared
    llvm::Function *printfFunc = context.module->getFunction("printf");
    if (!printfFunc)
    {
        llvm::FunctionType *printfType = llvm::FunctionType::get(
            llvm::IntegerType::getInt32Ty(context.llvmContext),
            llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context.llvmContext)),
            true);
        printfFunc = llvm::Function::Create(
            printfType, llvm::Function::ExternalLinkage, "printf", context.module.get());
    }

    llvm::Type *valType = val->getType();
    llvm::Value *formatStr = nullptr;

    if (valType->isIntegerTy(32))
    {
        formatStr = context.builder.CreateGlobalStringPtr("%d\n", "fmtint");
    }
    else if (valType->isFloatTy())
    {
        val = context.builder.CreateFPExt(val, llvm::Type::getDoubleTy(context.llvmContext), "floatToDouble");
        formatStr = context.builder.CreateGlobalStringPtr("%f\n", "fmtfloat");
    }
    else if (valType->isIntegerTy(1))
    {
        formatStr = context.builder.CreateGlobalStringPtr("%d\n", "fmtbool");
    }
    else if (valType->isPointerTy())
    {
        // In LLVM 18+, pointers are opaque, so no getElementType() anymore.
        // Instead, compare type to i8* for strings
        llvm::Type *i8PtrType = llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context.llvmContext));
        if (valType == i8PtrType)
        {
            formatStr = context.builder.CreateGlobalStringPtr("%s\n", "fmtstr");
        }
        else
        {
            std::cerr << "PrintStmtNode: Unsupported pointer type for printing.\n";
            return nullptr;
        }
    }
    else
    {
        std::cerr << "PrintStmtNode: Unsupported type for printing.\n";
        return nullptr;
    }

    return context.builder.CreateCall(printfFunc, {formatStr, val}, "printcall");
}

llvm::Value *BlockNode::codegen(CodeGenContext &context)
{
    for (const auto &stmt : statements)
        stmt->codegen(context);
    return nullptr;
}

llvm::Value *ProgramNode::codegen(CodeGenContext &context)
{
    for (const auto &stmt : statements)
        stmt->codegen(context);
    return nullptr;
}

llvm::Value *IfStmtNode::codegen(CodeGenContext &context)
{
    llvm::Value *cond = condition->codegen(context);
    cond = context.builder.CreateICmpNE(cond, llvm::ConstantInt::getFalse(context.llvmContext), "ifcond");

    llvm::Function *func = context.builder.GetInsertBlock()->getParent();

    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context.llvmContext, "then", func);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(context.llvmContext, "else", func);
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context.llvmContext, "ifcont", func);

    context.builder.CreateCondBr(cond, thenBB, elseBB);

    context.builder.SetInsertPoint(thenBB);
    thenBlock->codegen(context);
    context.builder.CreateBr(mergeBB);
    thenBB = context.builder.GetInsertBlock();

    context.builder.SetInsertPoint(elseBB);
    if (elseBlock)
        elseBlock->codegen(context);
    context.builder.CreateBr(mergeBB);
    elseBB = context.builder.GetInsertBlock();

    context.builder.SetInsertPoint(mergeBB);

    return nullptr;
}

llvm::Value *RepeatStmtNode::codegen(CodeGenContext &context)
{
    llvm::Function *func = context.builder.GetInsertBlock()->getParent();

    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(context.llvmContext, "loop", func);
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(context.llvmContext, "loopcond", func);
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(context.llvmContext, "afterloop", func);

    // Jump to loop block
    context.builder.CreateBr(loopBB);

    // --- Loop body ---
    context.builder.SetInsertPoint(loopBB);

    // Save previous break/continue blocks
    llvm::BasicBlock *prevBreak = context.getBreakBlock();
    llvm::BasicBlock *prevContinue = context.getContinueBlock();

    // Set current loop's break/continue targets
    context.setBreakBlock(afterBB);
    context.setContinueBlock(condBB);

    body->codegen(context); // Inside loop

    // After body, jump to condition check
    context.builder.CreateBr(condBB);

    // --- Condition check ---
    context.builder.SetInsertPoint(condBB);
    llvm::Value *condVal = condition->codegen(context);
    condVal = context.builder.CreateICmpNE(condVal, llvm::ConstantInt::getFalse(context.llvmContext), "loopcond");

    context.builder.CreateCondBr(condVal, loopBB, afterBB);

    // --- After loop ---
    context.builder.SetInsertPoint(afterBB);

    // Restore previous break/continue targets
    context.setBreakBlock(prevBreak);
    context.setContinueBlock(prevContinue);

    return nullptr;
}

llvm::Value *BreakNode::codegen(CodeGenContext &context)
{
    if (!context.getBreakBlock())
    {
        cerr << "Error: {stop} used outside of loop.\n";
        return nullptr;
    }
    return context.builder.CreateBr(context.getBreakBlock());
}

llvm::Value *ContinueNode::codegen(CodeGenContext &context)
{
    if (!context.getContinueBlock())
    {
        cerr << "Error: {skip} used outside of loop.\n";
        return nullptr;
    }
    return context.builder.CreateBr(context.getContinueBlock());
}

llvm::Value *BuiltinCallNode::codegen(CodeGenContext &context)
{
    vector<llvm::Value *> argValues;
    for (auto &arg : args)
    {
        llvm::Value *val = arg->codegen(context);
        if (!val)
            return nullptr;
        argValues.push_back(val);
    }

    llvm::Function *callee = context.module->getFunction(funcName);
    if (!callee)
    {
        cerr << "Error: unknown built-in function: " << funcName << endl;
        return nullptr;
    }

    return context.builder.CreateCall(callee, argValues, funcName + "_call");
}

/*llvm::Value *InputStmtNode::codegen(CodeGenContext &context)
{
    llvm::Function *scanfFunc = context.module->getFunction("scanf");
    if (!scanfFunc)
    {
        llvm::FunctionType *scanfType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context.llvmContext),
            llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context.llvmContext)),
            true);
        scanfFunc = llvm::Function::Create(
            scanfType, llvm::Function::ExternalLinkage, "scanf", context.module.get());
    }

    llvm::Value *ptr = context.namedValues[varName];
    if (!ptr)
    {
        llvm::Type *intTy = llvm::Type::getInt32Ty(context.llvmContext);
        ptr = context.builder.CreateAlloca(intTy, nullptr, varName);
        context.namedValues[varName] = ptr;
    }

    llvm::Value *formatStr = context.builder.CreateGlobalStringPtr("%d");
    return context.builder.CreateCall(scanfFunc, {formatStr, ptr});
}*/
llvm::Value *InputStmtNode::codegen(CodeGenContext &context)
{
    // Declare scanf if not already present
    llvm::Function *scanfFunc = context.module->getFunction("scanf");
    if (!scanfFunc)
    {
        llvm::FunctionType *scanfType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context.llvmContext),
            llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context.llvmContext)),
            true // variadic
        );
        scanfFunc = llvm::Function::Create(
            scanfType, llvm::Function::ExternalLinkage, "scanf", context.module.get());
    }

    llvm::Type *llvmType = nullptr;
    std::string fmt;
    llvm::Value *ptr = nullptr;

    // Choose LLVM type and format string
    if (inputType == "int")
    {
        llvmType = llvm::Type::getInt32Ty(context.llvmContext);
        fmt = "%d";
    }
    else if (inputType == "float")
    {
        llvmType = llvm::Type::getFloatTy(context.llvmContext);
        fmt = "%f";
    }
    else if (inputType == "bool")
    {
        llvmType = llvm::Type::getInt32Ty(context.llvmContext); // use i32 for scanf
        fmt = "%d";
    }
    else if (inputType == "string")
    {
        llvmType = llvm::ArrayType::get(llvm::Type::getInt8Ty(context.llvmContext), 256);
        fmt = "%s";
    }
    else
    {
        std::cerr << "Unsupported input type: " << inputType << std::endl;
        return nullptr;
    }

    // Allocate variable if not already allocated
    ptr = context.namedValues[varName];
    if (!ptr)
    {
        ptr = context.builder.CreateAlloca(llvmType, nullptr, varName);
        context.namedValues[varName] = ptr;
    }

    // For string, cast array ptr to i8*
    if (inputType == "string")
    {
        ptr = context.builder.CreatePointerCast(
            ptr, llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context.llvmContext)));
    }

    // Create format string for scanf
    llvm::Value *formatStr = context.builder.CreateGlobalStringPtr(fmt);

    // Call scanf
    llvm::Value *call = context.builder.CreateCall(scanfFunc, {formatStr, ptr});

    // If bool, convert stored i32 to i1 and store back
    if (inputType == "bool")
    {
        llvm::Value *intVal = context.builder.CreateLoad(
            llvm::Type::getInt32Ty(context.llvmContext), ptr);

        llvm::Value *boolVal = context.builder.CreateICmpNE(
            intVal,
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(context.llvmContext), 0));

        llvm::Value *boolPtr = context.namedValues[varName + "_bool"];
        if (!boolPtr)
        {
            boolPtr = context.builder.CreateAlloca(
                llvm::Type::getInt1Ty(context.llvmContext), nullptr, varName + "_bool");
            context.namedValues[varName + "_bool"] = boolPtr;
        }

        context.builder.CreateStore(boolVal, boolPtr);

        // Optionally: update main varName to i1 bool pointer
        context.namedValues[varName] = boolPtr;
    }

    // Update symbol table
    context.symbolTable[varName] = inputType;

    return call;
}

// These destructors must be defined even if they’re empty. This forces the compiler to emit the vtable.
LiteralNode::~LiteralNode() {}
IdentifierNode::~IdentifierNode() {}
BinaryExprNode::~BinaryExprNode() {}
UnaryExprNode::~UnaryExprNode() {}
DeclarationNode::~DeclarationNode() {}
PrintStmtNode::~PrintStmtNode() {}
ReturnStmtNode::~ReturnStmtNode() {}
IfStmtNode::~IfStmtNode() {}
RepeatStmtNode::~RepeatStmtNode() {}
AssignmentNode::~AssignmentNode() {}
BlockNode::~BlockNode() {}
ProgramNode::~ProgramNode() {}
BreakNode::~BreakNode() {}
ContinueNode::~ContinueNode() {}
BuiltinCallNode::~BuiltinCallNode() {}