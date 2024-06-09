#pragma once

#include <optional>
#include <string>
#include <vector>
#include <variant>

#include <parser/nodes.hpp>
#include <arena_alloc/arena.hpp>
#include <parser/tokens.hpp>

namespace HSharpParser {

    bool is_bin_operator(EToken ttype);
    std::optional<int> bin_precedence(EToken ttype);

    class Parser {
    protected:
        const std::vector<Token>& tokens;
        const std::vector<std::string>& lines;
        std::size_t index = 0;
        ArenaAllocator allocator;

        [[nodiscard]] std::optional<Token> peek(int offset = 0) const;
        Token try_consume(EToken type, int mode);
        std::optional<Token> try_consume(EToken type);
        Token consume();
        void skip(int count = 1);

        std::optional<NodeStmt*> parse_statement();
        std::optional<NodeExpression*> parse_expression(int min_prec = 0);
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
