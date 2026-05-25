// codegen.h
#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <string>
#include <vector>
#include <memory>

class CodeGenerator {
public:
    CodeGenerator(const std::string& outputFile);

    void generate(const ProgramNode& ast);
    void writeOutput();

    const std::vector<std::string>& getErrors() const { return errors; }

private:
    std::string outputFile;
    std::vector<std::string> outputLines;
    std::vector<std::string> errors;
    int labelCounter;

    std::string newLabel();
    void emit(const std::string& line);
    void addError(const std::string& msg);

    void generateBlock(const BlockNode& node);
    void generateStatement(const ASTNode* node);
    void generateAssignment(const AssignmentNode* node);
    void generateIf(const IfNode* node);
    void generateRead(const ReadNode* node);
    void generateWrite(const WriteNode* node);
    void generateExpression(const ASTNode* node);
    void generateFuncCall(const FuncCallNode* node);
};

#endif