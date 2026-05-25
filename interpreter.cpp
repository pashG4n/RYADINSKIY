// interpreter.cpp
#include "interpreter.h"
#include <limits>

Interpreter::Interpreter(SymbolTable& symbolTable)
    : symbols(symbolTable), returnFlag(false), returnValue(0) {}

void Interpreter::addError(const std::string& msg) {
    errors.push_back("Runtime error: " + msg);
}

bool Interpreter::interpret(ProgramNode* ast) {
    errors.clear();
    returnFlag = false;
    returnValue = 0;

    if (!ast) {
        addError("No AST to interpret");
        return false;
    }

    interpretProgram(ast);

    return errors.empty();
}

void Interpreter::interpretProgram(ProgramNode* node) {
    if (node->block) {
        interpretBlock(static_cast<BlockNode*>(node->block.get()));
    }
}

void Interpreter::interpretBlock(BlockNode* node) {
    for (const auto& stmt : node->statements) {
        interpretStatement(stmt.get());
        if (!errors.empty()) break;
    }
}

void Interpreter::interpretStatement(ASTNode* node) {
    if (auto assign = dynamic_cast<AssignmentNode*>(node)) {
        interpretAssignment(assign);
    }
    else if (auto ifNode = dynamic_cast<IfNode*>(node)) {
        interpretIf(ifNode);
    }
    else if (auto readNode = dynamic_cast<ReadNode*>(node)) {
        interpretRead(readNode);
    }
    else if (auto writeNode = dynamic_cast<WriteNode*>(node)) {
        interpretWrite(writeNode);
    }
    else if (auto block = dynamic_cast<BlockNode*>(node)) {
        interpretBlock(block);
    }
}

void Interpreter::interpretAssignment(AssignmentNode* node) {
    int value = evaluateExpression(node->expression.get());

    if (!symbols.isDeclared(node->name)) {
        addError("Variable '" + node->name + "' not declared");
        return;
    }

    symbols.setValue(node->name, value);
}

void Interpreter::interpretIf(IfNode* node) {
    int condition = evaluateExpression(node->condition.get());

    if (condition != 0) {
        interpretStatement(node->thenBranch.get());
    }
    else if (node->elseBranch) {
        interpretStatement(node->elseBranch.get());
    }
}

void Interpreter::interpretRead(ReadNode* node) {
    for (const auto& var : node->variables) {
        if (!symbols.isDeclared(var)) {
            addError("Variable '" + var + "' not declared in read");
            continue;
        }

        std::cout << "Enter value for " << var << ": ";
        int value;
        std::cin >> value;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            addError("Invalid integer input for " + var);
            continue;
        }

        symbols.setValue(var, value);
    }
}

void Interpreter::interpretWrite(WriteNode* node) {
    for (const auto& expr : node->expressions) {
        int value = evaluateExpression(expr.get());
        std::cout << value;

        if (expr != node->expressions.back()) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

int Interpreter::evaluateExpression(ASTNode* node) {
    if (auto num = dynamic_cast<NumberNode*>(node)) {
        return num->value;
    }
    else if (auto ident = dynamic_cast<IdentifierNode*>(node)) {
        std::string error;
        int value = symbols.getValue(ident->name, error);
        if (!error.empty()) {
            addError(error);
        }
        return value;
    }
    else if (auto binOp = dynamic_cast<BinaryOpNode*>(node)) {
        return evaluateBinaryOp(binOp);
    }
    else if (auto unOp = dynamic_cast<UnaryOpNode*>(node)) {
        return evaluateUnaryOp(unOp);
    }
    else if (auto call = dynamic_cast<FuncCallNode*>(node)) {
        return evaluateFuncCall(call);
    }

    addError("Unknown expression node type");
    return 0;
}

int Interpreter::evaluateBinaryOp(BinaryOpNode* node) {
    int left = evaluateExpression(node->left.get());
    int right = evaluateExpression(node->right.get());

    if (node->op == "+") return left + right;
    if (node->op == "-") return left - right;
    if (node->op == "*") return left * right;
    if (node->op == "/") {
        if (right == 0) {
            addError("Division by zero");
            return 0;
        }
        return left / right;
    }
    if (node->op == "<") return left < right;
    if (node->op == ">") return left > right;
    if (node->op == "<=") return left <= right;
    if (node->op == ">=") return left >= right;
    if (node->op == "=" || node->op == "==") return left == right;
    if (node->op == "<>" || node->op == "!=") return left != right;

    addError("Unknown binary operator: " + node->op);
    return 0;
}

int Interpreter::evaluateUnaryOp(UnaryOpNode* node) {
    int operand = evaluateExpression(node->operand.get());

    if (node->op == "-") return -operand;
    if (node->op == "+") return operand;

    addError("Unknown unary operator: " + node->op);
    return 0;
}

int Interpreter::evaluateFuncCall(FuncCallNode* node) {
    if (node->name == "abs") {
        if (node->arguments.empty()) {
            addError("abs() requires 1 argument");
            return 0;
        }
        int val = evaluateExpression(node->arguments[0].get());
        return val < 0 ? -val : val;
    }

    addError("Function '" + node->name + "' not implemented");
    return 0;
}

void Interpreter::printResults(std::ostream& out) const {
    if (!errors.empty()) {
        out << "\n=== INTERPRETATION ERRORS ===\n";
        for (const auto& err : errors) {
            out << "  " << err << "\n";
        }
    }

    symbols.printTable(out);
}