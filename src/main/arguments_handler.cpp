// STD
#include <iterator>
#include <memory>
#include <format>
#include <string>
#include <vector>
#include <iomanip>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <exception>
#include <algorithm>
#include <string_view>

// Local
#include <version.hpp>
#include <main/arguments_handler.hpp>

using namespace hsharp;

std::string hsharp::mapDirective(EDirective directive, bool useCLINotation) {
    switch (directive) {
        case EDirective::HELP:
            return (useCLINotation) ? "help" : "help_directive";
        case EDirective::VERSION:
            return (useCLINotation) ? "version" : "version_directive";
    }
    return "";
}

hsharp::CLIRegistry::CLIRegistry(std::unique_ptr<const IArgumentsAccessor> accessor) {
    handleRegistrySetup(std::move(accessor));
}

void hsharp::CLIRegistry::handleRegistrySetup(std::unique_ptr<const IArgumentsAccessor> accessor) {
    filename_ = std::get<std::string>(accessor->get("filename"));
    for (const EDirective& directive : supportedDirectives) {
        if (accessor->has(mapDirective(directive))) {
            directives_.push_back(std::get<EDirective>(accessor->get(mapDirective(directive))));
        }
    }
}

const std::vector<EDirective>& hsharp::CLIRegistry::getDirectives() const noexcept { return directives_; }
const std::string& hsharp::CLIRegistry::getFilename() const noexcept { return filename_; }

void hsharp::ArgumentsHandler::SIOArgumentsAccessor::set(const std::string& argument, ArgumentType value) {
    // TODO: check double argument init
    args_[argument] = std::move(value);
}

const ArgumentType& hsharp::ArgumentsHandler::SIOArgumentsAccessor::get(const std::string& argument) const {
    auto arg = args_.find(argument);
    if (arg == args_.end()) {
        throw std::runtime_error("Accessing non-existing argument: " + argument);
    }
    return arg->second;
}

const ArgumentType& hsharp::ArgumentsHandler::SIOArgumentsAccessor::operator[](const std::string& argument) const {
    return get(argument);
}

bool hsharp::ArgumentsHandler::SIOArgumentsAccessor::has(const std::string& argument) const {
    return args_.find(argument) != args_.end();
}

void hsharp::ArgumentsHandler::parse(std::unique_ptr<CLIRegistry::IArgumentsAccessor> accessor, int argc, char *argv[]) {
    Parser parser;

    try { 
        parser.parse(argv, argc);
    } catch (const std::exception& error) {
        std::puts("[util] processing CLI args: error");
        displayHelp(std::cerr);
        throw;
    }

    handleDirectives(parser, accessor);
    handleVariables(parser, accessor);
    registry_ = std::make_unique<CLIRegistry>(std::move(accessor));
}

void hsharp::ArgumentsHandler::clear() {
    registry_.reset();
}

const CLIRegistry& hsharp::ArgumentsHandler::registry() const {
    if (registry_) {
        return *registry_;
    }
    throw std::runtime_error("missing registry. Did you call parse()?");
}

void hsharp::ArgumentsHandler::handleDirectives(Parser& parser, std::unique_ptr<CLIRegistry::IArgumentsAccessor>& accessor) noexcept {
    for (const EDirective& directive : supportedDirectives) {
        auto directiveName = mapDirective(directive, true);
        if (std::get<bool>(parser.access(directiveName))) {
            accessor->set(mapDirective(directive), directive);
        }
    }
}

void hsharp::ArgumentsHandler::handleVariables(Parser& parser, std::unique_ptr<CLIRegistry::IArgumentsAccessor>& accessor) noexcept {
    auto files = std::get<std::vector<std::string>>(parser.access("files"));
    if (files.size()) {
        accessor->set("filename", std::get<std::vector<std::string>>(parser.access("files")).front()); // for now
        return;
    }
    accessor->set("filename", "");
}

void hsharp::displayHelp(std::ostream& os) {
    displayVersion(os);
    os << "\nOptions \n\n"
       << std::left << std::setw(16) << " FILENAME" << "File to process: compile and run. \n"
       << std::left << std::setw(16) << " -v --version" << "Show version and exit. \n"
       << std::left << std::setw(16) << " -h --help" << "Show version and this text and exit. \n";
}

void hsharp::displayVersion(std::ostream& os) {
    os << std::vformat("{} version {}, main developer: {}, build date: {} {}\n",
        std::make_format_args(HSHARP_NAME, HSHARP_VERSION, HSHARP_MAINTAINER, __DATE__, __TIME__));
}

hsharp::ArgumentsHandler::Parser::Parser()
: option_("files")
, current_(magicArgCount, false)
{}

const ArgumentType& hsharp::ArgumentsHandler::Parser::access(const std::string& argument) {
    return args_[argument];
}

void hsharp::ArgumentsHandler::Parser::parse(const char* const* argv_, int argc_) {
    pipe_ = {argv_, argc_};

    int position = 1;
    handleArgs(position);
    while (position < pipe_.size) {
        handleFlag(position);
        handleArgs(position);
    }

    for (const auto& item : matcher_) {
        if (!item.second.args && args_.find(item.first) == args_.end()) {
            args_[item.first] = false;
        }
    }
}

void hsharp::ArgumentsHandler::Parser::handleFlag(int& position) {
    const char* token = pipe_.data[position++];
    if (std::string_view(token).starts_with("--")) {
        matchLongFlag(token);
    } else if (std::string_view(token).starts_with("-")) {
        matchShortFlag(token);
    } else {
        fallback(std::vformat("flag {} does not start with one or two dashes", std::make_format_args(token)));
    }
}

void hsharp::ArgumentsHandler::Parser::handleArgs(int& position) {
    if (current_.args == magicArgCount || current_.args) {
        std::vector<std::string> registry;
        if (current_.args == magicArgCount) {
            registry = eat_while(position, [](const char* token) {
                return !std::string_view(token).starts_with('-');
            });
        } else {
            registry = eat_n(position, current_.args);
        }
        position += parseArgs(option_, registry);
    } else {
        args_[option_] = true;
    }
}

int hsharp::ArgumentsHandler::Parser::parseArgs(std::string option, std::vector<std::string> tokens) {
    if (current_.args == magicArgCount || current_.args > 1) {
        auto size = tokens.size();
        args_[option] = std::move(tokens);
        return size;
    } else {
        args_[option] = tokens.front();
        return 1;
    }
}

std::vector<std::string> hsharp::ArgumentsHandler::Parser::eat_n(int position, int n) {
    if (pipe_.size < position + n) {
        int outOfBounds = position + n;
        fallback(std::vformat(
            "not enough data chunks in pipe, shift: {}, can provide: {}", 
            std::make_format_args(outOfBounds, pipe_.size))
        );
    }
    std::vector<std::string> args (n);
    std::copy(pipe_.data + position, pipe_.data + position + n, args.begin());
    return args;
}

std::vector<std::string> hsharp::ArgumentsHandler::Parser::eat_while(int position, std::function<bool(const char*)> predicate) {
    int i = position;
    std::vector<std::string> args;
    while (predicate(pipe_.data[i])) {
        args.push_back(pipe_.data[i]);
        if (++i >= pipe_.size) {
            break;
        }
    }
    return args;
}

void hsharp::ArgumentsHandler::Parser::matchShortFlag(const char* token) {
    bool hungry = false;
    while (!hungry && ++token && *token) {
        char option = *token;
        auto lookup = std::find_if(
            matcher_.begin(), matcher_.end(), [option](const auto& item) {
                return item.first.front() == option;
            }
        );

        if (lookup == matcher_.end()) {
            fallback("could not match short option");
        }

        option_ = lookup->first;
        current_ = lookup->second;
        hungry = current_.args > 0;
    }

    if (hungry && token) {
        fallback("option is left hungry");
    }
}

void hsharp::ArgumentsHandler::Parser::matchLongFlag(const char* token) {
    auto lookup = std::find_if(
        matcher_.begin(), matcher_.end(), [token](const auto& item) {
            return std::strcmp(item.first.c_str(), token + 2) == 0;
        }
    );

    if (lookup == matcher_.end()) {
        fallback(std::vformat("could not match long option: {}", std::make_format_args(token)));
    }

    option_ = lookup->first;
    current_ = lookup->second;
}

void hsharp::ArgumentsHandler::Parser::fallback(const std::string& message) {
    throw std::runtime_error(message);
}