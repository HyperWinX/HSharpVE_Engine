#pragma once

#include <optional>
#include <string>
#include <vector>
#include <variant>

#include <main/file.hpp>

namespace HSharpParser {
    enum class TokenType {
        TOK_EXIT,
        TOK_VAR,
        TOK_PRINT,
        TOK_INT_LIT,
        TOK_STR_LIT,
        TOK_SEMICOLON,
        TOK_PAREN_OPEN,
        TOK_PAREN_CLOSE,
        TOK_IDENT,
        TOK_EQUALITY_SIGN
    };

    struct Token {
        TokenType ttype;
        std::optional<std::string> value{};
    };

    struct NodeExpressionIntLit {
        Token int_lit;
    };
    struct NodeExpressionStrLit {
        Token str_lit;
    };
    struct NodeExpressionIdent {
        Token ident;
    };

    struct NodeExpression {
        std::variant<NodeExpressionStrLit, NodeExpressionIntLit, NodeExpressionIdent> expr;
    };

    struct NodeExit {
        NodeExpression expr;
    };
    struct NodeStmtExit {
        NodeExpression expr;
    };
    struct NodeStmtPrint {
        NodeExpression expr;
    };
    struct NodeStmtVar {
        Token ident;
        NodeExpression expr;
    };

    struct NodeStmt {
        std::variant<NodeStmtExit, NodeStmtPrint, NodeStmtVar> statement;
    };

    struct NodeProgram {
        std::vector<NodeStmt> statements;
    };

    class Tokenizer {
    private:
        File &file;
        std::uint32_t index = 0;

        [[nodiscard]] std::optional<char> peek(int offset = 0) const;

        char consume();

    public:
        Tokenizer(File &file) : file(file) {
        }

        std::vector<Token> tokenize();
    };

    class Parser {
    private:
        const std::vector<Token> tokens;
        std::size_t index = 0;

        [[nodiscard]] std::optional<Token> peek(int offset = 0) const;

        Token consume();
        std::optional<NodeStmt> parse_statement();
        std::optional<NodeExpression> parse_expression();

    public:
        Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {
        }

        std::optional<NodeProgram> parse_program();
    };
}
