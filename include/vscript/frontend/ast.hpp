#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "deque"

#include "Token.hpp"
#include "vscript/runtime/Object.hpp"


namespace vs::frontend
{
    enum ENodeType
    {
        NT_Unknown,
        NT_BinaryOp,
        NT_StringLiteral,
        NT_NumericLiteral,
        NT_NullLiteral,
        NT_ListLiteral,
        NT_Function,
        NT_Statement,
        NT_Module,
        NT_Return,
        NT_Variable,
        NT_CreateAndAssign,
        NT_Assign,
        NT_Call,
        NT_BooleanLiteral,
        NT_When,
        NT_Scope,
        NT_For,
        NT_While,
        NT_Break,
        NT_Continue,
        NT_Class,
        NT_Access,
        NT_Access2,
        NT_Throw,
        NT_TryCatch
    };

    

    struct Node
    {
        ENodeType type = NT_Unknown;
        
        TokenDebugInfo debugInfo;

        // May be used for optimization later
        bool isStatic = false;

        Node() = default;

        Node(const TokenDebugInfo& inDebugInfo);

        Node(const TokenDebugInfo& inDebugInfo,ENodeType inType);
        
        virtual ~Node() = default;
    };

    struct HasLeft : Node
    {
        std::shared_ptr<Node> left;
        HasLeft(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inLeft);
    };

    
    struct BinaryOpNode : HasLeft
    {
        enum EBinaryOp
        {
            BO_Divide,
            BO_Multiply,
            BO_Add,
            BO_Subtract,
            BO_Mod,
            BO_And,
            BO_Or,
            BO_Not,
            BO_Equal,
            BO_NotEqual,
            BO_Less,
            BO_LessEqual,
            BO_Greater,
            BO_GreaterEqual,
            
        };
        
        std::shared_ptr<Node> right;
        EBinaryOp op;

        static EBinaryOp TokenTypeToBinaryOp(const ETokenType& tokType);
        
        BinaryOpNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inLeft, const std::shared_ptr<Node>& inRight, const EBinaryOp& inOp);
        

        BinaryOpNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inLeft, const std::shared_ptr<Node>& inRight, const ETokenType& inOp);
        
    };
    
    struct LiteralNode : Node
    {
        std::string value;

        LiteralNode(const TokenDebugInfo& inDebugInfo,const std::string& inValue,const ENodeType& inType);
    };

    struct ListLiteralNode : Node
    {
        std::vector<std::shared_ptr<Node>> values;

        ListLiteralNode(const TokenDebugInfo& inDebugInfo,const std::vector<std::shared_ptr<Node>>& inValues);
    };

    struct CreateAndAssignNode : Node
    {
        std::string name;
        std::shared_ptr<Node> value;
        CreateAndAssignNode(const TokenDebugInfo& inDebugInfo,const std::string& inName,const std::shared_ptr<Node>& inValue);
        
    };

    struct AssignNode : HasLeft
    {
        std::shared_ptr<Node> value;
        AssignNode(const TokenDebugInfo& inDebugInfo,const  std::shared_ptr<Node>& inTarget,const std::shared_ptr<Node>& inValue);
        
    };

    struct VariableNode : Node
    {
        std::string value;

        VariableNode(const TokenDebugInfo& inDebugInfo,const std::string& inValue);
        
    };

    struct ScopeNode : Node
    {
        std::vector<std::shared_ptr<Node>> statements;

        ScopeNode(const TokenDebugInfo& inDebugInfo,const std::vector<std::shared_ptr<Node>>& inStatements = {});
        
    };

    struct AccessNode : HasLeft
    {
        std::shared_ptr<Node> right;

        AccessNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inLeft,const std::shared_ptr<Node>& inRight);
        
    };

    struct AccessNode2 : HasLeft
    {
        std::shared_ptr<Node> within;

        AccessNode2(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inLeft,const std::shared_ptr<Node>& inWithin);
        
    };
    
    struct WhenNode : Node
    {
        
        struct Branch
        {
            std::shared_ptr<Node> expression;
            std::shared_ptr<Node> statement;
        };
        
        std::vector<Branch> branches;

        WhenNode(const TokenDebugInfo& inDebugInfo,const std::vector<Branch>& inBranches);
        
    };

    struct ReturnNode : Node
    {
        std::shared_ptr<Node> expression;

        ReturnNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inExpression);
    };

    struct ThrowNode : Node
    {
        std::shared_ptr<Node> expression;

        ThrowNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inExpression);
    };

    struct TryCatchNode : Node
    {
        std::shared_ptr<ScopeNode> tryScope;
        std::shared_ptr<ScopeNode> catchScope;
        std::string catchArgumentName;

        TryCatchNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<ScopeNode>& inTryScope,const std::shared_ptr<ScopeNode>& inCatchScope,const std::string& inCatchArgName);
    };

    struct FunctionNode : Node
    {
        std::string name;
        std::vector<std::string> args;
        std::shared_ptr<ScopeNode> body;

        FunctionNode(const TokenDebugInfo& inDebugInfo,const std::string& inName, const std::vector<std::string>& inArgs,const std::shared_ptr<ScopeNode>& inBody);
        
    };

    struct CallNode : HasLeft
    {
        std::vector<std::shared_ptr<Node>> args;

        CallNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inLeft,const std::vector<std::shared_ptr<Node>>& inArgs);
        
    };

    struct ForNode : Node
    {
        std::shared_ptr<Node> init;
        std::shared_ptr<Node> condition;
        std::shared_ptr<Node> update;
        std::shared_ptr<ScopeNode> body;

        ForNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inInit,const std::shared_ptr<Node>& inCondition,const std::shared_ptr<Node>& inUpdate,const std::shared_ptr<ScopeNode>& inBody);
        
    };

    struct WhileNode : Node
    {
        std::shared_ptr<Node> condition;
        std::shared_ptr<ScopeNode> body;

        WhileNode(const TokenDebugInfo& inDebugInfo,const std::shared_ptr<Node>& inCondition,const std::shared_ptr<ScopeNode>& inBody);
        
    };

    struct PrototypeNode : Node
    {
        std::string id;
        std::vector<std::string> parents;
        std::shared_ptr<ScopeNode> scope;

        PrototypeNode(const TokenDebugInfo& inDebugInfo,const std::string& inId,const std::vector<std::string>& inParents,const std::shared_ptr<ScopeNode>& inScope);
        
    };
    

    struct ModuleNode : Node
    {
        std::vector<std::shared_ptr<Node>> statements;

        ModuleNode(const TokenDebugInfo& inDebugInfo,const std::vector<std::shared_ptr<Node>>& inStatements);
        
    };

    void parseFunctionBody(Tokenized &tokens,std::vector<std::shared_ptr<Node>>& body);

    std::shared_ptr<Node> parseParen(Tokenized &tokens);
    
    std::shared_ptr<ListLiteralNode> parseList(Tokenized &tokens);
    
    std::shared_ptr<Node> parsePrimary(Tokenized &tokens);

    std::shared_ptr<Node> parseAccessors(Tokenized &tokens);
    
    std::shared_ptr<Node> parseMultiplicativeExpression(Tokenized &tokens);

    std::shared_ptr<Node> parseAdditiveExpression(Tokenized &tokens);

    std::shared_ptr<Node> parseComparisonExpression(Tokenized &tokens);

    std::shared_ptr<Node> parseLogicalExpression(Tokenized &tokens);

    std::shared_ptr<Node> parseAssignmentExpression(Tokenized &tokens);
    
    std::shared_ptr<Node> parseExpression(Tokenized &tokens);

    std::shared_ptr<ReturnNode> parseReturn(Tokenized &tokens);

    std::shared_ptr<ForNode> parseFor(Tokenized &tokens);

    std::shared_ptr<WhileNode> parseWhile(Tokenized &tokens);

    std::shared_ptr<TryCatchNode> parseTryCatch(Tokenized& tokens);
    
    std::shared_ptr<Node> parseStatement(Tokenized &tokens);
    
    std::shared_ptr<FunctionNode> parseFunction(Tokenized &tokens);
    

    std::vector<std::shared_ptr<Node>> parseCallArguments(Tokenized &tokens);
    
    std::shared_ptr<WhenNode> parseWhen(Tokenized &tokens);

    std::shared_ptr<WhenNode> parseWhenStatement(Tokenized &tokens);
    
    std::shared_ptr<PrototypeNode> parseClass(Tokenized& tokens);

    void getStatementTokens(Tokenized& statement, Tokenized& tokens);
    // Gets tokens till end (scope aware)
    void getTokensTill(Tokenized& result,Tokenized& tokens,const std::function<bool(const Token&,int)>& evaluator,int initialScope = 0,bool popEnd = true);
    void getTokensTill(Tokenized& result,Tokenized& tokens,const std::set<ETokenType>& ends,int initialScope = 0,bool popEnd = true);
    std::shared_ptr<ScopeNode> parseScope(Tokenized& tokens);
    std::shared_ptr<ModuleNode> parse(Tokenized tokens);
}
