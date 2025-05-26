#pragma once
#include "ast.h"
#include <memory>

using namespace std;

// an error flag
extern bool semanticError;

// The root of the AST will be stored here
extern unique_ptr<ProgramNode> astRoot;

// Functions to build AST nodes — called from parser actions
unique_ptr<LiteralNode> makeIntLiteral(int value, int line);
unique_ptr<LiteralNode> makeFloatLiteral(float value, int line);
unique_ptr<LiteralNode> makeStringLiteral(const string &value, int line);
unique_ptr<LiteralNode> makeCharLiteral(char value, int line);
unique_ptr<LiteralNode> makeBoolLiteral(bool value, int line);

unique_ptr<IdentifierNode> makeIdentifier(const string &name, int line);

unique_ptr<BinaryExprNode> makeBinaryExpr(
    unique_ptr<ASTNode> left,
    BinaryExprNode::Op op,
    unique_ptr<ASTNode> right,
    int line);

unique_ptr<UnaryExprNode> makeUnaryExpr(
    UnaryExprNode::Op op,
    unique_ptr<ASTNode> operand,
    int line);

unique_ptr<DeclarationNode> makeDeclaration(
    const string &type,
    const string &name,
    unique_ptr<ASTNode> expr,
    int line);

unique_ptr<PrintStmtNode> makePrintStmt(unique_ptr<ASTNode> expr, int line);
unique_ptr<ReturnStmtNode> makeReturnStmt(unique_ptr<ASTNode> expr, int line);

unique_ptr<IfStmtNode> makeIfStmt(
    unique_ptr<ASTNode> condition,
    unique_ptr<ASTNode> thenBlock,
    unique_ptr<ASTNode> elseBlock,
    int line);

unique_ptr<RepeatStmtNode> makeRepeatStmt(
    unique_ptr<ASTNode> condition,
    unique_ptr<ASTNode> body,
    int line);

unique_ptr<ASTNode> makeAssignment(
    const string &name,
    unique_ptr<ASTNode> expr,
    int line);

unique_ptr<BlockNode> makeBlock(
    unique_ptr<vector<unique_ptr<ASTNode>>> stmts,
    int line);

void addToBlock(BlockNode *block, unique_ptr<ASTNode> stmt);

ProgramNode *makeProgram();
// unique_ptr<ProgramNode> makeProgram();
void addToProgram(unique_ptr<ASTNode> stmt);

unique_ptr<BreakNode> makeBreak(int line);
unique_ptr<ContinueNode> makeContinue(int line);

unique_ptr<BuiltinCallNode> makeBuiltinCall(
    const string &name,
    vector<ASTNodePtr> args,
    int line);

std::unique_ptr<InputStmtNode> makeInputStmt(const std::string &type, const std::string &name, int line);
