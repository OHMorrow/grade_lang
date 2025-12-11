#include "tokenizer.h"
#include <cctype>

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> out;
    size_t pos = 0;
    size_t tokenStart = 0;
    char state = ' '; // ' ' = default, 'i' = identifier, 'n' = number before dot, 'd' = number after dot
    while (pos <= input.size()) {
        char currentChar = (pos < input.size()) ? input[pos] : '\0';
        if (state == ' ') {
            // skip whitespace and comments using helper
            pos = _consumeWhitespace(input, pos);
            if (pos >= input.size()) {
                out.emplace_back(TokenT::END_OF_FILE, "", pos);
                break;
            }
            currentChar = input[pos];
            if (isalpha(static_cast<unsigned char>(currentChar)) || currentChar == '_' || currentChar == '-' || currentChar == '/' || currentChar == '.') {
                state = 'i';
                tokenStart = pos;
                pos++;
            } else if (isdigit(static_cast<unsigned char>(currentChar))) {
                state = 'n';
                tokenStart = pos;
                pos++;
            } else if (currentChar == ':') {
                out.emplace_back(TokenT::COLON, ":", pos);
                pos++;
            } else if (currentChar == '(') {
                out.emplace_back(TokenT::LPAREN, "(", pos);
                pos++;
            } else if (currentChar == ')') {
                out.emplace_back(TokenT::RPAREN, ")", pos);
                pos++;
            } else if (currentChar == '{') {
                out.emplace_back(TokenT::LBRACE, "{", pos);
                pos++;
            } else if (currentChar == '}') {
                out.emplace_back(TokenT::RBRACE, "}", pos);
                pos++;
            } else {
                out.emplace_back(TokenT::UNKNOWN, std::string(1, currentChar), pos);
                pos++;
            }
        } else if (state == 'i') {
            if (isalnum(static_cast<unsigned char>(currentChar)) || currentChar == '_' || currentChar == '-' || currentChar == '/' || currentChar == '.') {
                pos++;
            } else {
                out.emplace_back(TokenT::IDENTIFIER, input.substr(tokenStart, pos - tokenStart), tokenStart);
                state = ' ';
            }
        } else if (state == 'n') {
            if (isdigit(static_cast<unsigned char>(currentChar))) {
                pos++;
            } else if (currentChar == '.') {
                state = 'd';
                pos++;
            } else if (currentChar == '%') {
                out.emplace_back(TokenT::PERCENT, input.substr(tokenStart, pos - tokenStart) + "%", tokenStart);
                pos++;
                state = ' ';
            } else {
                out.emplace_back(TokenT::INTEGER, input.substr(tokenStart, pos - tokenStart), tokenStart);
                state = ' ';
            }
        } else if (state == 'd') {
            if (isdigit(static_cast<unsigned char>(currentChar))) {
                pos++;
            } else if (currentChar == '%') {
                out.emplace_back(TokenT::PERCENT, input.substr(tokenStart, pos - tokenStart) + "%", tokenStart);
                pos++;
                state = ' ';
            } else {
                out.emplace_back(TokenT::UDOUBLE, input.substr(tokenStart, pos - tokenStart), tokenStart);
                state = ' ';
            }
        }
    }
    return out;
}

// comments are c-style: // to end of line, /* to */
size_t _consumeWhitespace(const std::string& input, size_t startPos) {
    size_t pos = startPos;
    while (pos < input.size()) {
        char currentChar = input[pos];
        if (isspace(static_cast<unsigned char>(currentChar))) {
            pos++;
        } else if (currentChar == '/') {
            pos++;
            if (pos < input.size() && input[pos] == '/') {
                // single-line comment
                while (pos < input.size() && input[pos] != '\n') {
                    pos++;
                }
            } else if (pos < input.size() && input[pos] == '*') {
                // multi-line comment
                pos++;
                while (pos + 1 < input.size()) {
                    if (input[pos] == '*' && input[pos + 1] == '/') {
                        pos += 2;
                        break;
                    }
                    pos++;
                }
            } else {
                // not a comment, step back
                pos--;
                break;
            }
        } else {
            break;
        }
    }
    return pos;
}