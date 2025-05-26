#include "ast_interface.h"

// Global AST root
// unique_ptr<ProgramNode> astRoot = nullptr;

// -------------------- Program --------------------

ProgramNode *makeProgram()
{
    astRoot = make_unique<ProgramNode>();
    return astRoot.get();
}

/*unique_ptr<ProgramNode> makeProgram()
{
    auto node = make_unique<ProgramNode>();
    // node->lineNumber = line;
    return node;
}

void addToProgram(unique_ptr<ASTNode> stmt)
{
    if (!stmt)
        return;
    if (!astRoot)
        astRoot = make_unique<ProgramNode>();
    astRoot->statements.push_back(move(stmt));
}*/

void addToProgram(unique_ptr<ASTNode> stmt)
{
    if (!stmt)
    {
        std::cerr << "[Parser Error] Tried to add null statement to ProgramNode!\n";
        return;
    }
    if (astRoot)
    {
        astRoot->addStatement(std::move(stmt));
    }
}

// -------------------- Literal Builders --------------------
unique_ptr<LiteralNode> makeIntLiteral(int value, int line)
{
    auto node = make_unique<LiteralNode>(LiteralNode::Type::Int, to_string(value));
    node->lineNumber = line;

    return node;
}

unique_ptr<LiteralNode> makeFloatLiteral(float value, int line)
{
    auto node = make_unique<LiteralNode>(LiteralNode::Type::Float, to_string(value));
    node->lineNumber = line;
    return node;
}

unique_ptr<LiteralNode> makeStringLiteral(const string &value, int line)
{
    auto node = make_unique<LiteralNode>(LiteralNode::Type::String, value);
    node->lineNumber = line;
    return node;
}

unique_ptr<LiteralNode> makeCharLiteral(char value, int line)
{
    auto node = make_unique<LiteralNode>(LiteralNode::Type::Char, string(1, value));
    node->lineNumber = line;
    return node;
}

unique_ptr<LiteralNode> makeBoolLiteral(bool value, int line)
{
    auto node = make_unique<LiteralNode>(LiteralNode::Type::Bool, value ? "true" : "false");
    node->lineNumber = line;
    return node;
}

// -------------------- Identifier --------------------
unique_ptr<IdentifierNode> makeIdentifier(const string &name, int line)
{
    auto node = make_unique<IdentifierNode>(name);
    node->lineNumber = line;
    return node;
}

// -------------------- Expressions --------------------
unique_ptr<BinaryExprNode> makeBinaryExpr(
    unique_ptr<ASTNode> left,
    BinaryExprNode::Op op,
    unique_ptr<ASTNode> right,
    int line)
{
    auto node = make_unique<BinaryExprNode>(move(left), op, move(right));
    node->lineNumber = line;
    return node;
}

unique_ptr<UnaryExprNode> makeUnaryExpr(UnaryExprNode::Op op, unique_ptr<ASTNode> operand, int line)
{
    auto node = make_unique<UnaryExprNode>(op, move(operand));
    node->lineNumber = line;
    return node;
}

// -------------------- Statements --------------------
unique_ptr<DeclarationNode> makeDeclaration(
    const string &type,
    const string &name,
    unique_ptr<ASTNode> expr,
    int line)
{
    auto node = make_unique<DeclarationNode>(type, name, move(expr));
    node->lineNumber = line;
    // cout << "d line no is" << line << endl;
    return node;
}

unique_ptr<PrintStmtNode> makePrintStmt(unique_ptr<ASTNode> expr, int line)
{
    auto node = make_unique<PrintStmtNode>(move(expr));
    node->lineNumber = line;
    return node;
}

std::unique_ptr<InputStmtNode> makeInputStmt(const std::string &type, const std::string &name, int line)
{
    auto node = std::make_unique<InputStmtNode>(type, name);
    node->lineNumber = line;
    return node;
}

unique_ptr<ReturnStmtNode> makeReturnStmt(unique_ptr<ASTNode> expr, int line)
{
    auto node = make_unique<ReturnStmtNode>(move(expr));
    node->lineNumber = line;
    return node;
}

unique_ptr<IfStmtNode> makeIfStmt(
    unique_ptr<ASTNode> condition,
    unique_ptr<ASTNode> thenBlock,
    unique_ptr<ASTNode> elseBlock,
    int line)
{
    auto node = make_unique<IfStmtNode>(move(condition), move(thenBlock), move(elseBlock));
    node->lineNumber = line;
    return node;
}

unique_ptr<RepeatStmtNode> makeRepeatStmt(
    unique_ptr<ASTNode> condition,
    unique_ptr<ASTNode> body,
    int line)
{
    auto node = make_unique<RepeatStmtNode>(move(condition), move(body));
    node->lineNumber = line;
    return node;
}

// -------------------- Assignment --------------------
unique_ptr<ASTNode> makeAssignment(const string &name, unique_ptr<ASTNode> expr, int line)
{
    auto node = make_unique<AssignmentNode>(name, move(expr));
    node->lineNumber = line;
    return node;
}

// -------------------- Block --------------------
unique_ptr<BlockNode> makeBlock(unique_ptr<vector<unique_ptr<ASTNode>>> statements, int line)
{
    auto node = make_unique<BlockNode>(move(statements));
    node->lineNumber = line;
    return node;
}

void addToBlock(BlockNode *block, unique_ptr<ASTNode> stmt)
{
    block->statements.push_back(move(stmt));
}

// -------------------- Break/Continue --------------------
unique_ptr<BreakNode> makeBreak(int line)
{
    auto node = make_unique<BreakNode>(line);
    node->lineNumber = line;
    return node;
}

unique_ptr<ContinueNode> makeContinue(int line)
{
    auto node = make_unique<ContinueNode>(line);
    node->lineNumber = line;
    return node;
}

// -------------------- Builtin Call --------------------
unique_ptr<BuiltinCallNode> makeBuiltinCall(const string &name, vector<ASTNodePtr> args, int line)
{
    auto node = make_unique<BuiltinCallNode>(name, move(args));
    node->lineNumber = line;
    return node;
}
