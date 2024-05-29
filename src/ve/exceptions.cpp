#include <cstdio>
#include <cstdlib>
#include <ve/ve.hpp>

using HSharpVE::ExceptionSource;
using HSharpVE::ExceptionType;

[[noreturn]] void throwFatalException(ExceptionSource source, ExceptionType type, const char* message) {
    fputs("\033[31;1m", stderr);
    switch (source){
        case ExceptionSource::Tokenizer:
            fputs("[TOKENIZER]: ", stderr);
            break;
        case ExceptionSource::Parser:
            fputs("[PARSER]: ", stderr);
            break;
        case ExceptionSource::VirtualEnv:
            fputs("[VIRTUALENV]: ", stderr);
            break;
    }
    switch (type){
        case HSharpVE::ExceptionType::SyntaxError:
            fputs("SyntaxError: ", stderr);
            break;
        case HSharpVE::ExceptionType::TypeError:
            fputs("TypeError: ", stderr);
            break;
        case HSharpVE::ExceptionType::InvalidAssign:
            fputs("InvalidAssign: ", stderr);
            break;
        case HSharpVE::ExceptionType::NotImplemented:
            fputs("NotImplemented: ", stderr);
            break;
        case HSharpVE::ExceptionType::ConversionError:
            fputs("ConversionError: ", stderr);
            break;
    }
    fputs(message, stderr);
    fputs("\033[0m\n", stderr);
    exit(1);
}