#pragma once

#include <optional>
#include <string>
#include <vector>

enum class TokenType{
    TOK_EXIT
};

struct Token{
    TokenType ttype;
    std::optional<std::string> value {};
};

std::vector<Token> Tokenize(const std::string str);