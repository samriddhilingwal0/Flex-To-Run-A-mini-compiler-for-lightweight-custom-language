#pragma once
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include "SymbolTable.h"
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include "codegen.h"

using namespace llvm;
using namespace std;

class CodeGenContext;

// Base class for all AST nodes
class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual void print() const = 0;
    virtual string analyze(SymbolTable &symbols) = 0;
    virtual llvm::Value *codegen(CodeGenContext &context) = 0;
    int lineNumber;
};

using ASTNodePtr = unique_ptr<ASTNode>;

// ===== Expression Nodes =====

class LiteralNode : public ASTNode
{
public:
    enum class Type
    {
        Int,
        Float,
        String,
        Char,
        Bool
    };
    Type type;
    string value;

    ~LiteralNode() override;

    LiteralNode(Type t, const string &val) : type(t), value(val) {}

    void print() const override { cout << "Literal(" << value << ")"; }
    string analyze(SymbolTable &symbols) override;
    llvm::Value *codegen(CodeGenContext &context) override;
};

class IdentifierNode : public ASTNode
{
public:
    string name;
    string type;

    ~IdentifierNode() override;

    IdentifierNode(const string &id) : name(id) {}

    string analyze(SymbolTable &symbols) override;
    llvm::Value *codegen(CodeGenContext &context) override;

    void print() const override { cout << "Identifier(" << name << ")"; }
};

class BinaryExprNode : public ASTNode
{
public:
    enum class Op
    {
        Add,
        Sub,
        Mul,
        Div,
        Eq,
        Neq,
        Lt,
        Gt,
        Leq,
        Geq,
        And,
        Or
    };
    ASTNodePtr left;
    ASTNodePtr right;
    Op op;

    ~BinaryExprNode() override;

    BinaryExprNode(ASTNodePtr lhs, Op oper, ASTNodePtr rhs)
        : left(move(lhs)), op(oper), right(move(rhs)) {}

    string analyze(SymbolTable &symbols) override;
    llvm::Value *codegen(CodeGenContext &context) override;

    void print() const override
    {
        cout << "(";
        left->print();
        switch (op)
        {
        case Op::Add:
            cout << " + ";
            break;
        case Op::Sub:
            cout << " - ";
            break;
        case Op::Mul:
            cout << " * ";
            break;
        case Op::Div:
            cout << " / ";
            break;
        case Op::Eq:
            cout << " == ";
            break;
        case Op::Neq:
            cout << " != ";
            break;
        case Op::Lt:
            cout << " < ";
            break;
        case Op::Gt:
            cout << " > ";
            break;
        case Op::Leq:
            cout << " <= ";
            break;
        case Op::Geq:
            cout << " >= ";
            break;
        case Op::And:
            cout << " and ";
            break;
        case Op::Or:
            cout << " or ";
            break;
        }
        right->print();
        cout << ")";
    }
};

class UnaryExprNode : public ASTNode
{
public:
    enum class Op
    {
        Not,
        Minus
    };
    Op op;
    ASTNodePtr operand;

    ~UnaryExprNode() override;

    UnaryExprNode(Op o, ASTNodePtr expr) : op(o), operand(move(expr)) {}

    void print() const override
    {
        cout << "Unary(";
        switch (op)
        {
        case Op::Not:
            cout << "not ";
            break;
        case Op::Minus:
            cout << "-";
            break;
        }
        operand->print();
        cout << ")";
    }
    string analyze(SymbolTable &symbols) override;
    llvm::Value *codegen(CodeGenContext &context) override;
};

// ===== Statement Nodes =====

class DeclarationNode : public ASTNode
{
public:
    string typeName;
    string identifier;
    ASTNodePtr expr;

    ~DeclarationNode() override;

    DeclarationNode(const string &type, const string &id, ASTNodePtr e)
        : typeName(type), identifier(id), expr(move(e)) {}

    string analyze(SymbolTable &symbols) override;
    llvm::Value *codegen(CodeGenContext &context) override;

    void print() const override
    {
        cout << "Declare(" << typeName << " " << identifier << " = ";
        expr->print();
        cout << ")";
    }
};

class PrintStmtNode : public ASTNode
{
public:
    ASTNodePtr expr;

    ~PrintStmtNode() override;

    PrintStmtNode(ASTNodePtr e) : expr(move(e)) {}

    string analyze(SymbolTable &symbols) override;
    llvm::Value *codegen(CodeGenContext &context) override;

    void print() const override
    {
        cout << "Print(";
        expr->print();
        cout << ")";
    }
};

class ReturnStmtNode : public ASTNode
{
public:
    ASTNodePtr expr;

    ~ReturnStmtNode() override;

    ReturnStmtNode(ASTNodePtr e) : expr(move(e)) {}

    string analyze(SymbolTable &symbols) override;
    llvm::Value *codegen(CodeGenContext &context) override;

    void print() const override
    {
        cout << "Return(";
        expr->print();
        cout << ")";
    }
};

class IfStmtNode : public ASTNode
{
public:
    ASTNodePtr condition;
    ASTNodePtr thenBlock;
    ASTNodePtr elseBlock;

    ~IfStmtNode() override;

    IfStmtNode(ASTNodePtr cond, ASTNodePtr thenBlk, ASTNodePtr elseBlk = nullptr)
        : condition(move(cond)), thenBlock(move(thenBlk)), elseBlock(move(elseBlk)) {}

    string analyze(SymbolTable &symbols) override;
    llvm::Value *codegen(CodeGenContext &context) override;

    void print() const override
    {
        cout << "If(";
        condition->print();
        cout << ") Then ";
        thenBlock->print();
        if (elseBlock)
        {
            cout << " Else ";
            elseBlock->print();
        }
    }
};

class RepeatStmtNode : public ASTNode
{
public:
    ASTNodePtr condition;
    ASTNodePtr body;

    ~RepeatStmtNode() override;

    RepeatStmtNode(ASTNodePtr cond, ASTNodePtr blk)
        : condition(move(cond)), body(move(blk)) {}

    string analyze(SymbolTable &symbols) override;
    llvm::Value *codegen(CodeGenContext &context) override;

    void print() const override
    {
        cout << "Repeat(";
        condition->print();
        cout << ") ";
        body->print();
    }
};

class AssignmentNode : public ASTNode
{
public:
    string name;
    ASTNodePtr value;

    ~AssignmentNode() override;

    AssignmentNode(string name, ASTNodePtr value)
        : name(move(name)), value(move(value)) {}

    string analyze(SymbolTable &symbols) override;
    llvm::Value *codegen(CodeGenContext &context) override;

    void print() const override
    {
        cout << "Assignment(" << name << " = ";
        value->print();
        cout << ")";
    }
};

class BlockNode : public ASTNode
{
public:
    vector<ASTNodePtr> statements;

    ~BlockNode() override;

    BlockNode(unique_ptr<vector<ASTNodePtr>> stmts)
        : statements(move(*stmts)) {}

    string analyze(SymbolTable &symbols) override;
    llvm::Value *codegen(CodeGenContext &context) override;

    void print() const override
    {
        cout << "{ ";
        for (const auto &stmt : statements)
        {
            stmt->print();
            cout << "; ";
        }
        cout << "}";
    }
};

class InputStmtNode : public ASTNode
{
public:
    std::string inputType; // e.g., "int"
    std::string varName;   // single variable name

    ~InputStmtNode() override = default;

    InputStmtNode(const std::string &type, const std::string &var)
        : inputType(type), varName(var) {}

    void print() const override
    {
        std::cout << "InputStmt(" << inputType << ", " << varName << ")";
    }

    std::string analyze(SymbolTable &symbols) override;

    llvm::Value *codegen(CodeGenContext &context) override;
};

// ===== Program Node (Root) =====

class ProgramNode : public ASTNode
{
public:
    vector<ASTNodePtr> statements;

    ~ProgramNode() override;

    void addStatement(ASTNodePtr stmt)
    {
        statements.push_back(move(stmt));
    }

    void print() const override
    {
        cout << "Program:\n";
        for (const auto &stmt : statements)
        {
            stmt->print();
            cout << "\n";
        }
    }

    string analyze(SymbolTable &symbols) override;
    llvm::Value *codegen(CodeGenContext &context) override;
};

class BreakNode : public ASTNode
{
public:
    int line;

    ~BreakNode() override;

    BreakNode(int line) : line(line) {}

    string analyze(SymbolTable &analyzer) override;
    llvm::Value *codegen(CodeGenContext &context) override;

    void print() const override
    {
        cout << " {stop}  ";
    }
};

class ContinueNode : public ASTNode
{
public:
    int line;

    ~ContinueNode() override;

    ContinueNode(int line) : line(line) {}

    string analyze(SymbolTable &analyzer) override;
    llvm::Value *codegen(CodeGenContext &context) override;

    void print() const override
    {
        cout << " {skip}  ";
    }
};

class BuiltinCallNode : public ASTNode
{
public:
    string funcName;
    vector<ASTNodePtr> args;

    ~BuiltinCallNode() override;

    BuiltinCallNode(const string &name, vector<ASTNodePtr> arguments)
        : funcName(name), args(move(arguments)) {}

    void print() const override
    {
        cout << "BuiltinCall(" << funcName << "(";
        for (size_t i = 0; i < args.size(); ++i)
        {
            args[i]->print();
            if (i + 1 < args.size())
                cout << ", ";
        }
        cout << "))";
    }

    string analyze(SymbolTable &symbols) override;
    llvm::Value *codegen(CodeGenContext &context) override;
};
