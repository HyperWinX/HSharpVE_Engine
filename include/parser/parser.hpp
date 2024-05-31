#pragma once

#include <optional>
#include <string>
#include <vector>
#include <variant>

#include <main/file.hpp>
#include <arena_alloc/arena.hpp>
#include <parser/tokens.hpp>

namespace HSharpParser {
    struct Token {
        uint32_t line;
        TokenType ttype{};
        std::optional<std::string> value{};
    };

    /* Forward declarations to allow any order of regular declarations */
    struct NodeExpression;
    struct NodeExpressionIdent;
    struct NodeBinExpr;
    struct NodeBinExprAdd;
    struct NodeBinExprSub;
    struct NodeBinExprMul;
    struct NodeBinExprDiv;
    struct NodeStmt;
    struct NodeStmtExit;
    struct NodeStmtInput;
    struct NodeStmtPrint;
    struct NodeStmtVar;
    struct NodeTerm;
    struct NodeTermIdent;
    struct NodeTermIntLit;
    struct NodeProgram;

    /* Base nodes declarations */
    struct NodeTermIntLit {
        Token int_lit;
        uint32_t line;
    };
    struct NodeTermIdent {
        Token ident;
        uint32_t line;
    };
    struct NodeExpressionStrLit {
        Token str_lit;
        uint32_t line;
    };

    struct NodeBinExpr {
        std::variant<NodeBinExprAdd*,
                    NodeBinExprSub*,
                    NodeBinExprMul*,
                    NodeBinExprDiv*> var;
        uint32_t line;
    };

    struct NodeTerm {
        std::variant<NodeTermIntLit*, NodeTermIdent*> term;
        uint32_t line;
    };

    /* Basic expression node, includes all possible expressions */
    struct NodeExpression {
        std::variant<NodeTerm*, NodeExpressionStrLit*, NodeBinExpr*> expr;
        uint32_t line;
    };

    /* Binary expressions */
    struct NodeBinExprAdd {
        NodeExpression *lhs{}, *rhs{};
    };
    struct NodeBinExprSub {
        NodeExpression *lhs{}, *rhs{};
    };
    struct NodeBinExprMul {
        NodeExpression *lhs{}, *rhs{};
    };
    struct NodeBinExprDiv {
        NodeExpression *lhs{}, *rhs{};
    };

    /* Statement nodes */
    struct NodeExit {
        NodeExpression* expr;
    };
    struct NodeStmtExit {
        NodeExpression* expr;
        uint32_t line;
    };
    struct NodeStmtPrint {
        NodeExpression* expr;
    };
    struct NodeStmtInput {
        NodeExpression* expr;
    };
    struct NodeStmtVar {
        Token ident{};
        NodeExpression* expr{};
    };
    struct NodeStmtVarAssign {
        Token ident{};
        NodeExpression* expr{};
    };
    struct NodeStmt {
        std::variant<NodeStmtExit*,
                    NodeStmtPrint*,
                    NodeStmtInput*,
                    NodeStmtVar*,
                    NodeStmtVarAssign*> statement;
        uint32_t line;
    };

    /* Start of AST */
    struct NodeProgram {
        std::vector<NodeStmt*> statements;
    };

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
