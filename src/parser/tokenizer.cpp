#include <cctype>
#include <cstddef>
#include <cstdint>
#include <format>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

#include <parser/tokens.hpp>
#include <parser/tokenizer.hpp>
#include <ve/exceptions.hpp>

using namespace HSharpParser;


std::vector<Token> Tokenizer::tokenize(std::istream& is) {
    state_.num = 0;
    state_.line.clear();
    std::vector<Token> tokens;

    while (getline(is, state_.line)) {
        auto position = state_.line.begin();
        while (position != state_.line.end()) {
            EBet guess = bet(position, state_.line.end());
            Token token = read(position, state_.line.end(), guess);
            // post-processing?
            // probably work for parser
            ++state_.num;
        }
    }

    return tokens;
}

Tokenizer::EBet Tokenizer::bet(std::string::iterator position, std::string::iterator end) {
    char first = peek(position, end);
    if (std::isalpha(first)) {
        return EBet::IDENTIFIER;
    }
    if (std::isdigit(first) || first == '\"') {
        return EBet::LITERAL;
    }
    return EBet::SPECIAL;
}

Token Tokenizer::read(std::string::iterator& position, std::string::iterator end, EBet bet) {
    switch (bet) {
        case EBet::IDENTIFIER: return readLiteral(position, end);
        case EBet::LITERAL: return readLiteral(position, end);
        case EBet::SPECIAL: return readSpecial(position, end);
        default: HSHARP_NOT_IMPLEMENTED(HSharpVE::EExceptionSource::TOKENIZER, "enum value out of range");
    }
}

Token Tokenizer::readIdentifier(std::string::iterator& position, std::string::iterator end) {
    std::string identifier;

    if (char head = peek(position, end); isIdentifierChar(head, true)) {
        identifier.push_back(head);
    }

    while (position != end && isIdentifierChar(*position)) {
        identifier.push_back(*(position++));
    }

    if (identifier.empty()) {
        fallback("expected identifier", LineSpecialization(state_.line, state_.num, position - state_.line.begin()));
    }

    return makeToken(EToken::IDENTIFIER, identifier);
}

char Tokenizer::peek(std::string::iterator position, std::string::iterator end) {
    if (position != end) {
        return *position;
    }
    fallback("unexpected EOL", LineSpecialization(state_.line, state_.num, position - state_.line.begin()));
}

Token Tokenizer::readLiteral(std::string::iterator& position, std::string::iterator end) {
    std::string literal;
    EToken type;

    if (peek(position, end) == '\"') {
        // read string
        type = EToken::STRING_LITERAL;
        while (position != end && *position != '\"') {
            literal.push_back(*(position++));
        }
        literal.push_back(*(position++));
    } else {
        type = EToken::INTEGER_LITERAL;
        while (position != end && std::isdigit(*position)) {
            literal.push_back(*(position++));
        }
    }

    if (literal.empty()) {
        fallback("expected literal", LineSpecialization(state_.line, state_.num, position - state_.line.begin()));
    }

    return makeToken(type, literal);
}

Token Tokenizer::readSpecial(std::string::iterator& position, std::string::iterator end) {
    if (position != end) {
        auto type = typings.find(*position);
        if (type == typings.end()) {
            fallback(std::format("{}", *position));
        } else {
            std::string payload; payload.push_back(*position);
            return makeToken(type->second, payload);
        }
    } else {
        fallback("unexpected EOL", LineSpecialization(state_.line, state_.num, position - state_.line.begin()));
    }
}

std::string HSharpParser::toString(EToken token) {
    switch (token) {
        HSHARP_ENUM_CASE(HSharpParser::EToken, BUILTIN_FUNCTION);
        HSHARP_ENUM_CASE(HSharpParser::EToken, IDENTIFIER);
        HSHARP_ENUM_CASE(HSharpParser::EToken, INTEGER_LITERAL);
        HSHARP_ENUM_CASE(HSharpParser::EToken, STRING_LITERAL);
        HSHARP_ENUM_CASE(HSharpParser::EToken, STATEMENT_TERMINATOR);
        HSHARP_ENUM_CASE(HSharpParser::EToken, ADDITION_SIGN);
        HSHARP_ENUM_CASE(HSharpParser::EToken, SUBTRACTION_SIGN);
        HSHARP_ENUM_CASE(HSharpParser::EToken, STAR_SIGN);
        HSHARP_ENUM_CASE(HSharpParser::EToken, ASSIGNMENT_SIGN);
        HSHARP_ENUM_CASE(HSharpParser::EToken, PARENTHESIS_OPEN_SIGN);
        HSHARP_ENUM_CASE(HSharpParser::EToken, PARENTHESIS_CLOSE_SIGN);
        HSHARP_ENUM_CASE(HSharpParser::EToken, CONTEXT_OPEN_SIGN);
        HSHARP_ENUM_CASE(HSharpParser::EToken, CONTEXT_CLOSE_SIGN);
        HSHARP_ENUM_CASE(HSharpParser::EToken, OWNING_SIGN);
        HSHARP_ENUM_CASE(HSharpParser::EToken, CASCADE_BRANCHING_OPERATOR);
        default: HSHARP_NOT_IMPLEMENTED(HSharpVE::EExceptionSource::UTILITY, "enum value out of range");
    }
}

Token HSharpParser::makeToken(EToken type, std::string payload) {
    Token token;
    token.payload = std::move(payload);
    token.type = type;
    return token;
}

std::string Tokenizer::formatError(LineSpecialization lineSpec, ErrorSpecialization errorSpec) {
    static std::string formatBase = "\n{}\n";
    std::string formatted = std::vformat(formatBase, std::make_format_args(lineSpec.line));
    
    for (int i = 0; i < std::max(0, lineSpec.num - 5); ++i) {
        formatted.push_back(' ');
    }

    for (int i = std::max(lineSpec.num - 5, 0); i < std::min<int>(lineSpec.line.size(), lineSpec.num); ++i) {
        formatted.push_back('^');
    }

    for (int i = std::min<int>(lineSpec.line.size(), lineSpec.num); i < lineSpec.line.size(); ++i) {
        formatted.push_back(' ');
    }

    auto reason = HSharpVE::toString(errorSpec.reason);
    formatted += std::vformat("line {}: {}: {}", std::make_format_args(lineSpec.line, reason, errorSpec.message));
    return formatted;
}

std::istream& Tokenizer::getline(std::istream& is, std::string& line) {
    std::getline(is, line);

    while (is && line.back() == '\\') {
        std::string unformatted;
        if (!std::getline(is, unformatted)) {
            fallback("error reading line", LineSpecialization(state_.line, state_.num, 0));
        }

        strip(unformatted);
        line.pop_back();
        line.append(unformatted);
    }

    return is;
}

bool Tokenizer::isIdentifierChar(char ch, bool /* isFirst */) {
    return std::isalpha(ch); // might add something else here
}

void Tokenizer::strip(std::string& line, bool stripBothSides) {
    if (stripBothSides) {
        std::size_t i = 0;
        while (i < line.size() && std::isspace(line[i])) {
            ++i;
        }
        line = line.substr(i);
    }

    while (!line.empty() && std::isspace(line.back())) {
        line.pop_back();
    }
}

[[noreturn]] void Tokenizer::fallback(std::string error, std::optional<LineSpecialization> lineSpec) {
    ErrorSpecialization spec;
    spec.message = std::move(error);
    spec.reason = HSharpVE::EExceptionReason::UNEXPECTED_TOKEN;
    auto formatted = formatError(lineSpec.value(), spec);
    throw std::runtime_error(formatted);
}