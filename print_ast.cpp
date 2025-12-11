#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "parser.h"
#include "eval.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: print_ast <source-file>\n";
        return 1;
    }
    const char* path = argv[1];
    std::ifstream in(path);
    if (!in) {
        std::cerr << "Failed to open file: " << path << "\n";
        return 1;
    }
    std::stringstream ss;
    ss << in.rdbuf();
    Program* prog = nullptr;
    try {
        prog = parseProgram(ss.str());
        if (!prog) {
            std::cerr << "Failed to parse program.\n";
            return 1;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Parse error: " << ex.what() << "\n";
        delete prog;
        return 1;
    }

    std::cout << "AST for " << path << ":\n";
    for (const auto& kv : prog->categories) {
        const std::string& name = kv.first;
        Expression* expr = kv.second;
        std::cout << "Category: " << name << "\n";
        if (expr) expr->printAST(std::cout, 2);
        else std::cout << "  <null expression>\n";
    }

    delete prog;
    return 0;
}
