#include <iostream>

#include <parser/parser.hpp>
#include <ve/ve.hpp>

using HSharpVE::ExpressionVisitorRetPair;

void HSharpVE::VirtualEnvironment::StatementVisitor_StatementPrint(HSharpParser::NodeStmtPrint* stmt) {
    ExpressionVisitorRetPair pair = std::visit(exprvisitor, stmt->expr->expr);
    if (pair.type != VariableType::STRING) {
        std::cerr << "Print: expected string, but got integer\n";
        exit(1);
    }
    std::puts(static_cast<std::string*>(pair.value)->c_str());

    dispose_value(pair);
}

void HSharpVE::VirtualEnvironment::StatementVisitor_StatementExit(HSharpParser::NodeStmtExit* stmt) {
    int64_t exitcode;
    ExpressionVisitorRetPair pair = std::visit(exprvisitor, stmt->expr->expr);
    if (pair.type == VariableType::STRING) {
        if (!is_number(*static_cast<std::string*>(pair.value)))
            throwFatalVirtualEnvException("ConvertException: cannot convert string into integer");
        exitcode = std::stol(*static_cast<std::string*>(pair.value));
    } else if (pair.type != VariableType::INT) {
        std::cerr << "Exit: expected integer, but got string\n";
        exit(1);
    } else
        exitcode = *static_cast<int64_t*>(pair.value);
    dispose_value(pair);
    delete_variables();
    exit(exitcode);
}

void HSharpVE::VirtualEnvironment::StatementVisitor_StatementVar(HSharpParser::NodeStmtVar* stmt) {
    if (global_scope.variables.contains(stmt->ident.value.value())) {
        std::cerr << "Variable reinitialization is not allowed\n";
        exit(1);
    } else {
        const ExpressionVisitorRetPair pair = std::visit(exprvisitor, stmt->expr->expr);
        global_scope.variables[stmt->ident.value.value()] = {.vtype = pair.type, .value = pair.value};
    }
}

void HSharpVE::VirtualEnvironment::StatementVisitor_StatementVarAssign(HSharpParser::NodeStmtVarAssign *stmt) {
    if (!is_variable(const_cast<char*>(stmt->ident.value.value().c_str())))
        throwFatalVirtualEnvException("AssignException: cannot assign value to immediate value");
    auto variable = &global_scope.variables[stmt->ident.value.value().c_str()];
    ExpressionVisitorRetPair info = std::visit(exprvisitor, stmt->expr->expr);
    variable->vtype = info.type;

    variable->value = info.value;
}


ExpressionVisitorRetPair HSharpVE::VirtualEnvironment::ExpressionVisitor_ExprStrLit(NodeExpressionStrLit* expr) {
    auto str = static_cast<std::string*>(strings_pool.malloc());
    *str = std::string(expr->str_lit.value.value());
    return {VariableType::STRING, str};
}

ExpressionVisitorRetPair HSharpVE::VirtualEnvironment::ExpressionVisitor_ExprIntLit(HSharpParser::NodeTermIntLit* expr) {
    if (!is_number(expr->int_lit.value.value())) {
        std::cerr << "Expression is not valid integer!" << std::endl;
        exit(1);
    } else {
        auto value = static_cast<int64_t*>(integers_pool.malloc());
        *value = std::stol(expr->int_lit.value.value());
        return {VariableType::INT, value};
    }
}

ExpressionVisitorRetPair HSharpVE::VirtualEnvironment::ExpressionVisitor_ExprIdent(HSharpParser::NodeTermIdent* expr) const {
    if (!global_scope.variables.contains(expr->ident.value.value()))
        std::terminate();
    else {
        return {
            global_scope.variables.at(expr->ident.value.value()).vtype,
            global_scope.variables.at(expr->ident.value.value()).value
        };
    }
}

ExpressionVisitorRetPair HSharpVE::VirtualEnvironment::ExpressionVisitor_BinExpr(HSharpParser::NodeBinExpr *expr) const {
    return {};
}
