#pragma once

#include <optional>
#include <string>
#include <vector>
#include <variant>

#include <parser/nodes.hpp>
#include <main/file.hpp>
#include <arena_alloc/arena.hpp>
#include <parser/tokens.hpp>

namespace HSharpParser {
    class Tokenizer {
    private:
        File &file;
        std::uint32_t index = 0;

        [[nodiscard]] std::optional<char> peek(int offset = 0) const;

        char consume();
        void skip(int count = 1);

    public:
        explicit Tokenizer(File &file) : file(file){}

        std::vector<Token> tokenize();
    };

    class Parser {
    private:
        const std::vector<Token>& tokens;
        const std::vector<std::string>& lines;
        std::size_t index = 0;
        ArenaAllocator allocator;

        [[nodiscard]] std::optional<Token> peek(int offset = 0) const;
        Token try_consume(TokenType type, int mode);
        std::optional<Token> try_consume(TokenType type);
        Token consume();
        void skip(int count = 1);

        std::optional<NodeStmt*> parse_statement();
        std::optional<NodeExpression*> parse_expression();
        std::optional<NodeBinExpr*> parse_bin_expr();
        std::optional<NodeTerm*> parse_term();

    public:
        explicit Parser(std::vector<Token>& tokens, std::vector<std::string>& lines) :
        tokens(tokens),
        lines(lines),
        allocator(1024 * 1024 * 4) {}

        std::optional<NodeProgram> parse_program();
    };
    class ArenaAllocator;
}
