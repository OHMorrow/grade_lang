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
            // allow UDOUBLE starting with '.' when followed by a digit
            if (currentChar == '.' && pos + 1 < input.size() && isdigit(static_cast<unsigned char>(input[pos + 1]))) {
                state = 'd';
                tokenStart = pos;
                pos++; // consume the '.'
            }
            // NOTE: Removed '.' from start characters so a single '.' becomes UNKNOWN.
            else if (isalpha(static_cast<unsigned char>(currentChar)) || currentChar == '_' || currentChar == '-' || currentChar == '/') {
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
            // allow '.' inside identifiers so things like "special-id/1.2" are a single IDENTIFIER
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
                // remember the position of the leading '/'
                size_t slashPos = pos - 1;
                pos++; // move past '*'
                bool closed = false;
                while (pos + 1 < input.size()) {
                    if (input[pos] == '*' && input[pos + 1] == '/') {
                        pos += 2;
                        closed = true;
                        break;
                    }
                    pos++;
                }
                if (!closed) {
                    // unterminated comment: do not consume it — restore to the '/' so tokenizer can handle it
                    pos = slashPos;
                    break;
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