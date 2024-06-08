#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <filesystem>

#include <version.hpp>
#include <parser/tokenizer.hpp>
#include <parser/parser.hpp>
#include <main/string_split.h>
#include <ve/ve.hpp>
#include <main/arguments_handler.hpp>

using HSharpParser::Token;
using HSharpParser::NodeExit;

int main(int argc, char *argv[]) {
    // handle CLI args
    hsharp::ArgumentsHandler argsHandler;
    argsHandler.parse(std::make_unique<hsharp::ArgumentsHandler::SIOArgumentsAccessor>(), argc, argv);
    const hsharp::CLIRegistry& registry = argsHandler.registry();

    for (const hsharp::EDirective& directive : registry.getDirectives()) {
        switch (directive) {
            case hsharp::EDirective::HELP:
                hsharp::displayHelp(std::cout);
                return 0;
            case hsharp::EDirective::VERSION:
                hsharp::displayVersion(std::cout);
                return 0;
        }
    }

    std::string filename = registry.getFilename();

    std::ifstream input(filename, std::ios::binary | std::ios::ate);
    if (!input.is_open()) {
        std::cerr << "Cannot open file! Exiting now..." << std::endl;
        exit(1);
    }

    HSharpParser::Tokenizer tokenizer;
    std::vector<Token> tokens = tokenizer.tokenize(input);

    std::vector<std::string> lines;

    HSharpParser::Parser parser(tokens, lines);
    std::optional<HSharpParser::NodeProgram> root = parser.parse_program();
    if (!root.has_value()) {
        std::cerr << "Parsing failed!\n";
        exit(1);
    }

    HSharpVE::VirtualEnvironment ve(root.value(), lines, true); // for now
    ve.run();
    // Exit point
    input.close();
}