#include "parser.h"
#include "tokenizer.h"
#include <stdexcept>
#include <string>
#include <cstdlib>

// forward declarations for helper token functions
static Token peekToken(const std::vector<Token>& tokens, size_t idx);
static Token consumeToken(const std::vector<Token>& tokens, size_t& idx);
static void expectToken(const std::vector<Token>& tokens, size_t& idx, TokenT expected);

// forward declarations
static Expression* parseExpr(const std::vector<Token>& tokens, size_t& idx);
static ListExpr* parseList(const std::vector<Token>& tokens, size_t& idx);

// Parses a program from the given tokens.
Program* parseProgram(const std::vector<Token>& tokens) {
    size_t idx = 0;
    Program* program = new Program();
    while (true) {
        Token t = peekToken(tokens, idx);
        if (t.type == TokenT::END_OF_FILE) break;
        if (t.type == TokenT::IDENTIFIER) {
            // category: IDENTIFIER ':' expr
            Token nameTok = consumeToken(tokens, idx);
            Token colon = peekToken(tokens, idx);
            if (colon.type != TokenT::COLON) {
                throw std::runtime_error("Parse error: expected ':' after category name at position " + std::to_string(colon.position));
            }
            // consume colon
            consumeToken(tokens, idx);
            Expression* expr = parseExpr(tokens, idx);
            program->categories[nameTok.text] = expr;
            continue;
        }
        // skip unknown or other tokens until EOF (or error)
        throw std::runtime_error("Parse error: unexpected token at position " + std::to_string(t.position));
    }
    return program;
}

// Parses a program from the given input.
Program* parseProgram(const std::string& input) {
    std::vector<Token> toks = tokenize(input);
    return parseProgram(toks);
}

static Token peekToken(const std::vector<Token>& tokens, size_t idx) {
    if (idx < tokens.size()) return tokens[idx];
    return Token(TokenT::END_OF_FILE, "", tokens.empty() ? 0 : tokens.back().position + 1);
}
static Token consumeToken(const std::vector<Token>& tokens, size_t& idx) {
    Token t = peekToken(tokens, idx);
    if (idx < tokens.size()) ++idx;
    return t;
}
static void expectToken(const std::vector<Token>& tokens, size_t& idx, TokenT expected) {
    Token t = peekToken(tokens, idx);
    if (t.type != expected) {
        throw std::runtime_error("Parse error: expected token at position " + std::to_string(t.position));
    }
    ++idx;
}

// parse a list_item: expr ( ':' expr )?
static ListElement* parseListItem(const std::vector<Token>& tokens, size_t& idx) {
    Expression* valueExpr = parseExpr(tokens, idx);
    Expression* weightExpr = nullptr;
    Token t = peekToken(tokens, idx);
    if (t.type == TokenT::COLON) {
        // consume colon
        consumeToken(tokens, idx);
        weightExpr = parseExpr(tokens, idx);
    }
    return new ListElement(valueExpr, weightExpr);
}

static ListExpr* parseList(const std::vector<Token>& tokens, size_t& idx) {
    // assume '{' already consumed
    std::vector<ListElement*> elems;
    Token t = peekToken(tokens, idx);
    while (t.type != TokenT::RBRACE) {
        if (t.type == TokenT::END_OF_FILE) {
            throw std::runtime_error("Parse error: unexpected end of file inside list at position " + std::to_string(t.position));
        }
        elems.push_back(parseListItem(tokens, idx));
        t = peekToken(tokens, idx);
    }
    // consume '}'
    expectToken(tokens, idx, TokenT::RBRACE);
    return new ListExpr(elems);
}

static Expression* parseOperation(const std::vector<Token>& tokens, size_t& idx, const std::string& opName) {
    // '(' already consumed by caller
    std::vector<Expression*> args;
    Token t = peekToken(tokens, idx);
    while (t.type != TokenT::RPAREN) {
        if (t.type == TokenT::END_OF_FILE) {
            throw std::runtime_error("Parse error: unexpected end of file in operation '" + opName + "' at position " + std::to_string(t.position));
        }
        args.push_back(parseExpr(tokens, idx));
        t = peekToken(tokens, idx);
    }
    // consume ')'
    expectToken(tokens, idx, TokenT::RPAREN);
    return new OperationExpr(opName, args);
}

static Expression* parseExpr(const std::vector<Token>& tokens, size_t& idx) {
    Token t = peekToken(tokens, idx);
    if (t.type == TokenT::PERCENT) {
        consumeToken(tokens, idx);
        // text includes '%', strip and parse
        std::string num = t.text;
        if (!num.empty() && num.back() == '%') num.pop_back();
        double value = std::strtod(num.c_str(), nullptr) / 100.0;
        return new ConstantExpr(new GradeValue(value));
    }
    if (t.type == TokenT::UDOUBLE) {
        consumeToken(tokens, idx);
        double d = std::strtod(t.text.c_str(), nullptr);
        return new ConstantExpr(new GradeValue(d));
    }
    if (t.type == TokenT::INTEGER) {
        consumeToken(tokens, idx);
        unsigned long long v = std::stoull(t.text);
        return new ConstantExpr(new IntegerValue(v));
    }
    if (t.type == TokenT::IDENTIFIER) {
        // could be operation or category ref
        consumeToken(tokens, idx);
        std::string name = t.text;
        Token next = peekToken(tokens, idx);
        if (next.type == TokenT::LPAREN) {
            // consume '('
            consumeToken(tokens, idx);
            return parseOperation(tokens, idx, name);
        } else {
            return new CategoryRefExpr(name);
        }
    }
    if (t.type == TokenT::LBRACE) {
        consumeToken(tokens, idx);
        return parseList(tokens, idx);
    }

    throw std::runtime_error("Parse error: unexpected token at position " + std::to_string(t.position));
}