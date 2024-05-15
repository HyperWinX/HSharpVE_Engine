#include <optional>

#include <parser/parser.hpp>

[[nodiscard]] std::optional<char> HSharpParser::Tokenizer::peek(const int offset) const {
    if (index + offset >= file.contents.value().size())
        return {};
    else
        return file.contents.value().at(index + offset);
}

char HSharpParser::Tokenizer::consume() {
    return file.contents.value().at(index++);
}

[[nodiscard]] std::optional<HSharpParser::Token> HSharpParser::Parser::peek(const int offset) const {
    if (index + offset >= tokens.size())
        return {};
    else
        return tokens.at(index + offset);
}

HSharpParser::Token HSharpParser::Parser::consume() {
    return tokens.at(index++);
}

HSharpParser::Token HSharpParser::Parser::try_consume(TokenType type, const char* err_msg) {
    if (peek().has_value() && peek().value().ttype == type)
        return consume();
    else {
        std::cerr << err_msg << std::endl;
        exit(1);
    }
}

std::optional<HSharpParser::Token> HSharpParser::Parser::try_consume(TokenType type) {
    if (peek().has_value() && peek().value().ttype == type)
        return consume();
    else
        return {};
}
