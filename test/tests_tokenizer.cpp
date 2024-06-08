#include <gtest/gtest.h>
#include <parser/parser.hpp>
#include <main/file.hpp>

using HSharpParser::Tokenizer;
using HSharpParser::TokenType;
using HSharpParser::Token;
using std::vector;

TEST(TOKENIZER, TOK_EXIT){
    File file;
    file.contents = "exit";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_EXIT);
}

TEST(TOKENIZER, TOK_VAR){
    File file;
    file.contents = "var";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_VAR);
}

TEST(TOKENIZER, TOK_PRINT){
    File file;
    file.contents = "print";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_PRINT);
}

TEST(TOKENIZER, TOK_INPUT){
    File file;
    file.contents = "input";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_INPUT);
}

TEST(TOKENIZER, TOK_IDENT){
    File file;
    file.contents = "identifier";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_IDENT);
}

TEST(TOKENIZER, TOK_INT_LIT){
    File file;
    file.contents = "5";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_INT_LIT);
}

TEST(TOKENIZER, TOK_SEMICOLON){
    File file;
    file.contents = ";";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_SEMICOLON);
}

TEST(TOKENIZER, TOK_PAREN_OPEN){
    File file;
    file.contents = "(";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_PAREN_OPEN);
}

TEST(TOKENIZER, TOK_PAREN_CLOSE){
    File file;
    file.contents = ")";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_PAREN_CLOSE);
}

TEST(TOKENIZER, TOK_EQUALITY_SIGN){
    File file;
    file.contents = "=";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_EQUALITY_SIGN);
}

TEST(TOKENIZER, TOK_PLUS){
    File file;
    file.contents = "+";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_PLUS);
}

TEST(TOKENIZER, TOK_MINUS){
    File file;
    file.contents = "-";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_MINUS);
}

TEST(TOKENIZER, TOK_MUL_SIGN){
    File file;
    file.contents = "*";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_MUL_SIGN);
}

TEST(TOKENIZER, TOK_FSLASH){
    File file;
    file.contents = "/";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_FSLASH);
}

TEST(TOKENIZER, TOK_DOT){
    File file;
    file.contents = ".";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].ttype, TokenType::TOK_DOT);
}

TEST(TOKENIZER, COMMENT){
    File file;
    file.contents = "// one line comment\n";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 0);
}

TEST(TOKENIZER, TOK_MULTILINE_COMMENT){
    File file;
    file.contents = "/* first line of comment\nsecond line of comment */";
    file.size = file.contents->length();
    Tokenizer tokenizer(file);
    vector<Token> tokens = tokenizer.tokenize();
    EXPECT_EQ(tokens.size(), 0);
}