#include <optional>

#include <parser/parser.hpp>
#include <ve/exceptions.hpp>
#include <parser/tokens.hpp>


[[nodiscard]] std::optional<HSharpParser::Token> HSharpParser::Parser::peek(const int offset) const {
    if (index + offset >= tokens.size())
        return {};
    else
        return tokens.at(index + offset);
}

HSharpParser::Token HSharpParser::Parser::consume() {
    return tokens.at(index++);
}

void HSharpParser::Parser::skip(int count) {
    index += count;
}

HSharpParser::Token HSharpParser::Parser::try_consume(HSharpParser::EToken type, int mode) {
    if (peek().has_value() && peek().value().type == type)
        return consume();
    else {
        std::string msg{};
        HSharpVE::EExceptionReason exc_type;
        if (!peek().has_value()){
            msg.append(std::format("Unexpected EOF"));
            exc_type = HSharpVE::EExceptionReason::EARLY_EOF;
        }
        else{
            msg.append(std::format( "Unexpected token type: {}, expected {}\n\tLine {}: {}\n",
                                    HSharpParser::toString(peek().value().type),
                                    HSharpParser::toString(type),
                                    0,
                                    0));
            exc_type = HSharpVE::EExceptionReason::UNEXPECTED_TOKEN;
        }
        error(HSharpVE::EExceptionSource::PARSER, exc_type, msg);
        exit(1);
    }
}

std::optional<HSharpParser::Token> HSharpParser::Parser::try_consume(HSharpParser::EToken type) {
    if (peek().has_value() && peek().value().type == type)
        return consume();
    else
        return {};
}
