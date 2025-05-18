#include "symboltable.h"
#include <iostream>

SymbolTable symbolTable;

SymbolTable::SymbolTable()
{
    enterScope(); // Start with global scope
}

void SymbolTable::enterScope()
{
    // std::cout << "we are entering" << std::endl;
    scopes.emplace_back(); // New empty scope
}

void SymbolTable::exitScope()
{
    if (scopes.empty())
    {
        throw std::runtime_error("No scope to exit.");
    }
    scopes.pop_back();
}

void SymbolTable::enterLoop()
{
    ++loopDepth;
}

void SymbolTable::exitLoop()
{
    if (loopDepth > 0)
        --loopDepth;
}

bool SymbolTable::isInsideLoop() const
{
    return loopDepth > 0;
}

void SymbolTable::declare(const std::string &name, const std::string &type, int line)
{
    auto &currentScope = scopes.back();
    if (currentScope.count(name) > 0)
    {
        std::cerr << "Error: Variable '" << name << "' already declared in this scope (line " << line << ").\n";
        throw std::runtime_error("Variable '" + name + "' already declared in this scope.");
    }
    currentScope.emplace(name, Symbol(name, type, line));
}

const Symbol &SymbolTable::lookup(const std::string &name) const
{
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        auto found = it->find(name);
        if (found != it->end())
        {
            return found->second;
        }
    }
    throw std::runtime_error("Variable '" + name + "' not declared.");
}

// a bit reduntant but we are keeping for now
bool SymbolTable::isDeclared(const std::string &name) const
{
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        if (it->count(name) > 0)
        {
            return true;
        }
    }
    return false;
}

void SymbolTable::print() const
{
    std::cout << "Symbol Table (from global to inner scopes):\n";
    for (size_t i = 0; i < scopes.size(); ++i)
    {
        std::cout << "  Scope " << i << ":\n";
        for (const auto &pair : scopes[i])
        {
            const auto &sym = pair.second;
            std::cout << "    " << sym.name << " : " << sym.type
                      << " (line " << sym.lineDeclared << ")\n";
        }
    }
}
