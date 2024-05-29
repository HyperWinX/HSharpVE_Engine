#pragma once

#include <optional>
#include <string>
#include <vector>
#include <variant>

#include <main/file.hpp>
#include <arena_alloc/arena.hpp>

namespace HSharpParser {
    enum class TokenType {
        TOK_EXIT,
        TOK_VAR,
        TOK_PRINT,
        TOK_INPUT,
        TOK_INT_LIT,
        TOK_STR_LIT,
        TOK_SEMICOLON,
        TOK_PLUS,
        TOK_MINUS,
        TOK_FSLASH,
        TOK_MUL_SIGN,
        TOK_EQUALITY_SIGN,
        TOK_PAREN_OPEN,
        TOK_PAREN_CLOSE,
        TOK_CURLY_OPEN,
        TOK_CURLY_CLOSE,
        TOK_IDENT,
        TOK_IF
    };

    struct Token {
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
    };
    struct NodeTermIdent {
        Token ident;
    };
    struct NodeExpressionStrLit {
        Token str_lit;
    };

    struct NodeBinExpr {
        std::variant<NodeBinExprAdd*,
                    NodeBinExprSub*,
                    NodeBinExprMul*,
                    NodeBinExprDiv*> var;
    };

    struct NodeTerm {
        std::variant<NodeTermIntLit*, NodeTermIdent*> term;
    };

    /* Basic expression node, includes all possible expressions */
    struct NodeExpression {
        std::variant<NodeTerm*, NodeExpressionStrLit*, NodeBinExpr*> expr;
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

    public:
        explicit Tokenizer(File &file) : file(file){}

        std::vector<Token> tokenize();
    };

    class Parser {
    private:
        const std::vector<Token> tokens;
        std::size_t index = 0;
        ArenaAllocator allocator;

        [[nodiscard]] std::optional<Token> peek(int offset = 0) const;
        Token try_consume(TokenType type, const char* err_msg);
        std::optional<Token> try_consume(TokenType type);
        Token consume();
        void skip(int count = 1);

        std::optional<NodeStmt*> parse_statement();
        std::optional<NodeExpression*> parse_expression();
        std::optional<NodeBinExpr*> parse_bin_expr();
        std::optional<NodeTerm*> parse_term();

    public:
        explicit Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), allocator(1024 * 1024 * 4) {}

        std::optional<NodeProgram> parse_program();
    };
    class ArenaAllocator;
}
