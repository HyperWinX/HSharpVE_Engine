namespace HSharpVE{
    enum class ExceptionSource;
    enum class ExceptionType;
}

[[noreturn]] void throwFatalException(HSharpVE::ExceptionSource source, HSharpVE::ExceptionType type, const char* message);