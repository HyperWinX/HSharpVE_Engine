#include <iostream>
#include <fstream>
#include <vector>

#include <version.hpp>
#include <main/file.hpp>
#include <parser/parser.hpp>

void DisplayHelp();

int main(int argc, char* argv[]){
    if (argc == 1){
        DisplayHelp();
        exit(0);
    }
    std::ifstream input = {};
    File source_file;
    
    input = OpenFile(argv[1]);

    source_file.size = GetFilesize(argv[1]);
    source_file.contents = reinterpret_cast<char*>(malloc(source_file.size));
    if (!source_file.contents){
        std::cerr << "Memory allocation failed! Exiting now..." << std::endl;
        input.close();
        exit(1);
    }

    ReadFile(input, source_file.contents, source_file.size);

    //std::vector<Token> tokens = Tokenize(buf);

    // Exit point
    free(source_file.contents);
    input.close();
}

void DisplayHelp(){
    std::cout << "Please, pass at least one argument to continue." << std::endl;
}