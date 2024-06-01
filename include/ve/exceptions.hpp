#pragma once

#include <boost/preprocessor.hpp>

#define X_DEFINE_ENUM_WITH_STRING_CONVERSIONS_TOSTRING_CASE(r, data, elem)    \
    case elem : return BOOST_PP_STRINGIZE(elem);

#define DEFINE_ENUM_WITH_STRING_CONVERSIONS(name, enumerators)                \
    enum name {                                                               \
        BOOST_PP_SEQ_ENUM(enumerators)                                        \
    };                                                                        \
                                                                              \
    inline const char* ToString(name v)                                       \
    {                                                                         \
        switch (v)                                                            \
        {                                                                     \
            BOOST_PP_SEQ_FOR_EACH(                                            \
                X_DEFINE_ENUM_WITH_STRING_CONVERSIONS_TOSTRING_CASE,          \
                name,                                                         \
                enumerators                                                   \
            )                                                                 \
            default: return "[Unknown " BOOST_PP_STRINGIZE(name) "]";         \
        }                                                                     \
    }


namespace HSharpVE{
DEFINE_ENUM_WITH_STRING_CONVERSIONS(ExceptionSource,
(Tokenizer)\
(Parser)\
(VirtualEnv));

DEFINE_ENUM_WITH_STRING_CONVERSIONS(ExceptionType,
(SyntaxError)\
(TypeError)\
(InvalidAssign)\
(NotImplemented)\
(ConversionError)\
(ExpressionParseError)\
(StatementParseError)\
(UnexpectedToken)\
(EndOfFile));
}

[[noreturn]] void throwFatalException(HSharpVE::ExceptionSource source, HSharpVE::ExceptionType type, std::string& message);
[[noreturn]] void throwFatalException(HSharpVE::ExceptionSource source, HSharpVE::ExceptionType type, const char* message);