#pragma once
#include <parser/parser.hpp>

enum class NodeType{
    INPUT,
    PRINT,
    EXIT,
    VAR,
    VAR_ASSIGN,
    TERM,
    STR_LIT,
    BIN_EXPR,
    INT_LIT,
    IDENT,
    ADD,
    SUB,
    MUL,
    DIV
};

struct Visitor{
    std::vector<NodeType>& visitors_executed;
    Visitor(std::vector<NodeType>& vector) : visitors_executed(vector){}

    void operator()(HSharpParser::NodeStmtInput* stmt) {
        visitors_executed.push_back(NodeType::INPUT);
        // Not implemented - not going to test
    }
    void operator()(HSharpParser::NodeStmtPrint* stmt) {
        visitors_executed.push_back(NodeType::PRINT);
        std::visit(*this, stmt->expr->expr);
    }
    void operator()(HSharpParser::NodeStmtExit* stmt) {
        visitors_executed.push_back(NodeType::EXIT);
        std::visit(*this, stmt->expr->expr);
    }
    void operator()(HSharpParser::NodeStmtVar* stmt) {
        visitors_executed.push_back(NodeType::VAR);
        std::visit(*this, stmt->expr->expr);
    }
    void operator()(HSharpParser::NodeStmtVarAssign* stmt) {
        visitors_executed.push_back(NodeType::VAR_ASSIGN);
        std::visit(*this, stmt->expr->expr);
    }
    void operator()(HSharpParser::NodeTerm* term) {
        visitors_executed.push_back(NodeType::TERM);
        std::visit(*this, term->term);
    }
    void operator()(const HSharpParser::NodeExpressionStrLit* expr) {
        visitors_executed.push_back(NodeType::STR_LIT);
    }
    void operator()(HSharpParser::NodeBinExpr* expr) {
        visitors_executed.push_back(NodeType::BIN_EXPR);
        std::visit(*this, expr->var);
    }
    void operator()(const HSharpParser::NodeTermIntLit* term) {
        visitors_executed.push_back(NodeType::INT_LIT);
    }
    void operator()(const HSharpParser::NodeTermIdent* term) {
        visitors_executed.push_back(NodeType::IDENT);
    }
    void operator()(const HSharpParser::NodeBinExprAdd* expr) {
        visitors_executed.push_back(NodeType::ADD);
        std::visit(*this, expr->lhs->expr);
        std::visit(*this, expr->lhs->expr);
    }
    void operator()(const HSharpParser::NodeBinExprSub* expr) {
        visitors_executed.push_back(NodeType::SUB);
        std::visit(*this, expr->lhs->expr);
        std::visit(*this, expr->lhs->expr);
    }
    void operator()(const HSharpParser::NodeBinExprMul* expr) {
        visitors_executed.push_back(NodeType::MUL);
        std::visit(*this, expr->lhs->expr);
        std::visit(*this, expr->lhs->expr);
    }
    void operator()(const HSharpParser::NodeBinExprDiv* expr) {
        visitors_executed.push_back(NodeType::DIV);
        std::visit(*this, expr->lhs->expr);
        std::visit(*this, expr->lhs->expr);
    }
    ~Visitor() = default;
};