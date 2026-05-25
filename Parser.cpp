// parser.cpp - ПОЛНАЯ ВЕРСИЯ С ПОДДЕРЖКОЙ ТАБЛИЦЫ СИМВОЛОВ
#include "parser.h"
#include <iostream>
#include <cctype>

Parser::Parser(const std::vector<Token>& tokens, SymbolTable& symTable)
    : tokens(tokens), pos(0), symbolTable(symTable) {}

Token Parser::current() const {
    if (pos >= tokens.size()) {
        Token eof;
        eof.type = TokenType::END_OF_FILE;
        eof.value = "";
        eof.line = -1;
        eof.column = -1;
        return eof;
    }
    return tokens[pos];
}

Token Parser::peek(int offset) const {
    size_t newPos = pos + offset;
    if (newPos >= tokens.size()) {
        Token eof;
        eof.type = TokenType::END_OF_FILE;
        eof.value = "";
        eof.line = -1;
        eof.column = -1;
        return eof;
    }
    return tokens[newPos];
}

void Parser::advance() {
    if (pos < tokens.size()) pos++;
}

bool Parser::match(TokenType type) {
    if (current().type == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::expect(TokenType type, const std::string& errorMsg) {
    if (current().type == type) {
        advance();
        return true;
    }
    addError(errorMsg + ", found '" + current().value + "'", current());
    return false;
}

void Parser::addError(const std::string& msg, const Token& token) {
    errors.push_back("Syntax error at line " + std::to_string(token.line) +
        ", col " + std::to_string(token.column) + ": " + msg);
}

// ============================================
// ОСНОВНЫЕ МЕТОДЫ РАЗБОРА
// ============================================

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();

    if (current().type == TokenType::KW_PROGRAM) {
        advance();
        if (current().type == TokenType::IDENTIFIER) {
            advance();
        }
        expect(TokenType::OP_SEMICOLON, "Expected ';' after program name");
    }

    program->declarations = parseDeclarations();
    program->block = parseBlock();
    expect(TokenType::OP_DOT, "Expected '.' at end of program");

    return program;
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseDeclarations() {
    std::vector<std::unique_ptr<ASTNode>> decls;

    while (current().type == TokenType::KW_VAR ||
        current().type == TokenType::KW_FUNCTION) {
        auto decl = parseDeclaration();
        if (decl) {
            decls.push_back(std::move(decl));
        }
        else {
            break;
        }
    }

    return decls;
}

std::unique_ptr<ASTNode> Parser::parseDeclaration() {
    if (current().type == TokenType::KW_VAR) {
        return parseVarDecl();
    }
    else if (current().type == TokenType::KW_FUNCTION) {
        return parseFuncDecl();
    }
    return nullptr;
}

std::unique_ptr<VarDeclNode> Parser::parseVarDecl() {
    auto varDecl = std::make_unique<VarDeclNode>();

    expect(TokenType::KW_VAR, "Expected 'var'");

    do {
        if (current().type == TokenType::IDENTIFIER) {
            std::string varName = current().value;

            // Добавляем переменную в таблицу символов
            std::string error;
            if (!symbolTable.declareVariable(varName, SymbolType::INTEGER, error)) {
                addError(error, current());
                return nullptr;
            }

            varDecl->names.push_back(varName);
            advance();
        }
        else {
            addError("Expected identifier in variable declaration", current());
            return nullptr;
        }
    } while (match(TokenType::OP_COMMA));

    expect(TokenType::OP_COLON, "Expected ':' after variable list");
    expect(TokenType::KW_INTEGER, "Expected 'integer' type");
    varDecl->type = "integer";
    expect(TokenType::OP_SEMICOLON, "Expected ';' after variable declaration");

    return varDecl;
}

std::unique_ptr<FuncDeclNode> Parser::parseFuncDecl() {
    auto funcDecl = std::make_unique<FuncDeclNode>();

    expect(TokenType::KW_FUNCTION, "Expected 'function'");

    if (current().type == TokenType::IDENTIFIER) {
        funcDecl->name = current().value;
        advance();
    }
    else {
        addError("Expected function name", current());
        return nullptr;
    }

    expect(TokenType::OP_LPAREN, "Expected '(' after function name");

    if (current().type != TokenType::OP_RPAREN) {
        do {
            if (current().type == TokenType::IDENTIFIER) {
                funcDecl->params.push_back(current().value);
                advance();
            }
            else {
                addError("Expected parameter name", current());
                return nullptr;
            }
        } while (match(TokenType::OP_COMMA));
    }

    expect(TokenType::OP_RPAREN, "Expected ')' after parameter list");
    expect(TokenType::OP_COLON, "Expected ':' before return type");
    expect(TokenType::KW_INTEGER, "Expected 'integer' return type");
    funcDecl->returnType = "integer";
    expect(TokenType::OP_SEMICOLON, "Expected ';' after function header");

    funcDecl->body = parseBlock();
    expect(TokenType::OP_SEMICOLON, "Expected ';' after function body");

    return funcDecl;
}

std::unique_ptr<BlockNode> Parser::parseBlock() {
    auto block = std::make_unique<BlockNode>();

    expect(TokenType::KW_BEGIN, "Expected 'begin'");

    block->statements = parseStatementSeq();

    expect(TokenType::KW_END, "Expected 'end'");

    return block;
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseStatementSeq() {
    std::vector<std::unique_ptr<ASTNode>> stmts;

    while (current().type != TokenType::KW_END &&
        current().type != TokenType::END_OF_FILE) {
        auto stmt = parseStatement();
        if (stmt) {
            stmts.push_back(std::move(stmt));
        }
        if (current().type == TokenType::OP_SEMICOLON) {
            advance();
        }
        else {
            break;
        }
    }

    return stmts;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    switch (current().type) {
    case TokenType::IDENTIFIER:
        return parseAssignment();
    case TokenType::KW_IF:
        return parseIfStatement();
    case TokenType::KW_READ:
        return parseReadStatement();
    case TokenType::KW_WRITE:
        return parseWriteStatement();
    case TokenType::KW_BEGIN:
        return parseBlock();
    default:
        addError("Unexpected token in statement", current());
        return nullptr;
    }
}

std::unique_ptr<ASTNode> Parser::parseAssignment() {
    auto assign = std::make_unique<AssignmentNode>();

    std::string varName = current().value;

    // Проверяем, объявлена ли переменная
    if (!symbolTable.isDeclared(varName)) {
        addError("Variable '" + varName + "' not declared", current());
        return nullptr;
    }

    assign->name = varName;
    advance();

    expect(TokenType::OP_ASSIGN, "Expected ':=' in assignment");

    assign->expression = parseComparison();

    return assign;
}

std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    auto ifNode = std::make_unique<IfNode>();

    expect(TokenType::KW_IF, "Expected 'if'");

    ifNode->condition = parseComparison();

    expect(TokenType::KW_THEN, "Expected 'then'");

    ifNode->thenBranch = parseStatement();

    if (current().type == TokenType::KW_ELSE) {
        advance();
        ifNode->elseBranch = parseStatement();
    }

    return ifNode;
}

std::unique_ptr<ASTNode> Parser::parseReadStatement() {
    auto readNode = std::make_unique<ReadNode>();

    expect(TokenType::KW_READ, "Expected 'read'");
    expect(TokenType::OP_LPAREN, "Expected '(' after read");

    do {
        if (current().type == TokenType::IDENTIFIER) {
            std::string varName = current().value;

            // Проверяем, объявлена ли переменная
            if (!symbolTable.isDeclared(varName)) {
                addError("Variable '" + varName + "' not declared in read", current());
                return nullptr;
            }

            readNode->variables.push_back(varName);
            advance();
        }
        else {
            addError("Expected identifier in read statement", current());
            return nullptr;
        }
    } while (match(TokenType::OP_COMMA));

    expect(TokenType::OP_RPAREN, "Expected ')' after read arguments");

    return readNode;
}

std::unique_ptr<ASTNode> Parser::parseWriteStatement() {
    auto writeNode = std::make_unique<WriteNode>();

    expect(TokenType::KW_WRITE, "Expected 'write'");
    expect(TokenType::OP_LPAREN, "Expected '(' after write");

    writeNode->expressions = parseExpressionList();

    expect(TokenType::OP_RPAREN, "Expected ')' after write arguments");

    return writeNode;
}

// ============================================
// РАЗБОР ВЫРАЖЕНИЙ (С ПРИОРИТЕТАМИ)
// ============================================

// Comparison ::= Expression { ("<" | ">" | "<=" | ">=" | "=" | "<>") Expression }
std::unique_ptr<ASTNode> Parser::parseComparison() {
    auto left = parseExpression();

    while (true) {
        std::string op;
        if (current().type == TokenType::OP_LT) {
            op = "<";
        }
        else if (current().type == TokenType::OP_GT) {
            op = ">";
        }
        else if (current().type == TokenType::OP_LE) {
            op = "<=";
        }
        else if (current().type == TokenType::OP_GE) {
            op = ">=";
        }
        else if (current().type == TokenType::OP_EQ_SINGLE) {
            op = "=";
        }
        else if (current().type == TokenType::OP_NE) {
            op = "<>";
        }
        else {
            break;
        }

        advance();
        auto right = parseExpression();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }

    return left;
}

// Expression ::= Term { ("+" | "-") Term }
std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto left = parseTerm();

    while (current().type == TokenType::OP_PLUS ||
        current().type == TokenType::OP_MINUS) {
        std::string op = current().value;
        advance();
        auto right = parseTerm();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }

    return left;
}

// Term ::= Factor { ("*" | "/") Factor }
std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto left = parseFactor();

    while (current().type == TokenType::OP_STAR ||
        current().type == TokenType::OP_SLASH) {
        std::string op = current().value;
        advance();
        auto right = parseFactor();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }

    return left;
}

// Factor ::= Number | Identifier [ "(" ExpressionList ")" ] | "(" Expression ")" | "+" Factor | "-" Factor
std::unique_ptr<ASTNode> Parser::parseFactor() {
    // Number
    if (current().type == TokenType::NUMBER) {
        int value = std::stoi(current().value);
        advance();
        return std::make_unique<NumberNode>(value);
    }

    // Identifier or Function Call
    if (current().type == TokenType::IDENTIFIER) {
        std::string name = current().value;
        advance();

        // Проверяем, объявлена ли переменная (если это не вызов функции)
        if (current().type != TokenType::OP_LPAREN) {
            if (!symbolTable.isDeclared(name)) {
                addError("Variable '" + name + "' not declared", current());
                return nullptr;
            }
        }

        // Function call?
        if (current().type == TokenType::OP_LPAREN) {
            advance();
            auto call = std::make_unique<FuncCallNode>();
            call->name = name;
            if (current().type != TokenType::OP_RPAREN) {
                call->arguments = parseExpressionList();
            }
            expect(TokenType::OP_RPAREN, "Expected ')' after function arguments");
            return call;
        }

        // Variable
        return std::make_unique<IdentifierNode>(name);
    }

    // Parenthesized expression
    if (current().type == TokenType::OP_LPAREN) {
        advance();
        auto expr = parseComparison();
        expect(TokenType::OP_RPAREN, "Expected ')' after expression");
        return expr;
    }

    // Unary plus/minus
    if (current().type == TokenType::OP_PLUS) {
        advance();
        auto operand = parseFactor();
        return std::make_unique<UnaryOpNode>("+", std::move(operand));
    }

    if (current().type == TokenType::OP_MINUS) {
        advance();
        auto operand = parseFactor();
        return std::make_unique<UnaryOpNode>("-", std::move(operand));
    }

    addError("Unexpected token in factor", current());
    return nullptr;
}

// ExpressionList ::= Expression { "," Expression }
std::vector<std::unique_ptr<ASTNode>> Parser::parseExpressionList() {
    std::vector<std::unique_ptr<ASTNode>> exprs;

    do {
        exprs.push_back(parseComparison());
    } while (match(TokenType::OP_COMMA));

    return exprs;
}

// ============================================
// ПУБЛИЧНЫЕ МЕТОДЫ
// ============================================

bool Parser::parse() {
    ast = parseProgram();

    if (pos < tokens.size() && current().type != TokenType::END_OF_FILE) {
        addError("Extra tokens after program end", current());
        return false;
    }

    return errors.empty();
}

void Parser::printAST(std::ostream& out) const {
    out << "=== ABSTRACT SYNTAX TREE ===\n";
    if (ast) {
        ast->print(out);
    }
    else {
        out << "(no AST generated)\n";
    }
    out << "=============================\n";
}