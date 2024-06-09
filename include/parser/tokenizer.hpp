#pragma once

#include <map>
#include <cstdint>
#include <istream>
#include <optional>
#include <string>
#include <vector>

#include <parser/nodes.hpp>
#include <ve/exceptions.hpp>
#include <parser/tokens.hpp>
#include <arena_alloc/arena.hpp>

namespace HSharpParser {

    class Tokenizer {
    public:
        Tokenizer() = default;
        std::vector<Token> tokenize(std::istream& is);

    private:

        inline const static std::map<char, EToken> typings = {
            std::make_pair(';', EToken::STATEMENT_TERMINATOR),
            std::make_pair('+', EToken::ADDITION_SIGN),
            std::make_pair('-', EToken::SUBTRACTION_SIGN),
            std::make_pair('*', EToken::STAR_SIGN),
            std::make_pair('=', EToken::ASSIGNMENT_SIGN),
            std::make_pair('(', EToken::PARENTHESIS_OPEN_SIGN),
            std::make_pair(')', EToken::PARENTHESIS_CLOSE_SIGN),
            std::make_pair('{', EToken::CONTEXT_OPEN_SIGN),
            std::make_pair('}', EToken::CONTEXT_CLOSE_SIGN)
        };

        struct LineSpecialization {
            std::string line;
            std::int32_t num;
            std::int32_t pos;
        };
        
        struct ErrorSpecialization {
            HSharpVE::EExceptionReason reason;
            std::string message;
        };

        enum class EBet : std::int_fast8_t {
            IDENTIFIER,
            LITERAL,
            SPECIAL
        };

        [[noreturn]] void fallback(std::string error, std::optional<LineSpecialization> lineSpec = std::nullopt);

        // formatting
        std::string formatError(LineSpecialization lineSpec, ErrorSpecialization errorSpec);
        std::istream& getline(std::istream& ifs, std::string& line);
        void strip(std::string& line, bool stripBothSides = false);

        // reading
        EBet bet(std::string::iterator position, std::string::iterator end);
        char peek(std::string::iterator position, std::string::iterator end);
        Token read(std::string::iterator& position, std::string::iterator end, EBet bet);
        Token readIdentifier(std::string::iterator& position, std::string::iterator end);
        Token readLiteral(std::string::iterator& position, std::string::iterator end);
        Token readSpecial(std::string::iterator& position, std::string::iterator end);

        // char checks
        bool isIdentifierChar(char ch, bool isFirst = false);

    private:
        
        struct State {
            std::string line;
            std::int32_t num;
        } state_;
    };

}