#pragma once

#include <iostream>

namespace HSharpVE {
    class Logger {
    private:
        bool enabled;
    public:
        explicit Logger(bool logger_enabled) : enabled(logger_enabled) {}
        void log(const char* msg) {
            if (!enabled) return;
            std::puts(msg);
        }
    };
}