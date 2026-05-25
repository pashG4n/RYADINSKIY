// main.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "symbols.h"
#include "interpreter.h"

using namespace std;

string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Cannot open file: " + filename);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();

    // Convert to lowercase for case-insensitive parsing
    for (char& c : content) {
        c = tolower(c);
    }
    return content;
}

int main() {
    setlocale(LC_ALL, "Russian");

    const string inputFile = "input.txt";
    const string lexerOutput = "lexer_output.txt";
    const string parserOutput = "parser_output.txt";
    const string symbolOutput = "symbols_output.txt";

    cout << "=== Pascal Compiler with Interpreter ===\n\n";

    // Read source file
    string sourceCode;
    try {
        sourceCode = readFile(inputFile);
        cout << "Source code read from " << inputFile << "\n\n";
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        cerr << "Make sure " << inputFile << " exists in the same directory.\n";
        return 1;
    }

    // ============================================
    // 1. ËÅÊÑÈ×ÅÑÊÈÉ ÀÍÀËÈÇ
    // ============================================
    cout << "1. Lexical analysis...\n";
    Lexer lexer(sourceCode);

    vector<Token> tokenList;
    Token token;
    do {
        token = lexer.nextToken();
        tokenList.push_back(token);
    } while (token.type != TokenType::END_OF_FILE);

    ofstream lexOut(lexerOutput);
    if (!lexOut.is_open()) {
        cerr << "Cannot create " << lexerOutput << endl;
        return 1;
    }

    lexOut << "=== LEXER OUTPUT ===\n";
    lexOut << "Total tokens: " << tokenList.size() << "\n\n";
    for (size_t i = 0; i < tokenList.size(); i++) {
        const auto& t = tokenList[i];
        lexOut << "[" << i << "] Line " << t.line << ", Col " << t.column << ": ";
        lexOut << "Type=" << (int)t.type << ", Value='" << t.value << "'\n";
    }
    lexOut.close();

    if (!lexer.getErrors().empty()) {
        cout << "  Lexical errors found!\n";
        for (const auto& err : lexer.getErrors()) {
            cout << "    " << err << "\n";
        }
        return 1;
    }
    cout << "  No lexical errors. Tokens: " << tokenList.size() << "\n";

    // ============================================
    // 2. ÑÈÍÒÀÊÑÈ×ÅÑÊÈÉ È ÑÅÌÀÍÒÈ×ÅÑÊÈÉ ÀÍÀËÈÇ
    // ============================================
    cout << "2. Syntax and semantic analysis...\n";
    SymbolTable symbolTable;
    Parser parser(tokenList, symbolTable);

    bool parseSuccess = parser.parse();

    ofstream parseOut(parserOutput);
    parser.printAST(parseOut);
    parseOut.close();

    if (!parseSuccess) {
        cout << "  Errors found:\n";
        for (const auto& err : parser.getErrors()) {
            cout << "    " << err << "\n";
        }
        return 1;
    }
    cout << "  No syntax errors. AST built successfully.\n";

    // ============================================
    // 3. ÑÎÕÐÀÍÅÍÈÅ ÒÀÁËÈÖÛ ÑÈÌÂÎËÎÂ
    // ============================================
    ofstream symOut(symbolOutput);
    symbolTable.printTable(symOut);
    symOut.close();
    cout << "  Symbol table saved to " << symbolOutput << "\n";

    // ============================================
    // 4. ÈÍÒÅÐÏÐÅÒÀÖÈß
    // ============================================
    cout << "3. Interpretation...\n";
    Interpreter interpreter(symbolTable);

    bool interpretSuccess = interpreter.interpret(parser.getAST().get());

    if (!interpretSuccess) {
        cout << "  Interpretation errors found!\n";
        for (const auto& err : interpreter.getErrors()) {
            cout << "    " << err << "\n";
        }
    }
    else {
        cout << "  Interpretation completed successfully.\n";
    }

    // ============================================
    // 5. ÐÅÇÓËÜÒÀÒÛ
    // ============================================
    cout << "\n=== RESULTS ===\n";
    interpreter.printResults(cout);

    // Summary
    cout << "\n=== OUTPUT FILES ===\n";
    cout << "  - " << lexerOutput << " (lexical analysis)\n";
    cout << "  - " << parserOutput << " (AST)\n";
    cout << "  - " << symbolOutput << " (symbol table)\n";

    return 0;
}