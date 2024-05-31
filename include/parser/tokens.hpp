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


namespace HSharpParser{
    DEFINE_ENUM_WITH_STRING_CONVERSIONS(TokenType,
(TOK_EXIT)\
(TOK_VAR)\
(TOK_PRINT)\
(TOK_INPUT)\
(TOK_INT_LIT)\
(TOK_STR_LIT)\
(TOK_SEMICOLON)\
(TOK_PLUS)\
(TOK_MINUS)\
(TOK_FSLASH)\
(TOK_MUL_SIGN)\
(TOK_EQUALITY_SIGN)\
(TOK_PAREN_OPEN)\
(TOK_PAREN_CLOSE)\
(TOK_CURLY_OPEN)\
(TOK_CURLY_CLOSE)\
(TOK_DOT)\
(TOK_IDENT)\
(TOK_IF));
}