// ast.cpp
#include "ast.h"
#include <iostream>

void ProgramNode::print(std::ostream& out, int indent) const {
    out << std::string(indent, ' ') << "Program\n";
    for (const auto& decl : declarations) {
        decl->print(out, indent + 2);
    }
    if (block) block->print(out, indent + 2);
}

void VarDeclNode::print(std::ostream& out, int indent) const {
    out << std::string(indent, ' ') << "VarDecl: ";
    for (size_t i = 0; i < names.size(); i++) {
        if (i > 0) out << ", ";
        out << names[i];
    }
    out << " : " << type << "\n";
}

void FuncDeclNode::print(std::ostream& out, int indent) const {
    out << std::string(indent, ' ') << "Function: " << name << "(";
    for (size_t i = 0; i < params.size(); i++) {
        if (i > 0) out << ", ";
        out << params[i];
    }
    out << ") : " << returnType << "\n";
    if (body) body->print(out, indent + 2);
}

void BlockNode::print(std::ostream& out, int indent) const {
    out << std::string(indent, ' ') << "Block\n";
    for (const auto& stmt : statements) {
        stmt->print(out, indent + 2);
    }
}

void AssignmentNode::print(std::ostream& out, int indent) const {
    out << std::string(indent, ' ') << "Assign: " << name << " := ";
    if (expression) expression->print(out, 0);
    out << "\n";
}

void IfNode::print(std::ostream& out, int indent) const {
    out << std::string(indent, ' ') << "If\n";
    out << std::string(indent + 2, ' ') << "Condition: ";
    if (condition) condition->print(out, 0);
    out << "\n";
    out << std::string(indent + 2, ' ') << "Then:\n";
    if (thenBranch) thenBranch->print(out, indent + 4);
    if (elseBranch) {
        out << std::string(indent + 2, ' ') << "Else:\n";
        elseBranch->print(out, indent + 4);
    }
}

void ReadNode::print(std::ostream& out, int indent) const {
    out << std::string(indent, ' ') << "Read: ";
    for (size_t i = 0; i < variables.size(); i++) {
        if (i > 0) out << ", ";
        out << variables[i];
    }
    out << "\n";
}

void WriteNode::print(std::ostream& out, int indent) const {
    out << std::string(indent, ' ') << "Write: ";
    for (const auto& expr : expressions) {
        expr->print(out, 0);
        out << " ";
    }
    out << "\n";
}

void BinaryOpNode::print(std::ostream& out, int indent) const {
    out << "(";
    left->print(out, 0);
    out << " " << op << " ";
    right->print(out, 0);
    out << ")";
}

void UnaryOpNode::print(std::ostream& out, int indent) const {
    out << "(" << op;
    operand->print(out, 0);
    out << ")";
}

void FuncCallNode::print(std::ostream& out, int indent) const {
    out << name << "(";
    for (size_t i = 0; i < arguments.size(); i++) {
        if (i > 0) out << ", ";
        arguments[i]->print(out, 0);
    }
    out << ")";
}

void NumberNode::print(std::ostream& out, int indent) const {
    out << value;
}

void IdentifierNode::print(std::ostream& out, int indent) const {
    out << name;
}