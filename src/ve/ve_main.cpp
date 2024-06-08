#include <iostream>
#include <memory>
#include <string>
#include <cstring>

#include <ve/exceptions.hpp>
#include <parser/parser.hpp>
#include <ve/ve.hpp>

using std::uint32_t;
using std::string;

/* All internal function definitions */
void HSharpVE::VirtualEnvironment::StatementVisitor::operator()(HSharpParser::NodeStmtInput *stmt) const {
    error(EExceptionSource::VIRTUAL_ENV,EExceptionReason::NOT_IMPLEMENTED,"input() is not implemented");
}

void HSharpVE::VirtualEnvironment::StatementVisitor::operator()(HSharpParser::NodeStmtPrint *stmt) const {
    parent->StatementVisitor_StatementPrint(stmt);
}

void HSharpVE::VirtualEnvironment::StatementVisitor::operator()(HSharpParser::NodeStmtExit *stmt) const {
    parent->StatementVisitor_StatementExit(stmt);
}

void HSharpVE::VirtualEnvironment::StatementVisitor::operator()(HSharpParser::NodeStmtVar *stmt) const {
    parent->StatementVisitor_StatementVar(stmt);
}

void HSharpVE::VirtualEnvironment::StatementVisitor::operator()(HSharpParser::NodeStmtVarAssign *stmt) const {
    parent->StatementVisitor_StatementVarAssign(stmt);
}

HSharp::ValueInfo HSharpVE::VirtualEnvironment::ExpressionVisitor::operator()(HSharpParser::NodeTerm *term) const {
    return std::visit(parent->vTerm, term->term);
}

HSharp::ValueInfo HSharpVE::VirtualEnvironment::ExpressionVisitor::operator()(const HSharpParser::NodeExpressionStrLit *expr) const {
    auto str = parent->stringAllocator_.allocate(1);
    str = new(str)std::string(expr->str_lit.payload);
    return ValueInfo{
            .type = VariableType::STRING,
            .value = str,
            .line = expr->line,
            .dealloc_required = true
    };
}
HSharp::ValueInfo HSharpVE::VirtualEnvironment::ExpressionVisitor::operator()(HSharpParser::NodeBinExpr *expr) const {
    return std::visit(parent->vBinaryExpression, expr->var);
}
HSharp::ValueInfo HSharpVE::VirtualEnvironment::TermVisitor::operator()(const HSharpParser::NodeTermIdent *term) const {
    if (!parent->is_variable(const_cast<char*>(term->ident.payload.c_str()))){
        std::cerr << "Invalid identifier" << std::endl;
        exit(1);
    }
    return {
            .type = parent->globalScope_.variables.at(term->ident.payload).vtype,
            .value = parent->globalScope_.variables.at(term->ident.payload).value,
            .line = term->line,
            .dealloc_required = false
    };
}
HSharp::ValueInfo HSharpVE::VirtualEnvironment::TermVisitor::operator()(const HSharpParser::NodeTermIntLit *term) const {
    if (!is_number(term->int_lit.payload)) {
        std::cerr << "Expression is not valid integer!" << std::endl;
        exit(1);
    }
    auto num = parent->integerAllocator_.allocate(1);
    *num = std::stol(term->int_lit.payload);
    return {.type = VariableType::INT,
            .value = num,
            .line = term->line,
            .dealloc_required = true};
}
HSharp::ValueInfo HSharpVE::VirtualEnvironment::BinExprVisitor::operator()(const HSharpParser::NodeBinExprAdd *expr) const {
    auto result = parent->integerAllocator_.allocate(1);
    ValueInfo lhs, rhs;
    if ((lhs = std::visit(parent->vExpression_, expr->lhs->expr)).type != VariableType::INT)
        error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "Binary expression evaluation impossible: invalid literal type");
    if ((rhs = std::visit(parent->vExpression_, expr->rhs->expr)).type != VariableType::INT)
        error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "Binary expression evaluation impossible: invalid literal type");
    *result = *static_cast<int64_t*>(lhs.value) + *static_cast<int64_t*>(rhs.value);
    parent->dispose_value(lhs);
    parent->dispose_value(rhs);
    return ValueInfo{.type = VariableType::INT, .value = result, .dealloc_required = true};
}
HSharp::ValueInfo HSharpVE::VirtualEnvironment::BinExprVisitor::operator()(const HSharpParser::NodeBinExprSub *expr) const {
    auto result = parent->integerAllocator_.allocate(1);
    ValueInfo lhs, rhs;
    if ((lhs = std::visit(parent->vExpression_, expr->lhs->expr)).type != VariableType::INT)
        error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "Binary expression evaluation impossible: invalid literal type");
    if ((rhs = std::visit(parent->vExpression_, expr->rhs->expr)).type != VariableType::INT)
        error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "Binary expression evaluation impossible: invalid literal type");
    *result = *static_cast<int64_t*>(lhs.value) - *static_cast<int64_t*>(rhs.value);
    parent->dispose_value(lhs);
    parent->dispose_value(rhs);
    return ValueInfo{.type = VariableType::INT, .value = result, .dealloc_required = true};
}
HSharp::ValueInfo HSharpVE::VirtualEnvironment::BinExprVisitor::operator()(const HSharpParser::NodeBinExprMul *expr) const {
    auto result = parent->integerAllocator_.allocate(1);
    ValueInfo lhs, rhs;
    if ((lhs = std::visit(parent->vExpression_, expr->lhs->expr)).type != VariableType::INT)
        error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "Binary expression evaluation impossible: invalid literal type");
    if ((rhs = std::visit(parent->vExpression_, expr->rhs->expr)).type != VariableType::INT)
        error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "Binary expression evaluation impossible: invalid literal type");
    *result = *static_cast<int64_t*>(lhs.value) * *static_cast<int64_t*>(rhs.value);
    parent->dispose_value(lhs);
    parent->dispose_value(rhs);
    return ValueInfo{.type = VariableType::INT, .value = result, .dealloc_required = true};
}
HSharp::ValueInfo HSharpVE::VirtualEnvironment::BinExprVisitor::operator()(const HSharpParser::NodeBinExprDiv *expr) const {
    auto result = parent->integerAllocator_.allocate(1);
    ValueInfo lhs, rhs;
    if ((lhs = std::visit(parent->vExpression_, expr->lhs->expr)).type != VariableType::INT)
        error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "Binary expression evaluation impossible: invalid literal type");
    if ((rhs = std::visit(parent->vExpression_, expr->rhs->expr)).type != VariableType::INT)
        error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "Binary expression evaluation impossible: invalid literal type");
    *result = *static_cast<int64_t*>(lhs.value) / *static_cast<int64_t*>(rhs.value);
    parent->dispose_value(lhs);
    parent->dispose_value(rhs);
    return ValueInfo{.type = VariableType::INT, .value = result, .dealloc_required = true};
}

void HSharpVE::VirtualEnvironment::delete_var_value(HSharpVE::Variable &variable) {
    switch(variable.vtype){
        case VariableType::INT:
            integerAllocator_.deallocate(static_cast<int64_t*>(variable.value), 1);
            break;
        case VariableType::STRING:
            static_cast<string*>(variable.value)->~string();
            stringAllocator_.deallocate(static_cast<string*>(variable.value), 1);
            break;
        default:
            error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "Cannot delete value: invalid type");
    }
}
void* HSharpVE::VirtualEnvironment::allocate(HSharp::VariableType vtype) {
    switch(vtype){
        case VariableType::INT:
            return integerAllocator_.allocate(1);
        case VariableType::STRING:
            return stringAllocator_.allocate(1);
        default:
            error(EExceptionSource::VIRTUAL_ENV, EExceptionReason::TYPE_ERROR, "Cannot delete value: invalid type");
    }
}
void HSharpVE::VirtualEnvironment::exec_statement(const HSharpParser::NodeStmt* stmt) {
    std::visit(vStatement_, stmt->statement);
}
void HSharpVE::VirtualEnvironment::delete_variables() {
    for (auto pair : globalScope_.variables) {
        switch (pair.second.vtype) {
            case VariableType::INT: integerAllocator_.deallocate(static_cast<int64_t*>(pair.second.value), 1); break;
            case VariableType::STRING: stringAllocator_.deallocate(static_cast<std::string*>(pair.second.value), 1); break;
            default:
                std::printf("Cannot dispose variable %s: unknown type, freeing impossible", pair.first.c_str());
        }
    }
    globalScope_.variables.clear();
}
bool HSharpVE::VirtualEnvironment::is_variable(char* name) {
    auto it = std::find_if(std::begin(globalScope_.variables), std::end(globalScope_.variables), [name](auto&& arg) {
        return !strcmp(arg.first.c_str(), name);
    });
    return it != std::end(globalScope_.variables);
}
void HSharpVE::VirtualEnvironment::dispose_value(ValueInfo& data) {
    if (!data.dealloc_required) return;
    switch (data.type) {
        case VariableType::INT: integerAllocator_.deallocate(static_cast<int64_t*>(data.value), 1); break;
        case VariableType::STRING: stringAllocator_.deallocate(static_cast<std::string*>(data.value), 1); break;
        default: std::terminate();
    }
}
void HSharpVE::VirtualEnvironment::run() {
    globalScope_ = {};
    for (const HSharpParser::NodeStmt* stmt : root_.statements) {
        exec_statement(stmt);
    }
}

bool HSharpVE::VirtualEnvironment::is_number(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

template<typename... Args>
std::string HSharpVE::VirtualEnvironment::vformat_wrapper(const char* format, Args&&... args)
{
    return std::vformat(format, std::make_format_args(args...));
}