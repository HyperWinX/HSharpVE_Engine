#include <iostream>

#include <parser/parser.hpp>
#include <ve/ve.hpp>

using HSharpVE::ValueInfo;

void HSharpVE::VirtualEnvironment::StatementVisitor_StatementPrint(HSharpParser::NodeStmtPrint* stmt) {
    ValueInfo pair = std::visit(exprvisitor, stmt->expr->expr);
    std::string result;
    switch (pair.type){
        case VariableType::INT:
            result = std::to_string(*static_cast<int64_t*>(pair.value));
            break;
        case VariableType::STRING:
            result = *static_cast<std::string*>(pair.value);
            break;
        default:
            throwFatalVirtualEnvException("print(): conversion failed: unknown type");
    }
    std::puts(result.c_str());

    dispose_value(pair);
}

void HSharpVE::VirtualEnvironment::StatementVisitor_StatementExit(HSharpParser::NodeStmtExit* stmt) {
    int64_t exitcode;
    ValueInfo pair = std::visit(exprvisitor, stmt->expr->expr);
    switch(pair.type){
        case VariableType::INT:
            exitcode = *static_cast<int64_t*>(pair.value);
            break;
        case VariableType::STRING:{
            std::string* ptr = static_cast<std::string*>(pair.value);
            if (!is_number(*ptr))
                throwFatalVirtualEnvException("exit(): conversion failed: string is not convertable to number");
            exitcode = std::stol(*ptr);
            break;
        }
        default:
            throwFatalVirtualEnvException("exit(): conversion failed: unknown type");
    }
    dispose_value(pair);
    delete_variables();
    exit(exitcode);
}

void HSharpVE::VirtualEnvironment::StatementVisitor_StatementVar(HSharpParser::NodeStmtVar* stmt) {
    if (global_scope.variables.contains(stmt->ident.value.value())) {
        std::cerr << "Variable reinitialization is not allowed\n";
        exit(1);
    } else {
        const ValueInfo pair = std::visit(exprvisitor, stmt->expr->expr);
        global_scope.variables[stmt->ident.value.value()] = {.vtype = pair.type, .value = pair.value};
    }
}

void HSharpVE::VirtualEnvironment::StatementVisitor_StatementVarAssign(HSharpParser::NodeStmtVarAssign *stmt) {
    if (!is_variable(const_cast<char*>(stmt->ident.value.value().c_str())))
        throwFatalVirtualEnvException("AssignException: cannot assign value to immediate value");
    auto variable = &global_scope.variables[stmt->ident.value.value().c_str()];
    ValueInfo info = std::visit(exprvisitor, stmt->expr->expr);
    variable->vtype = info.type;

    variable->value = info.value;
}


ValueInfo HSharpVE::VirtualEnvironment::ExpressionVisitor_ExprStrLit(NodeExpressionStrLit* expr) {
    auto str = static_cast<std::string*>(strings_pool.malloc());
    *str = std::string(expr->str_lit.value.value());
    return {VariableType::STRING, str};
}

ValueInfo HSharpVE::VirtualEnvironment::ExpressionVisitor_ExprIntLit(HSharpParser::NodeTermIntLit* expr) {
    if (!is_number(expr->int_lit.value.value())) {
        std::cerr << "Expression is not valid integer!" << std::endl;
        exit(1);
    } else {
        auto value = static_cast<int64_t*>(integers_pool.malloc());
        *value = std::stol(expr->int_lit.value.value());
        return {VariableType::INT, value};
    }
}

ValueInfo HSharpVE::VirtualEnvironment::ExpressionVisitor_ExprIdent(HSharpParser::NodeTermIdent* expr) const {
    if (!global_scope.variables.contains(expr->ident.value.value()))
        std::terminate();
    else {
        return {
            global_scope.variables.at(expr->ident.value.value()).vtype,
            global_scope.variables.at(expr->ident.value.value()).value
        };
    }
}

ValueInfo HSharpVE::VirtualEnvironment::ExpressionVisitor_BinExpr(HSharpParser::NodeBinExpr *expr) const {
    return {};
}
