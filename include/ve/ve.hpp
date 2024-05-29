#pragma once

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>

#include <parser/parser.hpp>
#include <ve/exceptions.hpp>

/* Nodes */
using HSharpParser::NodeStmtInput;
using HSharpParser::NodeStmtExit;
using HSharpParser::NodeStmtPrint;
using HSharpParser::NodeStmtVar;
using HSharpParser::NodeStmtVarAssign;
using HSharpParser::NodeExpressionStrLit;

namespace HSharpVE {
    /* Exception handling */
    enum class ExceptionSource{
        Tokenizer,
        Parser,
        VirtualEnv
    };
    enum class ExceptionType{
        SyntaxError,
        TypeError,
        InvalidAssign,
        NotImplemented,
        ConversionError
    };

    enum class VariableType {
        INT,
        STRING
    };
    struct Variable {
        VariableType vtype;
        void* value;
    };
    struct Scope {
        std::unordered_map<std::string, Variable> variables;
    };
    struct ValueInfo {
        VariableType type;
        void* value;
        bool dealloc_required;
    };

    class VirtualEnvironment{
    private:
        struct StatementVisitor {
        private:
            VirtualEnvironment* parent;
        public:
            explicit StatementVisitor(VirtualEnvironment* parent) : parent(parent) {}
            void operator()(NodeStmtInput* stmt) const;
            void operator()(NodeStmtPrint* stmt) const;
            void operator()(NodeStmtExit* stmt) const;
            void operator()(NodeStmtVar* stmt) const;
            void operator()(NodeStmtVarAssign* stmt) const;
        };
        struct ExpressionVisitor {
        private:
            VirtualEnvironment* parent;
        public:
            explicit ExpressionVisitor(VirtualEnvironment* parent) : parent(parent) {}
            ValueInfo operator()(HSharpParser::NodeTerm* term) const;
            ValueInfo operator()(const HSharpParser::NodeExpressionStrLit* expr) const;
            ValueInfo operator()(HSharpParser::NodeBinExpr* expr) const;
        };
        struct TermVisitor {
        private:
            VirtualEnvironment* parent;
        public:
            explicit TermVisitor(VirtualEnvironment* parent) : parent(parent) {}
            ValueInfo operator()(const HSharpParser::NodeTermIntLit* term) const;
            ValueInfo operator()(const HSharpParser::NodeTermIdent* term) const;
        };
        struct BinExprVisitor {
        private:
            VirtualEnvironment* parent;
        public:
            explicit BinExprVisitor(VirtualEnvironment* parent) : parent(parent){}
            ValueInfo operator()(const HSharpParser::NodeBinExprAdd* expr) const;
            ValueInfo operator()(const HSharpParser::NodeBinExprSub* expr) const;
            ValueInfo operator()(const HSharpParser::NodeBinExprMul* expr) const;
            ValueInfo operator()(const HSharpParser::NodeBinExprDiv* expr) const;
        };
        HSharpParser::NodeProgram root;
        Scope global_scope;
        boost::object_pool<std::int64_t> integers_pool;
        boost::pool<> strings_pool;
        ExpressionVisitor exprvisitor{this};
        StatementVisitor stmtvisitor{this};
        TermVisitor termvisitor{this};
        BinExprVisitor binexprvisitor{this};
        bool verbose;

        void StatementVisitor_StatementPrint(HSharpParser::NodeStmtPrint* stmt);
        void StatementVisitor_StatementExit(HSharpParser::NodeStmtExit* stmt);
        void StatementVisitor_StatementVar(HSharpParser::NodeStmtVar* stmt);
        void StatementVisitor_StatementVarAssign(HSharpParser::NodeStmtVarAssign* stmt);
        ValueInfo ExpressionVisitor_ExprStrLit(NodeExpressionStrLit* expr);
        ValueInfo ExpressionVisitor_ExprIntLit(HSharpParser::NodeTermIntLit* expr);
        ValueInfo ExpressionVisitor_ExprIdent(HSharpParser::NodeTermIdent* expr) const;
        ValueInfo ExpressionVisitor_BinExpr(HSharpParser::NodeBinExpr* expr) const;

        void exec_statement(const HSharpParser::NodeStmt* stmt);

        void delete_variables();
        bool is_variable(char* name);
        void dispose_value(ValueInfo& data);
        void delete_var_value(Variable& variable);
        void* allocate(VariableType vtype);

        static bool is_number(const std::string& s);
    public:
        explicit VirtualEnvironment(HSharpParser::NodeProgram root, const bool verbose)
            : root(std::move(root)),
              integers_pool(16),
              strings_pool(sizeof(std::string)),
              verbose(verbose){
        }
        ~VirtualEnvironment() {
            delete_variables();
        }
        void run();
    };
}