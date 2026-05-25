// ast.h
#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

// Forward declarations
class ASTNode;
using ASTPtr = std::unique_ptr<ASTNode>;

// Базовый класс для всех узлов AST
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(std::ostream& out, int indent = 0) const = 0;
};

// Программа
class ProgramNode : public ASTNode {
public:
    std::vector<ASTPtr> declarations;
    ASTPtr block;

    void print(std::ostream& out, int indent = 0) const override;
};

// Объявление переменной
class VarDeclNode : public ASTNode {
public:
    std::vector<std::string> names;
    std::string type;

    void print(std::ostream& out, int indent = 0) const override;
};

// Объявление функции
class FuncDeclNode : public ASTNode {
public:
    std::string name;
    std::vector<std::string> params;
    std::string returnType;
    ASTPtr body;

    void print(std::ostream& out, int indent = 0) const override;
};

// Блок операторов
class BlockNode : public ASTNode {
public:
    std::vector<ASTPtr> statements;

    void print(std::ostream& out, int indent = 0) const override;
};

// Присваивание
class AssignmentNode : public ASTNode {
public:
    std::string name;
    ASTPtr expression;

    void print(std::ostream& out, int indent = 0) const override;
};

// Условный оператор
class IfNode : public ASTNode {
public:
    ASTPtr condition;
    ASTPtr thenBranch;
    ASTPtr elseBranch;

    void print(std::ostream& out, int indent = 0) const override;
};

// Оператор ввода
class ReadNode : public ASTNode {
public:
    std::vector<std::string> variables;

    void print(std::ostream& out, int indent = 0) const override;
};

// Оператор вывода
class WriteNode : public ASTNode {
public:
    std::vector<ASTPtr> expressions;

    void print(std::ostream& out, int indent = 0) const override;
};

// Бинарная операция
class BinaryOpNode : public ASTNode {
public:
    std::string op;
    ASTPtr left;
    ASTPtr right;

    BinaryOpNode(const std::string& op, ASTPtr left, ASTPtr right)
        : op(op), left(std::move(left)), right(std::move(right)) {}

    void print(std::ostream& out, int indent = 0) const override;
};

// Унарная операция
class UnaryOpNode : public ASTNode {
public:
    std::string op;
    ASTPtr operand;

    UnaryOpNode(const std::string& op, ASTPtr operand)
        : op(op), operand(std::move(operand)) {}

    void print(std::ostream& out, int indent = 0) const override;
};

// Вызов функции
class FuncCallNode : public ASTNode {
public:
    std::string name;
    std::vector<ASTPtr> arguments;

    void print(std::ostream& out, int indent = 0) const override;
};

// Числовая константа
class NumberNode : public ASTNode {
public:
    int value;

    NumberNode(int value) : value(value) {}

    void print(std::ostream& out, int indent = 0) const override;
};

// Идентификатор
class IdentifierNode : public ASTNode {
public:
    std::string name;

    IdentifierNode(const std::string& name) : name(name) {}

    void print(std::ostream& out, int indent = 0) const override;
};

#endif