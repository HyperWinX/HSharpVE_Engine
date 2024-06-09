#include <optional>
#include <iostream>
#include <functional>

#include <cassert>

#include <parser/tokens.hpp>
#include <parser/parser.hpp>
#include <ve/exceptions.hpp>

using namespace HSharpParser;

static constexpr char parser_fail_msg[] = "Parser failed to parse expression.\n";

std::optional<HSharpParser::NodeBinExpr *> HSharpParser::Parser::parse_bin_expr() {
    std::string msg{};
    HSharpVE::EExceptionReason exc_type;
    std::optional<NodeExpression*> lhs;
    NodeBinExpr* bin_expr;
    if (!(lhs = parse_expression())) {
        msg.append(parser_fail_msg);
        exc_type = HSharpVE::EExceptionReason::UNEXPECTED_TOKEN;
        //goto error;
    }

    bin_expr = allocator.alloc<NodeBinExpr>();
    if (!peek().has_value()){
        msg.append(parser_fail_msg);
        exc_type = HSharpVE::EExceptionReason::EARLY_EOF;
        //goto error;
    }
    EToken tokenType = peek()->type;
    switch (tokenType){
        case EToken::ADDITION_SIGN:{
            auto bin_expr_add = allocator.alloc<NodeBinExprAdd>();
            bin_expr_add->lhs = lhs.value();
            skip();
            bin_expr_add->rhs = parse_expression().value();
            bin_expr->var = bin_expr_add;
            return bin_expr;
        }
        case EToken::SUBTRACTION_SIGN:{
            auto bin_expr_sub = allocator.alloc<NodeBinExprSub>();
            bin_expr_sub->lhs = lhs.value();
            skip();
            bin_expr_sub->rhs = parse_expression().value();
            bin_expr->var = bin_expr_sub;
            return bin_expr;
        }
        case EToken::STAR_SIGN:{
            auto bin_expr_mul = allocator.alloc<NodeBinExprMul>();
            bin_expr_mul->lhs = lhs.value();
            skip();
            bin_expr_mul->rhs = parse_expression().value();
            bin_expr->var = bin_expr_mul;
            return bin_expr;
        }
        // case EToken::/* Not implemented */{
        //     auto bin_expr_div = allocator.alloc<NodeBinExprDiv>();
        //     bin_expr_div->lhs = lhs.value();
        //     skip();
        //     bin_expr_div->rhs = parse_expression().value();
        //     bin_expr->var = bin_expr_div;
        //     return bin_expr;
        // }
        default:
            msg.append(parser_fail_msg);
            exc_type = HSharpVE::EExceptionReason::UNEXPECTED_TOKEN;
            goto error;
    }

    error:
    error(HSharpVE::EExceptionSource::TOKENIZER, exc_type, msg);
}

std::optional<HSharpParser::NodeTerm*> HSharpParser::Parser::parse_term() {
    EToken type = peek()->type;
    switch (type){
        case EToken::INTEGER_LITERAL:{
            std::optional<Token> int_lit = consume();
            auto term_int_lit = allocator.alloc<NodeTermIntLit>();
            auto expr = allocator.alloc<NodeTerm>();
            term_int_lit->int_lit = int_lit.value();
            expr->term = term_int_lit;
            expr->line = 0; // filler
            return expr;
        }
        case EToken::IDENTIFIER:{
            std::optional<Token> ident = consume();
            auto term_ident = allocator.alloc<NodeTermIdent>();
            auto expr = allocator.alloc<NodeTerm>();
            term_ident->ident = ident.value();
            expr->term = term_ident;
            expr->line = 0; // filler
            return expr;
        }
        default:
            return {};
    }
}


std::optional<HSharpParser::NodeExpression *> HSharpParser::Parser::parse_expression(int min_prec) {
    if (auto term_lhs = parse_term()){
        auto expr_lhs = allocator.emplace<NodeExpression>(term_lhs.value());

        while (true){
            std::optional<NodeExpression*> expr_rhs;
            NodeExpression* expr_lhs2;
            NodeBinExpr* expr;
            std::optional<Token> cur_tok = peek();
            std::optional<int> prec;

            if (!cur_tok.has_value()) break;
            prec = bin_precedence(cur_tok->type);
            if (!prec.has_value() || prec < min_prec) break;

            Token token = consume();
            int next_min_prec = prec.value() + 1;
            expr_rhs = parse_expression(next_min_prec);
            if (!expr_rhs.has_value())
                HSharpVE::error(HSharpVE::EExceptionSource::PARSER,
                                HSharpVE::EExceptionReason::PARSE_ERROR,
                                "Failed to parse expression");

            expr = allocator.emplace<NodeBinExpr>();
            expr_lhs2 = allocator.emplace<NodeExpression>();
            switch(token.type){
                case EToken::ADDITION_SIGN:{
                    expr_lhs2->expr = expr_lhs->expr;
                    expr->var = allocator.emplace<NodeBinExprAdd>(expr_lhs2, expr_rhs.value());
                    break;
                }
                case EToken::SUBTRACTION_SIGN:{
                    expr_lhs2->expr = expr_lhs->expr;
                    expr->var = allocator.emplace<NodeBinExprSub>(expr_lhs2, expr_rhs.value());
                    break;
                }
                case EToken::STAR_SIGN:{
                    expr_lhs2->expr = expr_lhs->expr;
                    expr->var = allocator.emplace<NodeBinExprMul>(expr_lhs2, expr_rhs.value());
                    break;
                }
                // case TOK_FSLASH:{
                //     expr_lhs2->expr = expr_lhs->expr;
                //     expr->var = allocator.emplace<NodeBinExprDiv>(expr_lhs2, expr_rhs.value());
                //     break;
                // }
                default:
                    assert(false);
            }
            expr_lhs->expr = expr;
        }
        return expr_lhs;
    } else if (auto str_lit = try_consume(EToken::STRING_LITERAL)) {
        auto expr_str_lit = allocator.alloc<NodeExpressionStrLit>();
        auto expr = allocator.alloc<NodeExpression>();
        expr_str_lit->str_lit = str_lit.value();
        expr->expr = expr_str_lit;
        expr->line = 0; // filler
        return expr;
    }
    return {};
}

std::optional<HSharpParser::NodeStmt *> HSharpParser::Parser::parse_statement() {
    if (peek().has_value() && peek().value().type == EToken::BUILTIN_FUNCTION &&
        peek(1).has_value() && peek(1).value().type == EToken::PARENTHESIS_OPEN_SIGN) {
        // auto stmt_exit = allocator.alloc<NodeStmtExit>();
        // auto node_stmt = allocator.alloc<NodeStmt>();
        // std::optional<NodeExpression*> node_expr;
        // stmt_exit->line = consume().line;
        // skip();

        // if (!(node_expr = parse_expression())) goto error;

        // stmt_exit->expr = node_expr.value();
        // stmt_exit->line = node_expr.value()->line;

        // try_consume(TokenType::TOK_PAREN_CLOSE, 1);
        // try_consume(TokenType::TOK_SEMICOLON, 1);

        // node_stmt->statement = stmt_exit;
        // node_stmt->line = stmt_exit->line;
        // return node_stmt;
    } else if (peek().has_value() && peek().value().type == EToken::BUILTIN_FUNCTION &&
               peek(1).has_value() && peek(1).value().type == EToken::PARENTHESIS_OPEN_SIGN) {
        // skip(2);
        // auto stmt_print = allocator.alloc<NodeStmtPrint>();
        // if (auto expr = parse_expression())
        //     stmt_print->expr = expr.value();

        // try_consume(TokenType::TOK_PAREN_CLOSE, 1);
        // try_consume(TokenType::TOK_SEMICOLON, 1);

        // auto node_stmt = allocator.alloc<NodeStmt>();
        // node_stmt->statement = stmt_print;
        // return node_stmt;
    } else if (peek().has_value() && peek().value().type == EToken::BUILTIN_FUNCTION &&
               peek(1).has_value() && peek(1).value().type == EToken::PARENTHESIS_OPEN_SIGN) {
        // skip(2);
        // auto node_input = allocator.alloc<NodeStmtInput>();
        // if (auto expr = parse_expression())
        //     node_input->expr = expr.value();

        // try_consume(TokenType::TOK_PAREN_CLOSE, 1);
        // try_consume(TokenType::TOK_SEMICOLON, 1);

        // auto node_stmt = allocator.alloc<NodeStmt>();
        // node_stmt->statement = node_input;
        // return node_stmt;
    // } else if (peek().has_value() && peek().value().type == EToken:: &&
    //            peek(1).has_value() && peek(1).value().ttype == TokenType::TOK_IDENT &&
    //            peek(2).has_value() && peek(2).value().ttype == TokenType::TOK_EQUALITY_SIGN) {
    //     skip();
    //     auto node_stmt_var = allocator.alloc<NodeStmtVar>();
    //     auto node_stmt = allocator.alloc<NodeStmt>();
    //     std::optional<NodeExpression*> expr;
    //     node_stmt_var->ident = consume();
    //     skip();
    //     if (!(expr = parse_expression())) goto error;

    //     node_stmt_var->expr = expr.value();

    //     try_consume(TokenType::TOK_SEMICOLON, 1);

    //     node_stmt->statement = node_stmt_var;
    //     return node_stmt;
    } else if (peek().has_value() && peek().value().type == EToken::IDENTIFIER &&
                peek(1).has_value() && peek(1).value().type == EToken::ASSIGNMENT_SIGN) {
        auto node_stmt = allocator.alloc<NodeStmtVarAssign>();
        auto stmt = allocator.alloc<NodeStmt>();
        std::optional<NodeExpression*> expr;
        node_stmt->ident = consume();
        skip();
        if (!(expr = parse_expression())) goto error;

        node_stmt->expr = expr.value();

        try_consume(EToken::STATEMENT_TERMINATOR, 1);

        stmt->statement = node_stmt;
        return stmt;
    }

    error:
    error(HSharpVE::EExceptionSource::PARSER, HSharpVE::EExceptionReason::SYNTAX_ERROR, parser_fail_msg);
}


std::optional<HSharpParser::NodeProgram> HSharpParser::Parser::parse_program() {
    NodeProgram program;
    while (peek().has_value()) {
        std::optional<NodeStmt*> stmt = parse_statement();
        if (!stmt.has_value())
            error(HSharpVE::EExceptionSource::PARSER, HSharpVE::EExceptionReason::SYNTAX_ERROR, "Parser failed to parse statement");

        program.statements.push_back(stmt.value());
    }
    return program;
}