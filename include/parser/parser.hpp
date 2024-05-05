#pragma once

#include <optional>
#include <string>
#include <vector>

#include <main/file.hpp>

enum class TokenType{
    TOK_EXIT,
    TOK_INT_LIT,
    TOK_SEMICOLON
};

struct Token{
    TokenType ttype;
    std::optional<std::string> value {};
};

struct NodeExpression{
    Token int_lit;
};
struct NodeExit{
    NodeExpression expr;
};

class Tokenizer{
    private:
    File& file;
    std::uint32_t index = 0;

    [[nodiscard]] std::optional<char> peek(int ahead = 1) const;
    char consume();
    public:
    Tokenizer(File& file) : file(file){
        
    }
    std::vector<Token> tokenize();
};

class Parser{
    private:
    const std::vector<Token> tokens;
    std::size_t index = 0;

    [[nodiscard]] std::optional<Token> peek(int ahead = 1) const;
    Token consume();
    std::optional<NodeExpression> parse_expression();
    public:
    Parser(std::vector<Token> tokens) : tokens(std::move(tokens)){
        
    }
    std::optional<NodeExit> parse();
};