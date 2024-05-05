#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

#include <version.hpp>
#include <parser/parser.hpp>
#include <main/file.hpp>
#include <ve/ve.hpp>

void DisplayHelp();

int main(int argc, char* argv[]){
    if (argc == 1){
        DisplayHelp();
        exit(0);
    }
    std::ifstream input(argv[1], std::ios::binary | std::ios::ate);
    if (!input.is_open()){
        std::cerr << "Cannot open file! Exiting now..." << std::endl;
        exit(1);
    }
    File source_file;

    source_file.size = std::filesystem::file_size(argv[1]);
    source_file.contents = std::string(source_file.size, '\0');
    input.seekg(0);
    if (!input.read(&source_file.contents.value()[0], source_file.size)){
        std::cerr << "File read failure!" << std::endl;
        exit(1);
    }

    Tokenizer tokenizer(source_file);
    std::vector<Token> tokens = tokenizer.tokenize();
    
    Parser parser(tokens);
    std::optional<NodeExit> root = parser.parse();
    if (!root.has_value()){
        std::cerr << "Parsing failed!\n";
        exit(1);
    }
    
    VirtualEnvironment ve(root.value());
    ve.run();

    // Exit point
    input.close();
}

void DisplayHelp(){
    std::cout << "Please, pass at least one argument to continue." << std::endl;
}