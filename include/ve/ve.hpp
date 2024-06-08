#pragma once

#include <cstddef>
#include <memory>
#include <memory_resource>
#include <unordered_map>

#include <visitors.hpp>
#include <parser/parser.hpp>
#include <ve/exceptions.hpp>


namespace HSharpVE {

    /* Nodes */
    using HSharpParser::NodeStmtInput;
    using HSharpParser::NodeStmtExit;
    using HSharpParser::NodeStmtPrint;
    using HSharpParser::NodeStmtVar;
    using HSharpParser::NodeStmtVarAssign;
    using HSharpParser::NodeExpressionStrLit;
    using HSharp::VariableType;
    using HSharp::ValueInfo;

    struct Variable {
        VariableType vtype;
        void* value;
    };

    struct Scope {
        std::unordered_map<std::string, Variable> variables;
    };


    class VirtualEnvironment{
    public:
        VirtualEnvironment(HSharpParser::NodeProgram root, std::vector<std::string>& lines, const bool verbose);
        ~VirtualEnvironment();

        void run();

    private:
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

        template <typename... Args>
        std::string vformat_wrapper(const char* format, Args&&... args);
        static bool is_number(const std::string& s);

    private:
        class StatementVisitor : public HSharp::IStatementVisitor {
        public:
            explicit StatementVisitor(VirtualEnvironment* parent) : parent(parent) {}
            void operator()(NodeStmtInput* stmt) const override;
            void operator()(NodeStmtPrint* stmt) const override;
            void operator()(NodeStmtExit* stmt) const override;
            void operator()(NodeStmtVar* stmt) const override;
            void operator()(NodeStmtVarAssign* stmt) const override;

        private:
            VirtualEnvironment* parent;
        };

        class ExpressionVisitor : HSharp::IExpressionVisitor {
        public:
            explicit ExpressionVisitor(VirtualEnvironment* parent) : parent(parent) {}
            ValueInfo operator()(HSharpParser::NodeTerm* term) const override;
            ValueInfo operator()(const HSharpParser::NodeExpressionStrLit* expr) const override;
            ValueInfo operator()(HSharpParser::NodeBinExpr* expr) const override;

        private:
            VirtualEnvironment* parent;
        };

        class TermVisitor {
        public:
            explicit TermVisitor(VirtualEnvironment* parent) : parent(parent) {}
            ValueInfo operator()(const HSharpParser::NodeTermIntLit* term) const;
            ValueInfo operator()(const HSharpParser::NodeTermIdent* term) const;

        private:
            VirtualEnvironment* parent;
        };

        class BinExprVisitor {
        public:
            explicit BinExprVisitor(VirtualEnvironment* parent) : parent(parent){}
            ValueInfo operator()(const HSharpParser::NodeBinExprAdd* expr) const;
            ValueInfo operator()(const HSharpParser::NodeBinExprSub* expr) const;
            ValueInfo operator()(const HSharpParser::NodeBinExprMul* expr) const;
            ValueInfo operator()(const HSharpParser::NodeBinExprDiv* expr) const;

        private:
            VirtualEnvironment* parent;
        };

        HSharpParser::NodeProgram root_;
        std::vector<std::string>& lines_;
        Scope globalScope_;

        ExpressionVisitor vExpression_      {this};
        StatementVisitor vStatement_        {this};
        TermVisitor vTerm                   {this};
        BinExprVisitor vBinaryExpression    {this};

        bool verbose;

        std::pmr::unsynchronized_pool_resource resource_;
        std::pmr::polymorphic_allocator<std::string> stringAllocator_ {&resource_};
        std::pmr::polymorphic_allocator<std::int64_t> integerAllocator_ {&resource_};
    };
}