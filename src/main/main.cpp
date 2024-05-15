#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

#include <version.hpp>
#include <parser/parser.hpp>
#include <main/file.hpp>
#include <ve/ve.hpp>
#include <argparse/argparse.hpp>

using HSharpParser::Token;
using HSharpParser::NodeExit;

void DisplayHelp(const char*);

int main(int argc, char *argv[]) {
    std::string filename;
    argparse::ArgumentParser argparser(argv[0], VERSION, argparse::default_arguments::help);
    argparser.add_argument("file").help("File to execute").metavar("PROGRAM").store_into(filename).required();
    argparser.add_argument("--version").help("display HSharpVE version").default_value(false).implicit_value(true);
    argparser.add_argument("-v, --verbose").help("enable high verbosity level").default_value(false);
    try {
        argparser.parse_args(argc, argv);
    } catch (std::exception& exception) {
        std::cout << argparser;
        exit(1);
    }
    if (argparser["--version"] == true) {
        std::printf("%s version %s, main developer: %s, build date: %s %s\n", NAME, VERSION, MAINTAINER, __DATE__, __TIME__);
        exit(0);
    } else if (argparser["--help"] == true) {
        std::cout << argparser;
        //DisplayHelp(argv[0]);
        exit(0);
    }

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

    HSharpParser::Parser parser(tokens);
    std::optional<HSharpParser::NodeProgram> root = parser.parse_program();
    if (!root.has_value()) {
        std::cerr << "Parsing failed!\n";
        exit(1);
    }

    HSharpVE::VirtualEnvironment ve(root.value(), argparser["-v, --verbose"] == true);
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