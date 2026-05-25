// parser.h
#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include "symbols.h"
#include <memory>
#include <vector>
#include <string>

class Parser {
public:
    Parser(const std::vector<Token>& tokens, SymbolTable& symTable);

    bool parse();
    std::unique_ptr<ProgramNode> getAST() { return std::move(ast); }
    const std::vector<std::string>& getErrors() const { return errors; }
    void printAST(std::ostream& out) const;

private:
    std::vector<Token> tokens;
    size_t pos;
    std::unique_ptr<ProgramNode> ast;
    std::vector<std::string> errors;
    SymbolTable& symbolTable;

    // Вспомогательные методы
    Token current() const;
    Token peek(int offset = 1) const;
    void advance();
    bool match(TokenType type);
    bool expect(TokenType type, const std::string& errorMsg);
    void addError(const std::string& msg, const Token& token);

    // Методы разбора
    std::unique_ptr<ProgramNode> parseProgram();
    std::vector<std::unique_ptr<ASTNode>> parseDeclarations();
    std::unique_ptr<ASTNode> parseDeclaration();
    std::unique_ptr<VarDeclNode> parseVarDecl();
    std::unique_ptr<FuncDeclNode> parseFuncDecl();
    std::unique_ptr<BlockNode> parseBlock();
    std::vector<std::unique_ptr<ASTNode>> parseStatementSeq();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseReadStatement();
    std::unique_ptr<ASTNode> parseWriteStatement();

    // Разбор выражений
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::vector<std::unique_ptr<ASTNode>> parseExpressionList();
};

#endif