// lexer.cpp
#include "lexer.h"
#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string& source)
    : source(source), pos(0), line(1), column(1) {}

char Lexer::peek() const {
    if (pos >= source.length()) return '\0';
    return source[pos];
}

char Lexer::advance() {
    if (pos >= source.length()) return '\0';
    char c = source[pos++];
    if (c == '\n') {
        line++;
        column = 1;
    }
    else {
        column++;
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

void Lexer::skipComment() {
    if (peek() == '(' && pos + 1 < source.length() && source[pos + 1] == '*') {
        advance(); advance();
        while (pos + 1 < source.length() && !(peek() == '*' && source[pos + 1] == ')')) {
            if (peek() == '\0') break;
            advance();
        }
        if (peek() == '*') {
            advance(); advance();
        }
    }
}

Token Lexer::readIdentifier() {
    std::string value;
    while (isalnum(peek()) || peek() == '_') {
        value += advance();
    }

    static const std::unordered_map<std::string, TokenType> keywords = {
        {"program", TokenType::KW_PROGRAM},
        {"var", TokenType::KW_VAR},
        {"begin", TokenType::KW_BEGIN},
        {"end", TokenType::KW_END},
        {"integer", TokenType::KW_INTEGER},
        {"function", TokenType::KW_FUNCTION},
        {"if", TokenType::KW_IF},
        {"then", TokenType::KW_THEN},
        {"else", TokenType::KW_ELSE},
        {"read", TokenType::KW_READ},
        {"write", TokenType::KW_WRITE}
    };

    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return { it->second, value, line, column - (int)value.size() };
    }
    return { TokenType::IDENTIFIER, value, line, column - (int)value.size() };
}

Token Lexer::readNumber() {
    std::string value;
    while (isdigit(peek())) {
        value += advance();
    }
    return { TokenType::NUMBER, value, line, column - (int)value.size() };
}

Token Lexer::nextToken() {
    skipWhitespace();
    skipComment();

    if (pos >= source.length()) {
        return { TokenType::END_OF_FILE, "", line, column };
    }

    char c = peek();
    int startLine = line;
    int startCol = column;

    // Идентификатор или ключевое слово
    if (isalpha(c) || c == '_') {
        return readIdentifier();
    }

    // Число
    if (isdigit(c)) {
        return readNumber();
    }

    // Двухсимвольные операторы (проверяем сначала)
    if (c == ':' && pos + 1 < source.length() && source[pos + 1] == '=') {
        advance(); advance();
        return { TokenType::OP_ASSIGN, ":=", startLine, startCol };
    }

    if (c == '<' && pos + 1 < source.length() && source[pos + 1] == '=') {
        advance(); advance();
        return { TokenType::OP_LE, "<=", startLine, startCol };
    }

    if (c == '>' && pos + 1 < source.length() && source[pos + 1] == '=') {
        advance(); advance();
        return { TokenType::OP_GE, ">=", startLine, startCol };
    }

    if (c == '=' && pos + 1 < source.length() && source[pos + 1] == '=') {
        advance(); advance();
        return { TokenType::OP_EQ, "==", startLine, startCol };
    }

    if (c == '<' && pos + 1 < source.length() && source[pos + 1] == '>') {
        advance(); advance();
        return { TokenType::OP_NE, "<>", startLine, startCol };
    }

    // Односимвольные операторы
    advance(); // consume current char

    switch (c) {
    case '+': return { TokenType::OP_PLUS, "+", startLine, startCol };
    case '-': return { TokenType::OP_MINUS, "-", startLine, startCol };
    case '*': return { TokenType::OP_STAR, "*", startLine, startCol };
    case '/': return { TokenType::OP_SLASH, "/", startLine, startCol };
    case '(': return { TokenType::OP_LPAREN, "(", startLine, startCol };
    case ')': return { TokenType::OP_RPAREN, ")", startLine, startCol };
    case ',': return { TokenType::OP_COMMA, ",", startLine, startCol };
    case ';': return { TokenType::OP_SEMICOLON, ";", startLine, startCol };
    case '.': return { TokenType::OP_DOT, ".", startLine, startCol };
    case ':': return { TokenType::OP_COLON, ":", startLine, startCol };
    case '=': return { TokenType::OP_EQ_SINGLE, "=", startLine, startCol };
    case '<': return { TokenType::OP_LT, "<", startLine, startCol };
    case '>': return { TokenType::OP_GT, ">", startLine, startCol };
    default:
        addError("Unknown character: " + std::string(1, c));
        return { TokenType::INVALID, std::string(1, c), startLine, startCol };
    }
}

void Lexer::addError(const std::string& msg) {
    errors.push_back("Lexical error at line " + std::to_string(line) +
        ", col " + std::to_string(column) + ": " + msg);
}

void Lexer::printTokens(std::ostream& out) const {
    out << "=== LEXER OUTPUT ===\n";
    out << "Total tokens: " << tokens.size() << "\n\n";

    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& token = tokens[i];
        out << "[" << i << "] Line " << token.line << ", Col " << token.column << ": ";
        out << "Type=" << (int)token.type << ", Value='" << token.value << "'\n";
    }
}