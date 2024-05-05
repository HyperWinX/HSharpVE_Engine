#include <vector>
#include <string>
#include <iostream>

#include <parser/parser.hpp>
#include <main/file.hpp>

using std::uint32_t;

std::vector<Token> Tokenizer::tokenize(){
    std::string buf = {};
    std::vector<Token> tokens = {};

    while (peek().has_value()){
        if (std::isalpha(peek().value())){
            buf.push_back(consume());
            while (peek().has_value() && std::isalnum(peek().value()))
                buf.push_back(consume());

            if (buf == "exit"){
                tokens.push_back({.ttype = TokenType::TOK_EXIT});
                buf.clear();
                continue;
            } else {
                std::cerr << "Syntax error!\n";
                exit(1);
            }
            std::puts("Reached end of if statement");
        } else if (std::isdigit(peek().value())){
            buf.push_back(consume());
            while (peek().has_value() && std::isdigit(peek().value()))
                buf.push_back(consume());
            tokens.push_back({.ttype = TokenType::TOK_INT_LIT, .value = buf});
            buf.clear();
            continue;
        } else if (peek().value() == ';'){
            tokens.push_back({.ttype = TokenType::TOK_SEMICOLON});
            consume();
            continue;
        } else if (std::isspace(peek().value())){
            consume();
            continue;
        } else {
            std::cerr << "Syntax error!\n";
            exit(1);
        }
    }

    index = 0;
    return tokens;
}