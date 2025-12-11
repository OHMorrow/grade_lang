#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cmath>
#include <algorithm> // for trim helpers
#include "eval.h"
#include "parser.h"
#include "operations.h"

static std::string fmtPercent(double v) {
    if (std::isnan(v)) return std::string("undef");
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << (v * 100.0) << "%";
    return ss.str();
}

static void printValueAsPercent(Value* v) {
    if (!v) {
        std::cout << "<null>\n";
        return;
    }
    switch (v->getType()) {
        case DataType::TYPE_GRADE: {
            double g = static_cast<GradeValue*>(v)->getVal();
            std::cout << fmtPercent(g) << "\n";
            break;
        }
        case DataType::TYPE_INTEGER: {
            unsigned long long iv = static_cast<IntegerValue*>(v)->getVal();
            // treat integer as scalar grade value
            std::cout << fmtPercent(static_cast<double>(iv)) << "\n";
            break;
        }
        case DataType::TYPE_LIST: {
            ListValue* lv = static_cast<ListValue*>(v);
            std::cout << "[";
            for (size_t i = 0; i < lv->size(); ++i) {
                if (i) std::cout << ", ";
                double val = lv->getValueAt(i);
                double wt  = lv->getWeightAt(i);
                std::cout << fmtPercent(val);
                if (wt != 1.0) {
                    std::ostringstream wss;
                    wss << std::fixed << std::setprecision(2) << wt;
                    std::cout << ":" << wss.str();
                }
            }
            std::cout << "]\n";
            break;
        }
        default:
            std::cout << "<unknown>\n";
    }
}

static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Helper to load a program file into the context; returns true on success.
static bool loadProgramFromFile(Context& ctx, const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        std::cerr << "Failed to open file: " << path << "\n";
        return false;
    }
    std::stringstream ss;
    ss << in.rdbuf();
    Program* prog = nullptr;
    try {
        prog = parseProgram(ss.str());
        if (!prog) {
            std::cerr << "Failed to parse program file: " << path << "\n";
            delete prog;
            return false;
        }
        ctx.dataProviders.push_back(prog);
        std::cout << "Loaded program: " << path << "\n";
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Parse error in " << path << ": " << ex.what() << "\n";
        delete prog;
        return false;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << (argc > 0 ? argv[0] : "repl") << " <program-file> [additional-program-file ...]\n";
        return 1;
    }

    Context ctx;

    // register built-in operations provider
    OperationProvider* ops = createProvider();
    if (ops) ctx.operationProviders.push_back(ops);

    // Load all provided program files (argv[1] .. argv[argc-1])
    for (int i = 1; i < argc; ++i) {
        loadProgramFromFile(ctx, argv[i]);
    }

    std::cout << "GradeLang REPL. Outputs formatted as percentages. Type 'quit' or 'q' to exit.\n";
    std::string line;
    while (true) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;
        if (line == "quit" || line == "q") break;

        std::string key = trim(line);
        // support include/i command to load additional files at runtime:
        //   include <path>   or  i <path>
        {
            std::istringstream iss(key);
            std::string cmd;
            iss >> cmd;
            if (cmd == "include" || cmd == "i") {
                std::string rest;
                std::getline(iss, rest);
                rest = trim(rest);
                if (rest.empty()) {
                    std::cerr << "Usage: include <file-path>\n";
                } else {
                    loadProgramFromFile(ctx, rest);
                }
                continue;
            }
        }
        if (key.rfind("get ", 0) == 0) key = trim(key.substr(4));

        try {
            Value* v = ctx.getCategoryValue(key);
            printValueAsPercent(v);
        } catch (const std::exception& ex) {
            std::cerr << "Error: " << ex.what() << "\n";
        }
    }

    // cleanup
    for (OperationProvider* p : ctx.operationProviders) delete p;
    ctx.operationProviders.clear();
    for (DataProvider* dp : ctx.dataProviders) delete dp;
    ctx.dataProviders.clear();

    return 0;
}