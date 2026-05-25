// lexer.h
#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>

enum class TokenType {
    // Ключевые слова Pascal
    KW_PROGRAM, KW_VAR, KW_BEGIN, KW_END,
    KW_INTEGER, KW_FUNCTION,
    KW_IF, KW_THEN, KW_ELSE,
    KW_READ, KW_WRITE,

    // Операторы
    OP_ASSIGN,   // :=
    OP_PLUS, OP_MINUS, OP_STAR, OP_SLASH,
    OP_LPAREN, OP_RPAREN,
    OP_COMMA,
    OP_SEMICOLON,
    OP_DOT,
    OP_COLON,

    // Операторы сравнения
    OP_LT,       // <
    OP_GT,       // >
    OP_LE,       // <=
    OP_GE,       // >=
    OP_EQ,       // ==
    OP_NE,       // <>
    OP_EQ_SINGLE, // = (одинарное равно)

    // Прочее
    IDENTIFIER,
    NUMBER,
    END_OF_FILE,
    INVALID
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer {
public:
    Lexer(const std::string& source);
    Token nextToken();
    const std::vector<Token>& getTokens() const { return tokens; }
    const std::vector<std::string>& getErrors() const { return errors; }
    void printTokens(std::ostream& out) const;

private:
    std::string source;
    size_t pos;
    int line;
    int column;
    std::vector<Token> tokens;
    std::vector<std::string> errors;

    void skipWhitespace();
    void skipComment();
    Token readIdentifier();
    Token readNumber();
    char peek() const;
    char advance();
    void addError(const std::string& msg);
};

#endif