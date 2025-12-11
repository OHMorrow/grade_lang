#pragma once
#include "tokenizer.h"
#include "eval.h"

// Parses a program from the given input.
Program* parseProgram(const std::string& input);
Program* parseProgram(const std::vector<Token>& tokens);
