#pragma once

// STD
#include <string>
#include <cstdint>
#include <string_view>

#define HSHARP_STRINGIFY(variable) #variable

#define HSHARP_ENUM_CASE(enum_scope, enum_value) case enum_scope::enum_value: return HSHARP_STRINGIFY(enum_value)

#define HSHARP_NOT_IMPLEMENTED(scope, message) \
    HSharpVE::error(scope, HSharpVE::EExceptionReason::NOT_IMPLEMENTED, message)

namespace HSharpVE {

    // color constants
    constexpr char RESET[] = "\033[0m";
    constexpr char RED[] = "\033[31m";

    enum class EExceptionSource : std::int_fast8_t {
        TOKENIZER,      // Tokenizer is responsible for parsing input into chunks (tokens)
        PARSER,         // Parser is responsible for processing tokens into viable abstraction
        VIRTUAL_ENV,    // Runtime for parsed code
        UTILITY,        // Utility libraries
        // 252 more values
    };

    enum class EExceptionReason : std::int_fast8_t {
        SYNTAX_ERROR,       // Holder for generic parser error
        TYPE_ERROR,         // Variable type is not appropriate in current context
        PARSE_ERROR,        // Unknown error, can be thrown in parser
        NOT_IMPLEMENTED,    // Holder for early parser versions and testing features
        CAST_ERROR,         // Errors from casting mechanisms
        EARLY_EOF,          // Expecting more tokens in input
        UNEXPECTED_TOKEN    // Expecting different token
        // 250 more values 
    };

    std::string toString(EExceptionSource source);
    std::string toString(EExceptionReason reason);

    [[noreturn]] void error(EExceptionSource source, EExceptionReason reason, std::string_view message);
}
