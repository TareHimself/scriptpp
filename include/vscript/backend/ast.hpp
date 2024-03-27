#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "deque"

#include "Token.hpp"
#include "vscript/frontend/Object.hpp"


namespace vs::backend
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
    };

    

    struct Node
    {
        ENodeType type = NT_Unknown;
        uint32_t line = 0;
        uint32_t col = 0;

        // May be used for optimization later
        bool isStatic = false;

        Node() = default;

        Node(uint32_t inLine,uint32_t inCol);

        Node(uint32_t inLine,uint32_t inCol,ENodeType inType);
        
        virtual ~Node() = default;
    };

    struct HasLeft : Node
    {
        std::shared_ptr<Node> left;
        HasLeft(uint32_t inLine,uint32_t inCol,const std::shared_ptr<Node>& inLeft);
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
        
        BinaryOpNode(uint32_t inLine,uint32_t inCol,const std::shared_ptr<Node>& inLeft, const std::shared_ptr<Node>& inRight, const EBinaryOp& inOp);
        

        BinaryOpNode(uint32_t inLine,uint32_t inCol,const std::shared_ptr<Node>& inLeft, const std::shared_ptr<Node>& inRight, const ETokenType& inOp);
        
    };
    
    struct LiteralNode : Node
    {
        std::string value;

        LiteralNode(uint32_t inLine,uint32_t inCol,const std::string& inValue,const ENodeType& inType);
    };

    struct ListLiteralNode : Node
    {
        std::vector<std::shared_ptr<Node>> values;

        ListLiteralNode(uint32_t inLine,uint32_t inCol,const std::vector<std::shared_ptr<Node>>& inValues);
    };

    struct CreateAndAssignNode : Node
    {
        std::string name;
        std::shared_ptr<Node> value;
        CreateAndAssignNode(uint32_t inLine,uint32_t inCol,const std::string& inName,const std::shared_ptr<Node>& inValue);
        
    };

    struct AssignNode : HasLeft
    {
        std::shared_ptr<Node> value;
        AssignNode(uint32_t inLine,uint32_t inCol,const  std::shared_ptr<Node>& inTarget,const std::shared_ptr<Node>& inValue);
        
    };

    struct VariableNode : Node
    {
        std::string value;

        VariableNode(uint32_t inLine,uint32_t inCol,const std::string& inValue);
        
    };

    struct ScopeNode : Node
    {
        std::vector<std::shared_ptr<Node>> statements;

        ScopeNode(uint32_t inLine,uint32_t inCol,const std::vector<std::shared_ptr<Node>>& inStatements = {});
        
    };

    struct AccessNode : HasLeft
    {
        std::shared_ptr<Node> right;

        AccessNode(uint32_t inLine,uint32_t inCol,const std::shared_ptr<Node>& inLeft,const std::shared_ptr<Node>& inRight);
        
    };

    struct AccessNode2 : HasLeft
    {
        std::shared_ptr<Node> within;

        AccessNode2(uint32_t inLine,uint32_t inCol,const std::shared_ptr<Node>& inLeft,const std::shared_ptr<Node>& inWithin);
        
    };
    
    struct WhenNode : Node
    {
        
        struct Branch
        {
            std::shared_ptr<Node> expression;
            std::shared_ptr<Node> statement;
        };
        
        std::vector<Branch> branches;

        WhenNode(uint32_t inLine,uint32_t inCol,const std::vector<Branch>& inBranches);
        
    };

    struct ReturnNode : Node
    {
        std::shared_ptr<Node> expression;

        ReturnNode(uint32_t inLine,uint32_t inCol,const std::shared_ptr<Node>& inExpression);
        
    };

    struct FunctionNode : Node
    {
        std::string name;
        std::vector<std::string> args;
        std::shared_ptr<ScopeNode> body;

        FunctionNode(uint32_t inLine,uint32_t inCol,const std::string& inName, const std::vector<std::string>& inArgs,const std::shared_ptr<ScopeNode>& inBody);
        
    };

    struct CallNode : HasLeft
    {
        std::vector<std::shared_ptr<Node>> args;

        CallNode(uint32_t inLine,uint32_t inCol,const std::shared_ptr<Node>& inLeft,const std::vector<std::shared_ptr<Node>>& inArgs);
        
    };

    struct ForNode : Node
    {
        std::shared_ptr<Node> init;
        std::shared_ptr<Node> condition;
        std::shared_ptr<Node> update;
        std::shared_ptr<ScopeNode> body;

        ForNode(uint32_t inLine,uint32_t inCol,const std::shared_ptr<Node>& inInit,const std::shared_ptr<Node>& inCondition,const std::shared_ptr<Node>& inUpdate,const std::shared_ptr<ScopeNode>& inBody);
        
    };

    struct WhileNode : Node
    {
        std::shared_ptr<Node> condition;
        std::shared_ptr<ScopeNode> body;

        WhileNode(uint32_t inLine,uint32_t inCol,const std::shared_ptr<Node>& inCondition,const std::shared_ptr<ScopeNode>& inBody);
        
    };

    struct PrototypeNode : Node
    {
        std::string id;
        std::vector<std::string> parents;
        std::shared_ptr<ScopeNode> scope;

        PrototypeNode(uint32_t inLine,uint32_t inCol,const std::string& inId,const std::vector<std::string>& inParents,const std::shared_ptr<ScopeNode>& inScope);
        
    };
    

    struct ModuleNode : Node
    {
        std::vector<std::shared_ptr<Node>> statements;

        ModuleNode(uint32_t inLine,uint32_t inCol,const std::vector<std::shared_ptr<Node>>& inStatements);
        
    };

    void parseFunctionBody(std::list<Token> &tokens,std::vector<std::shared_ptr<Node>>& body);

    std::shared_ptr<Node> parseParen(std::list<Token> &tokens);
    
    std::shared_ptr<ListLiteralNode> parseList(std::list<Token> &tokens);
    
    std::shared_ptr<Node> parsePrimary(std::list<Token> &tokens);

    std::shared_ptr<Node> parseAccessors(std::list<Token> &tokens);
    
    std::shared_ptr<Node> parseMultiplicativeExpression(std::list<Token> &tokens);

    std::shared_ptr<Node> parseAdditiveExpression(std::list<Token> &tokens);

    std::shared_ptr<Node> parseComparisonExpression(std::list<Token> &tokens);

    std::shared_ptr<Node> parseLogicalExpression(std::list<Token> &tokens);

    std::shared_ptr<Node> parseAssignmentExpression(std::list<Token> &tokens);
    
    std::shared_ptr<Node> parseExpression(std::list<Token> &tokens);

    std::shared_ptr<ReturnNode> parseReturn(std::list<Token> &tokens);

    std::shared_ptr<ForNode> parseFor(std::list<Token> &tokens);

    std::shared_ptr<WhileNode> parseWhile(std::list<Token> &tokens);
    
    std::shared_ptr<Node> parseStatement(std::list<Token> &tokens);
    
    std::shared_ptr<FunctionNode> parseFunction(std::list<Token> &tokens);
    

    std::vector<std::shared_ptr<Node>> parseCallArguments(std::list<Token> &tokens);
    
    std::shared_ptr<WhenNode> parseWhen(std::list<Token> &tokens);

    std::shared_ptr<WhenNode> parseWhenStatement(std::list<Token> &tokens);
    
    std::shared_ptr<PrototypeNode> parseClass(std::list<Token>& tokens);

    void getStatementTokens(std::list<Token>& statement, std::list<Token>& tokens);
    // Gets tokens till end (scope aware)
    void getTokensTill(std::list<Token>& result,std::list<Token>& tokens,const std::function<bool(const Token&,int)>& evaluator,int initialScope = 0,bool popEnd = true);
    void getTokensTill(std::list<Token>& result,std::list<Token>& tokens,const std::set<ETokenType>& ends,int initialScope = 0,bool popEnd = true);
    std::shared_ptr<ScopeNode> parseScope(std::list<Token>& tokens);
    std::shared_ptr<ModuleNode> parse(std::list<Token> tokens);
}
