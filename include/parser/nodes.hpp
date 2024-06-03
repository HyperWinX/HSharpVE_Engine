#pragma once

#include <vector>
#include <variant>

#include <cstdint>

#include "tokens.hpp"

namespace HSharpParser{
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
}