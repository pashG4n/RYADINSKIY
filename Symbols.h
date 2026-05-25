// symbols.h
#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <iomanip>

enum class SymbolType {
    INTEGER,      // простая целая переменная
    INTEGER_ARRAY // массив целых чисел
};

struct Symbol {
    std::string name;
    SymbolType type;
    bool declared;
    bool initialized;
    int value;                    // для INTEGER
    std::vector<int> arrayValues; // для INTEGER_ARRAY
    std::vector<bool> arrayInitialized;
    int arraySize;                // размер массива (0 для не-массива)
};

class SymbolTable {
public:
    SymbolTable();

    // Управление областями видимости
    void enterScope(const std::string& scopeName);
    void exitScope();
    std::string currentScope() const;

    // Объявление символов
    bool declareVariable(const std::string& name, SymbolType type, std::string& error);
    bool declareArray(const std::string& name, int size, std::string& error);

    // Проверки
    bool isDeclared(const std::string& name) const;
    bool isInitialized(const std::string& name) const;
    SymbolType getType(const std::string& name) const;

    // Инициализация
    void markInitialized(const std::string& name);
    void setValue(const std::string& name, int value);
    int getValue(const std::string& name, std::string& error) const;

    // Работа с массивами
    void setArrayValue(const std::string& name, int index, int value, std::string& error);
    int getArrayValue(const std::string& name, int index, std::string& error) const;
    int getArraySize(const std::string& name) const;

    // Вывод
    void printTable(std::ostream& out) const;

    // Получение всех символов для интерпретатора
    const std::unordered_map<std::string, Symbol>& getAllSymbols() const { return symbols; }

private:
    std::vector<std::string> scopeStack;
    std::unordered_map<std::string, Symbol> symbols;

    Symbol* lookup(const std::string& name);
    const Symbol* lookup(const std::string& name) const;
};

#endif