#include <optional>

#include <parser/parser.hpp>
#include <ve/exceptions.hpp>
#include <parser/tokens.hpp>

[[nodiscard]] std::optional<char> HSharpParser::Tokenizer::peek(const int offset) const {
    if (index + offset >= file.contents.value().size())
        return {};
    else
        return file.contents.value().at(index + offset);
}

char HSharpParser::Tokenizer::consume() {
    return file.contents.value().at(index++);
}
void HSharpParser::Tokenizer::skip(int count) {
    index += count;
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

void HSharpParser::Parser::skip(int count) {
    index += count;
}

HSharpParser::Token HSharpParser::Parser::try_consume(TokenType type, int mode) {
    if (peek().has_value() && peek().value().ttype == type)
        return consume();
    else {
        std::string msg{};
        HSharpVE::ExceptionType exc_type;
        if (!peek().has_value()){
            msg.append(std::format("Unexpected EOF"));
            exc_type = HSharpVE::EndOfFile;
        }
        else{
            msg.append(std::format( "Unexpected token type: {}, expected {}\n\tLine {}: {}\n",
                                    HSharpParser::ToString(peek().value().ttype),
                                    HSharpParser::ToString(type),
                                    peek().value().line,
                                    lines[peek().value().line - 1].c_str()));
            exc_type = HSharpVE::UnexpectedToken;
        }
        throwFatalException(HSharpVE::ExceptionSource::Parser,
                            exc_type,
                            msg);
        exit(1);
    }
}

std::optional<HSharpParser::Token> HSharpParser::Parser::try_consume(TokenType type) {
    if (peek().has_value() && peek().value().ttype == type)
        return consume();
    else
        return {};
}
