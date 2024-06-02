#include <memory>
#include <format>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <exception>

#include <version.hpp>
#include <argparse/argparse.hpp>

// local
#include <main/arguments_handler.hpp>

using namespace hsharp;

std::string hsharp::mapDirective(EDirective directive, bool useCLINotation) {
    switch (directive) {
        case EDirective::HELP:
            return (useCLINotation) ? "--help" : "help_directive";
        case EDirective::VERSION:
            return (useCLINotation) ? "--version" : "version_directive";
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
    argparse::ArgumentParser parser;

    parser.add_argument("filename")
        .required();

    parser.add_argument("--version", "-v")
        .default_value(false)
        .implicit_value(true);

    parser.add_argument("-v, --verbose")
        .default_value(false);

    parser.add_argument("-h", "--help")
        .default_value(false)
        .implicit_value(true);

    try { 
        parser.parse_args(argc, argv); 
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

void hsharp::ArgumentsHandler::handleDirectives(argparse::ArgumentParser& parser, std::unique_ptr<CLIRegistry::IArgumentsAccessor>& accessor) noexcept {
    for (const EDirective& directive : supportedDirectives) {
        auto directiveName = mapDirective(directive, true);
        if (parser.get<bool>(directiveName)) {
            accessor->set(mapDirective(directive), directive);
        }
    }
}

void hsharp::ArgumentsHandler::handleVariables(argparse::ArgumentParser& parser, std::unique_ptr<CLIRegistry::IArgumentsAccessor>& accessor) noexcept {
    accessor->set("filename", parser.get<std::string>("filename"));
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