// interpreter.h
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "symbols.h"
#include <vector>
#include <string>
#include <stack>
#include <iostream>

class Interpreter {
public:
    Interpreter(SymbolTable& symbolTable);

    bool interpret(ProgramNode* ast);
    void printResults(std::ostream& out) const;
    const std::vector<std::string>& getErrors() const { return errors; }

private:
    SymbolTable& symbols;
    std::vector<std::string> errors;
    std::stack<int> evalStack;
    bool returnFlag;
    int returnValue;

    void addError(const std::string& msg);

    void interpretProgram(ProgramNode* node);
    void interpretBlock(BlockNode* node);
    void interpretStatement(ASTNode* node);
    void interpretAssignment(AssignmentNode* node);
    void interpretIf(IfNode* node);
    void interpretRead(ReadNode* node);
    void interpretWrite(WriteNode* node);

    int evaluateExpression(ASTNode* node);
    int evaluateBinaryOp(BinaryOpNode* node);
    int evaluateUnaryOp(UnaryOpNode* node);
    int evaluateFuncCall(FuncCallNode* node);
};

#endif