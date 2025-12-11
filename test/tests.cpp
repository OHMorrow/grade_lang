#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "parser.h"

bool isValidProgramFile(const std::string& path, std::string& errorMsg) {
    std::ifstream in(path);
    if (!in) {
        return false;
    }
    std::stringstream ss;
    ss << in.rdbuf();
    Program* prog = nullptr;
    try {
        prog = parseProgram(ss.str());
        if (!prog) {
            delete prog;
            return false;
        }
        delete prog;
        return true;
    } catch (const std::exception& ex) {
        errorMsg = ex.what();
        delete prog;
        return false;
    }
}

#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::cout << "Test failed: " << #cond << " with error " << errorMsg << "\n"; \
            return false; \
        }  else { \
            std::cout << "Test passed: " << #cond << "\n"; \
        } \
    } while (0)

#define ASSERT_FALSE(cond) \
    do { \
        if (cond) { \
            std::cout << "Test failed: !" << #cond << "\n"; \
            return false; \
        } else { \
            std::cout << "Test passed: !" << #cond << "\n"; \
        } \
    } while (0)


bool runTests() {
    std::string errorMsg;
    ASSERT_TRUE(isValidProgramFile("test/examples/01_midterm_clamp.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/02_homework_final.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/03_drop_lowest.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/04_pass_fail.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/05_resolve_undef.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/01_midterm_clamp_bad.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/02_homework_final_bad.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/03_drop_lowest_bad.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/04_pass_fail_bad.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/05_resolve_undef_bad.txt", errorMsg));
    std::cout << "All tests passed." << std::endl; // make sure to flush output
    return true;
}

int main() {
    if (!runTests()) {
        return 1;
    }
    return 0;
}
