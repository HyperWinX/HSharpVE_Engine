#include <vector>
#include <string>
#include <iostream>

#include <parser/parser.hpp>
#include <main/file.hpp>

using std::uint32_t;
using HSharpParser::Token;

std::vector<Token> HSharpParser::Tokenizer::tokenize() {
    std::string buf = {};
    std::vector<Token> tokens = {};

    while (peek().has_value()) {
        if (std::isalpha(peek().value())) {
            buf.push_back(consume());
            while (peek().has_value() && std::isalnum(peek().value()))
                buf.push_back(consume());

            if (buf == "exit") {
                tokens.push_back({.ttype = TokenType::TOK_EXIT});
                buf.clear();
            } else if (buf == "var") {
                tokens.push_back({.ttype = TokenType::TOK_VAR});
                buf.clear();
            } else if (buf == "print") {
                tokens.push_back({.ttype = TokenType::TOK_PRINT});
                buf.clear();
            } else if (buf == "input") {
                tokens.push_back({.ttype = TokenType::TOK_INPUT});
                buf.clear();
            } else {
                tokens.push_back({.ttype = TokenType::TOK_IDENT, .value = buf});
                buf.clear();
            }
        } else if (peek().value() == '"') {
            consume();
            while (peek().has_value() && peek().value() != '"')
                buf.push_back(consume());
            consume();
            tokens.push_back({.ttype = TokenType::TOK_STR_LIT, .value = buf});
            buf.clear();
        } else if (std::isdigit(peek().value())) {
            buf.push_back(consume());
            while (peek().has_value() && std::isdigit(peek().value()))
                buf.push_back(consume());
            tokens.push_back({.ttype = TokenType::TOK_INT_LIT, .value = buf});
            buf.clear();
        } else if (peek().value() == ';') {
            tokens.push_back({.ttype = TokenType::TOK_SEMICOLON});
            consume();
        } else if (peek().value() == '(') {
            tokens.push_back({.ttype = TokenType::TOK_PAREN_OPEN});
            consume();
        } else if (peek().value() == ')') {
            tokens.push_back({.ttype = TokenType::TOK_PAREN_CLOSE});
            consume();
        } else if (peek().value() == '=') {
            tokens.push_back({.ttype = TokenType::TOK_EQUALITY_SIGN});
            consume();
        } else if (peek().value() == '+') {
            tokens.push_back({.ttype = TokenType::TOK_PLUS});
            consume();
        } else if (peek().value() == '-') {
            tokens.push_back({.ttype = TokenType::TOK_MINUS});
            consume();
        } else if (peek().value() == '/') {
            tokens.push_back({.ttype = TokenType::TOK_FSLASH});
            consume();
        } else if (peek().value() == '*') {
            tokens.push_back({.ttype = TokenType::TOK_MUL_SIGN});
            consume();
        } else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '/') {
            consume();
            consume();
            while (peek().has_value() && peek().value() != '\n')
                consume();
        } else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '*') {
            consume();
            consume();
            while (peek().has_value()) {
                if (peek().value() == '*' && peek(1).has_value() && peek(1).value() == '/')
                    break;
                consume();
            }
            if (peek().has_value())
                consume();
            if (peek().has_value())
                consume();
        } else if (std::isspace(peek().value())) {
            consume();
        } else {
            std::cerr << "Syntax error!\n";
            exit(1);
        }
    }

    index = 0;
    return tokens;
}
