#include <iostream>
#include <string>
#include <cstring>

#include <parser/parser.hpp>
#include <ve/ve.hpp>

using std::uint32_t;

bool is_number(const std::string& s);

void HSharpVE::VirtualEnvironment::exec_expression(const HSharpParser::NodeExpression* expr) {
    std::visit(exprvisitor, expr->expr);
}

void HSharpVE::VirtualEnvironment::exec_statement(const HSharpParser::NodeStmt* stmt) {
    std::visit(stmtvisitor, stmt->statement);
}

void HSharpVE::VirtualEnvironment::delete_variables() {
    for (auto pair : global_scope.variables) {
        switch (pair.second.vtype) {
            case VariableType::INT: integers_pool.free(static_cast<int64_t*>(pair.second.value)); break;
            case VariableType::STRING: strings_pool.free(pair.second.value); break;
            default: std::terminate();
        }
    }
    global_scope.variables.clear();
}

bool HSharpVE::VirtualEnvironment::is_variable(char* name) {
    auto it = std::find_if(std::begin(global_scope.variables), std::end(global_scope.variables), [name](auto&& arg) {
        return !strcmp(arg.first.c_str(), name);
    });
    return it != std::end(global_scope.variables);
}

bool HSharpVE::VirtualEnvironment::is_variable_value(void* value) {
    auto it = std::find_if(std::begin(global_scope.variables),
        std::end(global_scope.variables), [value](auto&& arg) {
            return arg.second.value == value;
        });
    return it != std::end(global_scope.variables);
}

void HSharpVE::VirtualEnvironment::dispose_value(void* value, VariableType valuetype) {
    if (!is_variable_value(value)) {
        switch (valuetype) {
            case VariableType::INT: integers_pool.free(static_cast<int64_t*>(value));
            case VariableType::STRING: strings_pool.free(value);
            default: std::terminate();
        }
    }
}

void HSharpVE::VirtualEnvironment::run() {
    global_scope = {};
    for (const HSharpParser::NodeStmt* stmt : root.statements) {
        exec_statement(stmt);
    }
}

bool HSharpVE::VirtualEnvironment::is_number(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}