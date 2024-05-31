#include <optional>
#include <iostream>
#include <valarray>

#include <boost/preprocessor.hpp>

#include <parser/parser.hpp>
#include <ve/exceptions.hpp>

std::optional<HSharpParser::NodeBinExpr *> HSharpParser::Parser::parse_bin_expr() {
    if (auto lhs = parse_expression()) {
        auto bin_expr = allocator.alloc<NodeBinExpr>();
        if (peek().has_value() && peek().value().ttype == TokenType::TOK_PLUS) {
            auto bin_expr_sub = allocator.alloc<NodeBinExprAdd>();
            bin_expr_sub->lhs = lhs.value();
            skip();
            if (auto rhs = parse_expression()) {
                bin_expr_sub->rhs = rhs.value();
                bin_expr->var = bin_expr_sub;
                return bin_expr;
            } else {
                std::cerr << "Cannot parse binary expression: invalid expression" << std::endl;
                exit(1);
            }
        } else if (peek().has_value() && peek().value().ttype == TokenType::TOK_MINUS) {
            auto bin_expr_sub = allocator.alloc<NodeBinExprSub>();
            bin_expr_sub->lhs = lhs.value();
            skip();
            if (auto rhs = parse_expression()) {
                bin_expr_sub->rhs = rhs.value();
                bin_expr->var = bin_expr_sub;
                return bin_expr;
            } else {
                std::cerr << "Cannot parse binary expression: invalid expression" << std::endl;
                exit(1);
            }
        } else if (peek().has_value() && peek().value().ttype == TokenType::TOK_MUL_SIGN) {
            auto bin_expr_mul = allocator.alloc<NodeBinExprMul>();
            bin_expr_mul->lhs = lhs.value();
            skip();
            if (auto rhs = parse_expression()) {
                bin_expr_mul->rhs = rhs.value();
                bin_expr->var = bin_expr_mul;
                return bin_expr;
            } else {
                std::cerr << "Cannot parse binary expression: invalid expression" << std::endl;
                exit(1);
            }
        } else if (peek().has_value() && peek().value().ttype == TokenType::TOK_FSLASH) {
            auto bin_expr_div = allocator.alloc<NodeBinExprDiv>();
            bin_expr_div->lhs = lhs.value();
            consume();
            if (auto rhs = parse_expression()) {
                bin_expr_div->rhs = rhs.value();
                bin_expr->var = bin_expr_div;
                return bin_expr;
            } else {
                std::cerr << "Cannot parse binary expression: invalid expression" << std::endl;
                exit(1);
            }
        } else {
            std::cerr << "Cannot parse binary expression: invalid expression" << std::endl;
            exit(1);
        }
    } else {
        return {};
    }
}

std::optional<HSharpParser::NodeTerm*> HSharpParser::Parser::parse_term() {
    if (auto int_lit = try_consume(TokenType::TOK_INT_LIT)) {
        auto term_int_lit = allocator.alloc<NodeTermIntLit>();
        auto expr = allocator.alloc<NodeTerm>();
        term_int_lit->int_lit = int_lit.value();
        expr->term = term_int_lit;
        expr->line = int_lit.value().line;
        return expr;
    } else if (auto ident = try_consume(TokenType::TOK_IDENT)) {
        auto term_ident = allocator.alloc<NodeTermIdent>();
        auto expr = allocator.alloc<NodeTerm>();
        term_ident->ident = ident.value();
        expr->term = term_ident;
        expr->line = ident.value().line;
        return expr;
    }
    return {};
}


std::optional<HSharpParser::NodeExpression *> HSharpParser::Parser::parse_expression() {
    if (auto term = parse_term()) {
        if (try_consume(TokenType::TOK_PLUS)) {
            auto bin_expr = allocator.alloc<NodeBinExpr>();
            auto bin_expr_add = allocator.alloc<NodeBinExprAdd>();
            auto lhs_expr = allocator.alloc<NodeExpression>();
            lhs_expr->expr = term.value();
            bin_expr_add->lhs = lhs_expr;
            if (auto rhs = parse_expression()) {
                auto expr = allocator.alloc<NodeExpression>();
                bin_expr_add->rhs = rhs.value();
                bin_expr->var = bin_expr_add;
                expr->expr = bin_expr;
                return expr;
            } else {
                std::cerr << "Cannot parse binary expression: invalid expression" << std::endl;
                exit(1);
            }
        } else {
            auto expr = allocator.alloc<NodeExpression>();
            expr->expr = term.value();
            expr->line = term.value()->line;
            return expr;
        }
    } else if (auto str_lit = try_consume(TokenType::TOK_STR_LIT)) {
        auto expr_str_lit = allocator.alloc<NodeExpressionStrLit>();
        auto expr = allocator.alloc<NodeExpression>();
        expr_str_lit->str_lit = str_lit.value();
        expr->expr = expr_str_lit;
        expr->line = str_lit->line;
        return expr;
    } else if (auto bin_expr = parse_bin_expr()) {
        auto expr = allocator.alloc<NodeExpression>();
        expr->expr = bin_expr.value();
        return expr;
    } else {
        std::string msg{};
        msg.append("Parser failed to parse expression.\n");
        msg.append(std::format("Last token: {}", ToString(tokens[index].ttype)));
        throwFatalException(HSharpVE::ExceptionSource::Parser,
                            HSharpVE::ExceptionType::ExpressionParseError,
                            msg);
    }
}

std::optional<HSharpParser::NodeStmt *> HSharpParser::Parser::parse_statement() {
    if (peek().has_value() && peek().value().ttype == TokenType::TOK_EXIT &&
        peek(1).has_value() && peek(1).value().ttype == TokenType::TOK_PAREN_OPEN) {
        auto stmt_exit = allocator.alloc<NodeStmtExit>();
        stmt_exit->line = consume().line;
        skip();
        if (auto node_expr = parse_expression()) {
            stmt_exit->expr = node_expr.value();
            stmt_exit->line = node_expr.value()->line;
        } else {
            std::cerr << "Evaluation of expression is impossible: invalid expression.\n";
            exit(1);
        }

        try_consume(TokenType::TOK_PAREN_CLOSE, 1);
        try_consume(TokenType::TOK_SEMICOLON, 1);

        auto node_stmt = allocator.alloc<NodeStmt>();
        node_stmt->statement = stmt_exit;
        node_stmt->line = stmt_exit->line;
        return node_stmt;
    } else if (peek().has_value() && peek().value().ttype == TokenType::TOK_PRINT &&
               peek(1).has_value() && peek(1).value().ttype == TokenType::TOK_PAREN_OPEN) {
        skip(2);
        auto stmt_print = allocator.alloc<NodeStmtPrint>();
        if (auto expr = parse_expression())
            stmt_print->expr = expr.value();

        try_consume(TokenType::TOK_PAREN_CLOSE, 1);
        try_consume(TokenType::TOK_SEMICOLON, 1);

        auto node_stmt = allocator.alloc<NodeStmt>();
        node_stmt->statement = stmt_print;
        return node_stmt;
    } else if (peek().has_value() && peek().value().ttype == TokenType::TOK_INPUT &&
               peek(1).has_value() && peek(1).value().ttype == TokenType::TOK_PAREN_OPEN) {
        skip(2);
        auto node_input = allocator.alloc<NodeStmtInput>();
        if (auto expr = parse_expression())
            node_input->expr = expr.value();

        try_consume(TokenType::TOK_PAREN_CLOSE, 1);
        try_consume(TokenType::TOK_SEMICOLON, 1);

        auto node_stmt = allocator.alloc<NodeStmt>();
        node_stmt->statement = node_input;
        return node_stmt;
    } else if (peek().has_value() && peek().value().ttype == TokenType::TOK_VAR &&
               peek(1).has_value() && peek(1).value().ttype == TokenType::TOK_IDENT &&
               peek(2).has_value() && peek(2).value().ttype == TokenType::TOK_EQUALITY_SIGN) {
        skip();
        auto node_stmt_var = allocator.alloc<NodeStmtVar>();
        node_stmt_var->ident = consume();
        skip();
        if (auto expr = parse_expression()) {
            node_stmt_var->expr = expr.value();
        } else {
            std::cerr << "Invalid expression!\n";
            exit(1);
        }

        try_consume(TokenType::TOK_SEMICOLON, 1);

        auto node_stmt = allocator.alloc<NodeStmt>();
        node_stmt->statement = node_stmt_var;
        return node_stmt;
    } else if (peek().has_value() && peek().value().ttype == TokenType::TOK_IDENT &&
                peek(1).has_value() && peek(1).value().ttype == TokenType::TOK_EQUALITY_SIGN) {
        auto node_stmt = allocator.alloc<NodeStmtVarAssign>();
        node_stmt->ident = consume();
        skip();
        if (auto expr = parse_expression())
            node_stmt->expr = expr.value();
        else {
            std::cerr << "Failed to parse expression\n";
            exit(1);
        }

        try_consume(TokenType::TOK_SEMICOLON, 1);

        auto stmt = allocator.alloc<NodeStmt>();
        stmt->statement = node_stmt;
        return stmt;
    } else {
        return {};
    }
}


std::optional<HSharpParser::NodeProgram> HSharpParser::Parser::parse_program() {
    NodeProgram program;
    while (peek().has_value()) {
        std::optional<NodeStmt*> stmt = parse_statement();
        if (stmt.has_value()) {
            program.statements.push_back(stmt.value());
        } else {
            std::cerr << "Invalid statement!\n";
            exit(1);
        }
    }
    return program;
}