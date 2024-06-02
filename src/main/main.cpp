#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <filesystem>

#include <version.hpp>
#include <parser/parser.hpp>
#include <main/file.hpp>
#include <main/string_split.h>
#include <ve/ve.hpp>
#include <argparse/argparse.hpp>
#include <main/arguments_handler.hpp>

using HSharpParser::Token;
using HSharpParser::NodeExit;

void DisplayHelp(const char*);

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
    File source_file;

    source_file.size = std::filesystem::file_size(argv[1]);
    source_file.contents = std::string(source_file.size, '\0');
    input.seekg(0);
    if (!input.read(&source_file.contents.value()[0], source_file.size)) {
        std::cerr << "File read failure!" << std::endl;
        exit(1);
    }

    HSharpParser::Tokenizer tokenizer(source_file);
    std::vector<Token> tokens = tokenizer.tokenize();

    std::vector<std::string> lines;
    split_string(source_file.contents.value(), lines);
    source_file.contents->clear();

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

void DisplayHelp(const char* program_name) {
    std::printf("Usage: %s <file> [option(s)]\n", program_name);
    std::puts("Options:");
    std::puts("  --version       Display info about version");
    std::puts("  -h, --help      Display this menu");
    std::puts("  -v, --verbose   Set high verbosity level - get more info");
}