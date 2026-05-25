// symbols.cpp
#include "symbols.h"

SymbolTable::SymbolTable() {
    scopeStack.push_back("global");
}

void SymbolTable::enterScope(const std::string& scopeName) {
    scopeStack.push_back(scopeName);
}

void SymbolTable::exitScope() {
    if (scopeStack.size() > 1) {
        scopeStack.pop_back();
    }
}

std::string SymbolTable::currentScope() const {
    return scopeStack.back();
}

Symbol* SymbolTable::lookup(const std::string& name) {
    // »щем с конца (сначала в текущей области видимости)
    for (int i = (int)scopeStack.size() - 1; i >= 0; i--) {
        std::string fullName = scopeStack[i] + "::" + name;
        auto it = symbols.find(fullName);
        if (it != symbols.end()) {
            return &it->second;
        }
    }
    return nullptr;
}

const Symbol* SymbolTable::lookup(const std::string& name) const {
    for (int i = (int)scopeStack.size() - 1; i >= 0; i--) {
        std::string fullName = scopeStack[i] + "::" + name;
        auto it = symbols.find(fullName);
        if (it != symbols.end()) {
            return &it->second;
        }
    }
    return nullptr;
}

bool SymbolTable::declareVariable(const std::string& name, SymbolType type, std::string& error) {
    std::string fullName = currentScope() + "::" + name;

    // ѕровер€ем, не объ€влена ли уже в текущей области
    auto it = symbols.find(fullName);
    if (it != symbols.end()) {
        error = "Variable '" + name + "' already declared in scope '" + currentScope() + "'";
        return false;
    }

    Symbol sym;
    sym.name = name;
    sym.type = type;
    sym.declared = true;
    sym.initialized = false;
    sym.value = 0;
    sym.arraySize = 0;

    symbols[fullName] = sym;
    return true;
}

bool SymbolTable::declareArray(const std::string& name, int size, std::string& error) {
    if (size <= 0) {
        error = "Array size must be positive";
        return false;
    }

    std::string fullName = currentScope() + "::" + name;

    auto it = symbols.find(fullName);
    if (it != symbols.end()) {
        error = "Array '" + name + "' already declared in scope '" + currentScope() + "'";
        return false;
    }

    Symbol sym;
    sym.name = name;
    sym.type = SymbolType::INTEGER_ARRAY;
    sym.declared = true;
    sym.initialized = true;
    sym.arraySize = size;
    sym.arrayValues.resize(size, 0);
    sym.arrayInitialized.resize(size, false);

    symbols[fullName] = sym;
    return true;
}

bool SymbolTable::isDeclared(const std::string& name) const {
    return lookup(name) != nullptr;
}

bool SymbolTable::isInitialized(const std::string& name) const {
    const Symbol* sym = lookup(name);
    if (!sym) return false;
    return sym->initialized;
}

SymbolType SymbolTable::getType(const std::string& name) const {
    const Symbol* sym = lookup(name);
    if (!sym) return SymbolType::INTEGER;
    return sym->type;
}

void SymbolTable::markInitialized(const std::string& name) {
    Symbol* sym = lookup(name);
    if (sym) {
        sym->initialized = true;
    }
}

void SymbolTable::setValue(const std::string& name, int value) {
    Symbol* sym = lookup(name);
    if (sym && sym->type == SymbolType::INTEGER) {
        sym->value = value;
        sym->initialized = true;
    }
}

int SymbolTable::getValue(const std::string& name, std::string& error) const {
    const Symbol* sym = lookup(name);
    if (!sym) {
        error = "Variable '" + name + "' not declared";
        return 0;
    }
    if (!sym->initialized) {
        error = "Variable '" + name + "' not initialized";
        return 0;
    }
    if (sym->type != SymbolType::INTEGER) {
        error = "Variable '" + name + "' is not an integer variable";
        return 0;
    }
    return sym->value;
}

void SymbolTable::setArrayValue(const std::string& name, int index, int value, std::string& error) {
    Symbol* sym = lookup(name);
    if (!sym) {
        error = "Array '" + name + "' not declared";
        return;
    }
    if (sym->type != SymbolType::INTEGER_ARRAY) {
        error = "'" + name + "' is not an array";
        return;
    }
    if (index < 0 || index >= sym->arraySize) {
        error = "Array index " + std::to_string(index) + " out of bounds (0.." + std::to_string(sym->arraySize - 1) + ")";
        return;
    }
    sym->arrayValues[index] = value;
    sym->arrayInitialized[index] = true;
}

int SymbolTable::getArrayValue(const std::string& name, int index, std::string& error) const {
    const Symbol* sym = lookup(name);
    if (!sym) {
        error = "Array '" + name + "' not declared";
        return 0;
    }
    if (sym->type != SymbolType::INTEGER_ARRAY) {
        error = "'" + name + "' is not an array";
        return 0;
    }
    if (index < 0 || index >= sym->arraySize) {
        error = "Array index " + std::to_string(index) + " out of bounds (0.." + std::to_string(sym->arraySize - 1) + ")";
        return 0;
    }
    if (!sym->arrayInitialized[index]) {
        error = "Array element '" + name + "[" + std::to_string(index) + "]' not initialized";
        return 0;
    }
    return sym->arrayValues[index];
}

int SymbolTable::getArraySize(const std::string& name) const {
    const Symbol* sym = lookup(name);
    if (!sym || sym->type != SymbolType::INTEGER_ARRAY) return 0;
    return sym->arraySize;
}

void SymbolTable::printTable(std::ostream& out) const {
    out << "\n=== SYMBOL TABLE ===\n";
    out << std::left
        << std::setw(20) << "Name"
        << std::setw(15) << "Type"
        << std::setw(15) << "Initialized"
        << std::setw(20) << "Value/Array"
        << "\n";
    out << std::string(70, '-') << "\n";

    for (const auto& pair : symbols) {
        const Symbol& sym = pair.second;

        std::string typeStr;
        if (sym.type == SymbolType::INTEGER) typeStr = "INTEGER";
        else typeStr = "INTEGER_ARRAY";

        std::string initStr = sym.initialized ? "Yes" : "No";

        std::string valueStr;
        if (sym.type == SymbolType::INTEGER) {
            valueStr = sym.initialized ? std::to_string(sym.value) : "?";
        }
        else {
            valueStr = "[";
            for (int i = 0; i < sym.arraySize; i++) {
                if (i > 0) valueStr += ", ";
                if (sym.arrayInitialized[i]) {
                    valueStr += std::to_string(sym.arrayValues[i]);
                }
                else {
                    valueStr += "?";
                }
            }
            valueStr += "]";
        }

        out << std::left
            << std::setw(20) << sym.name
            << std::setw(15) << typeStr
            << std::setw(15) << initStr
            << std::setw(20) << valueStr
            << "\n";
    }
    out << "====================\n";
}