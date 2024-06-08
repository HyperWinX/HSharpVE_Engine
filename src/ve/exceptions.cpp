// STD
#include <iostream>

// local
#include <ve/exceptions.hpp>

using namespace HSharpVE;

std::string HSharpVE::toString(EExceptionSource source) {
    switch (source) {
        HSHARP_ENUM_CASE(HSharpVE::EExceptionSource, PARSER);
        HSHARP_ENUM_CASE(HSharpVE::EExceptionSource, UTILITY);
        HSHARP_ENUM_CASE(HSharpVE::EExceptionSource, TOKENIZER);
        HSHARP_ENUM_CASE(HSharpVE::EExceptionSource, VIRTUAL_ENV);
        default: HSHARP_NOT_IMPLEMENTED(EExceptionSource::UTILITY, "enum value out of range");
    }
}

std::string HSharpVE::toString(EExceptionReason reason) {
    switch (reason) {
        HSHARP_ENUM_CASE(HSharpVE::EExceptionReason, EARLY_EOF);
        HSHARP_ENUM_CASE(HSharpVE::EExceptionReason, CAST_ERROR);
        HSHARP_ENUM_CASE(HSharpVE::EExceptionReason, TYPE_ERROR);
        HSHARP_ENUM_CASE(HSharpVE::EExceptionReason, SYNTAX_ERROR);
        HSHARP_ENUM_CASE(HSharpVE::EExceptionReason, NOT_IMPLEMENTED);
        HSHARP_ENUM_CASE(HSharpVE::EExceptionReason, UNEXPECTED_TOKEN);
        default: HSHARP_NOT_IMPLEMENTED(EExceptionSource::UTILITY, "Enum value out of range");
    }
}

[[noreturn]] void HSharpVE::error(EExceptionSource source, EExceptionReason reason, std::string_view message) {
    std::cerr << RED << '[' << toString(source) << ']'
              << ' ' << toString(reason) << ": " << message 
              << RESET;
    std::abort();
}