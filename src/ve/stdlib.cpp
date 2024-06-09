#include <iostream>
#include <cstring>

#include <ve/exceptions.hpp>
#include <parser/parser.hpp>
#include <ve/ve.hpp>

using HSharp::ValueInfo;

void HSharpVE::VirtualEnvironment::StatementVisitor_StatementPrint(HSharpParser::NodeStmtPrint* stmt) {
    ValueInfo pair = std::visit(vExpression_, stmt->expr->expr);
    std::string result;
    switch (pair.type){
        case VariableType::INT:
            result = std::to_string(*static_cast<int64_t*>(pair.value));
            break;
        case VariableType::STRING:
            result = *static_cast<std::string*>(pair.value);
            break;
        default:
            error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "print(): conversion failed: unknown type");
    }
    std::puts(result.c_str());
    dispose_value(pair);
}

void HSharpVE::VirtualEnvironment::StatementVisitor_StatementExit(HSharpParser::NodeStmtExit* stmt) {
    int64_t exitcode;
    ValueInfo pair = std::visit(vExpression_, stmt->expr->expr);
    switch(pair.type){
        case VariableType::INT:
            exitcode = *static_cast<int64_t*>(pair.value);
            break;
        case VariableType::STRING:{
            std::string* ptr = static_cast<std::string*>(pair.value);
            if (!is_number(*ptr)){
                std::string msg{};
                msg.append("exit(): conversion failed: string is not convertable to number\n");
                msg.append(std::format("\tLine {}: {}", stmt->line, lines_.at(stmt->line - 1)));
                error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, msg);
            }
            exitcode = std::stol(*ptr);
            break;
        }
        default:
            error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "exit(): conversion failed: unknown type");
    }
    dispose_value(pair);
    delete_variables();
    exit(exitcode);
}

void HSharpVE::VirtualEnvironment::StatementVisitor_StatementVar(HSharpParser::NodeStmtVar* stmt) {
    if (globalScope_.variables.contains(stmt->ident.payload)) {
        std::cerr << "Variable reinitialization is not allowed\n";
        exit(1);
    } else {
        const ValueInfo pair = std::visit(vExpression_, stmt->expr->expr);
        globalScope_.variables[stmt->ident.payload] = {.vtype = pair.type, .value = pair.value};
    }
}

void HSharpVE::VirtualEnvironment::StatementVisitor_StatementVarAssign(HSharpParser::NodeStmtVarAssign *stmt) {
    if (!is_variable(const_cast<char*>(stmt->ident.payload.c_str())))
        error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "Cannot assign value to immediate value");
    auto variable = &globalScope_.variables[stmt->ident.payload.c_str()];
    ValueInfo info = std::visit(vExpression_, stmt->expr->expr);
    delete_var_value(*variable);
    variable->vtype = info.type;
    variable->value = allocate(info.type);
    switch(info.type){
        case VariableType::INT:
            memcpy(variable->value, info.value, sizeof(int64_t));
            break;
        case VariableType::STRING:
            variable->value = new(variable->value)std::string(*static_cast<std::string*>(info.value));
            break;
        default:
            error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "Cannot assign value: invalid type");
    }
    dispose_value(info);
}


ValueInfo HSharpVE::VirtualEnvironment::ExpressionVisitor_ExprStrLit(NodeExpressionStrLit* expr) {
    auto str = static_cast<std::string*>(stringAllocator_.allocate(1));
    *str = std::string(expr->str_lit.payload);
    return {VariableType::STRING, str};
}

ValueInfo HSharpVE::VirtualEnvironment::ExpressionVisitor_ExprIntLit(HSharpParser::NodeTermIntLit* expr) {
    if (!is_number(expr->int_lit.payload)) {
        std::cerr << "Expression is not valid integer!" << std::endl;
        exit(1);
    } else {
        auto value = static_cast<int64_t*>(integerAllocator_.allocate(1));
        *value = std::stol(expr->int_lit.payload);
        return {VariableType::INT, value};
    }
}

ValueInfo HSharpVE::VirtualEnvironment::ExpressionVisitor_ExprIdent(HSharpParser::NodeTermIdent* expr) const {
    if (!globalScope_.variables.contains(expr->ident.payload))
        std::terminate();
    else {
        return {
            globalScope_.variables.at(expr->ident.payload).vtype,
            globalScope_.variables.at(expr->ident.payload).value
        };
    }
}

ValueInfo HSharpVE::VirtualEnvironment::ExpressionVisitor_BinExpr(HSharpParser::NodeBinExpr *expr) const {
    return {};
}
