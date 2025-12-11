#pragma once
#include <string>
#include <vector>
#include <cstddef>

enum class TokenT {
    END_OF_FILE,
    INTEGER,
    IDENTIFIER, // includes underscores, hyphens, slashes, dots
    UDOUBLE,
    PERCENT, // an integer or double followed by a '%' character
    COLON,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    UNKNOWN
};

struct Token {
    TokenT type;
    std::string text;
    size_t position;
    
    Token(TokenT t, const std::string& txt, size_t pos)
        : type(t), text(txt), position(pos) {}
};

// Tokenize input into a stream of Token objects.
std::vector<Token> tokenize(const std::string& input);

// comments are c-style: // to end of line, /* to */
size_t _consumeWhitespace(const std::string& input, size_t startPos);
