#include <cstdio>
#include <cstdlib>

#include <boost/algorithm/string.hpp>

#include <ve/ve.hpp>

using HSharpVE::ExceptionSource;
using HSharpVE::ExceptionType;

[[noreturn]] void throwFatalException(ExceptionSource source, ExceptionType type, const char* message){
    fputs("\033[31;1m", stderr);
    fputs(boost::to_upper_copy(std::format("[{}]: ", HSharpVE::ToString(source))).c_str(), stderr);
    fputs(std::format("{}: ", HSharpVE::ToString(type)).c_str(), stderr);
    fputs(message, stderr);
    fputs("\033[0m\n", stderr);
    exit(1);
}

[[noreturn]] void throwFatalException(ExceptionSource source, ExceptionType type, std::string& message) {
    throwFatalException(source, type, message.c_str());
}