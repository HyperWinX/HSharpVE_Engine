#pragma once

#include <parser/nodes.hpp>

using namespace HSharpParser;

namespace HSharp{
    struct ValueInfo{};

    class IStatementVisitor{
    public:
        virtual void operator()(NodeStmtInput* stmt) const;
        virtual void operator()(NodeStmtPrint* stmt) const;
        virtual void operator()(NodeStmtExit* stmt) const;
        virtual void operator()(NodeStmtVar* stmt) const;
        virtual void operator()(NodeStmtVarAssign* stmt) const;
        virtual ~IStatementVisitor();
    };

    struct IExpressionVisitor {
    public:
        virtual ValueInfo operator()(HSharpParser::NodeTerm* term) const;
        virtual ValueInfo operator()(const HSharpParser::NodeExpressionStrLit* expr) const;
        virtual ValueInfo operator()(HSharpParser::NodeBinExpr* expr) const;
    };

    struct ITermVisitor {
    public:
        virtual ValueInfo operator()(const HSharpParser::NodeTermIntLit* term) const;
        virtual ValueInfo operator()(const HSharpParser::NodeTermIdent* term) const;
    };

    struct IBinExprVisitor {
    public:
        virtual ValueInfo operator()(const HSharpParser::NodeBinExprAdd* expr) const;
        virtual ValueInfo operator()(const HSharpParser::NodeBinExprSub* expr) const;
        virtual ValueInfo operator()(const HSharpParser::NodeBinExprMul* expr) const;
        virtual ValueInfo operator()(const HSharpParser::NodeBinExprDiv* expr) const;
    };
}