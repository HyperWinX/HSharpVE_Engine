#include <cstdio>
#include <cstdlib>

[[noreturn]] void throwFatalVirtualEnvException(const char* message) {
    fputs(message, stderr);
    exit(1);
}