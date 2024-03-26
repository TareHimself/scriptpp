#include "vscript/backend/ast.hpp"

#include <optional>
#include <stdexcept>

namespace vs::backend
{
    Node::Node(uint32_t inLine, uint32_t inCol)
    {
        line = inLine;
        col = inCol;
    }

    Node::Node(uint32_t inLine, uint32_t inCol, ENodeType inType) : Node(inLine,inCol)
    {
        type = inType;
    }

    HasLeft::HasLeft(uint32_t inLine, uint32_t inCol, const std::shared_ptr<Node>& inLeft) : Node(inLine,inCol)
    {
        left = inLeft;
    }
    
    BinaryOpNode::EBinaryOp BinaryOpNode::TokenTypeToBinaryOp(const ETokenType& tokType)
    {
        switch (tokType)
        {
        case TT_OpAdd:
            return  BO_Add;
        case TT_OpSubtract:
            return  BO_Subtract;
        case TT_OpDivide:
            return  BO_Divide;
        case  TT_OpMultiply:
            return BO_Multiply;
        case  TT_OpMod:
            return BO_Mod;
        case  TT_OpAnd:
            return BO_And;
        case  TT_OpOr:
            return BO_Or;
        case  TT_OpNot:
            return BO_Not;
        case  TT_OpEqual:
            return BO_Equal;
        case  TT_OpNotEqual:
            return BO_NotEqual;
        case  TT_OpLess:
            return BO_Less;
        case  TT_OpLessEqual:
            return BO_LessEqual;
        case  TT_OpGreater:
            return BO_Greater;
        case  TT_OpGreaterEqual:
            return BO_GreaterEqual;
        }

        return BO_Add;
    }

    BinaryOpNode::BinaryOpNode(uint32_t inLine, uint32_t inCol, const std::shared_ptr<Node>& inLeft,
                               const std::shared_ptr<Node>& inRight, const EBinaryOp& inOp) : HasLeft(inLine,inCol,inLeft)
    {
        right = inRight;
        op = inOp;
        type = ENodeType::NT_BinaryOp;
    }

    BinaryOpNode::BinaryOpNode(uint32_t inLine, uint32_t inCol, const std::shared_ptr<Node>& inLeft,
                               const std::shared_ptr<Node>& inRight, const ETokenType& inOp) : HasLeft(inLine,inCol,inLeft)
    {
        right = inRight;
        type = ENodeType::NT_BinaryOp;
            
        op = TokenTypeToBinaryOp(inOp);
    }

    LiteralNode::LiteralNode(uint32_t inLine, uint32_t inCol, const std::string& inValue,const ENodeType& inType) : Node(inLine,inCol)
    {
        value = inValue;
        type = inType;
    }

    CreateAndAssignNode::CreateAndAssignNode(uint32_t inLine, uint32_t inCol, const std::string& inName,
                                             const std::shared_ptr<Node>& inValue) : Node(inLine,inCol)
    {
        name = inName;
        value = inValue;
        type = ENodeType::NT_CreateAndAssign;
    }

    AssignNode::AssignNode(uint32_t inLine, uint32_t inCol, const std::shared_ptr<Node>& inTarget,
                           const std::shared_ptr<Node>& inValue) : HasLeft(inLine,inCol,inTarget)
    {
        value = inValue;
        type = ENodeType::NT_Assign;
    }

    VariableNode::VariableNode(uint32_t inLine, uint32_t inCol, const std::string& inValue) : Node(inLine,inCol)
    {
        value = inValue;
        type = ENodeType::NT_Variable;
    }

    ScopeNode::ScopeNode(uint32_t inLine, uint32_t inCol, const std::vector<std::shared_ptr<Node>>& inStatements) : Node(inLine,inCol)
    {
        statements = inStatements;
        type = NT_Scope;
    }

    AccessNode::AccessNode(uint32_t inLine, uint32_t inCol, const std::shared_ptr<Node>& inLeft,
                           const std::shared_ptr<Node>& inRight) : HasLeft(inLine,inCol,inLeft)
    {
        right = inRight;
        type = NT_Access;
    }

    AccessNode2::AccessNode2(uint32_t inLine, uint32_t inCol, const std::shared_ptr<Node>& inLeft,
                             const std::shared_ptr<Node>& inWithin) : HasLeft(inLine,inCol,inLeft)
    {
        within = inWithin;
        type = NT_Access2;
    }

    WhenNode::WhenNode(uint32_t inLine, uint32_t inCol, const std::vector<Branch>& inBranches) : Node(inLine,inCol)
    {
        branches = inBranches;
        type = ENodeType::NT_When;
    }

    ReturnNode::ReturnNode(uint32_t inLine, uint32_t inCol, const std::shared_ptr<Node>& inExpression) : Node(inLine,inCol)
    {
        expression = inExpression;
        type = ENodeType::NT_Return;
    }

    FunctionNode::FunctionNode(uint32_t inLine, uint32_t inCol, const std::string& inName,
                               const std::vector<std::string>& inArgs, const std::shared_ptr<ScopeNode>& inBody) : Node(inLine,inCol)
    {
        name = inName;
        args = inArgs;
        body = inBody;
        type = ENodeType::NT_Function;
    }

    CallNode::CallNode(uint32_t inLine, uint32_t inCol, const std::shared_ptr<Node>& inLeft,
                       const std::vector<std::shared_ptr<Node>>& inArgs) : HasLeft(inLine,inCol,inLeft)
    {
        args = inArgs;
        type = ENodeType::NT_Call;
    }

    ForNode::ForNode(uint32_t inLine, uint32_t inCol, const std::shared_ptr<Node>& inInit,
                     const std::shared_ptr<Node>& inCondition, const std::shared_ptr<Node>& inUpdate,
                     const std::shared_ptr<ScopeNode>& inBody) : Node(inLine,inCol)
    {
        init = inInit;
        condition = inCondition;
        update = inUpdate;
        body = inBody;
        type = NT_For;
    }

    WhileNode::WhileNode(uint32_t inLine, uint32_t inCol, const std::shared_ptr<Node>& inCondition,
                         const std::shared_ptr<ScopeNode>& inBody) : Node(inLine,inCol)
    {
        condition = inCondition;
        body = inBody;
        type = ENodeType::NT_While;
    }

    PrototypeNode::PrototypeNode(uint32_t inLine, uint32_t inCol, const std::string& inId,
                                 const std::vector<std::string>& inParents, const std::shared_ptr<ScopeNode>& inScope) : Node(inLine,inCol)
    {
        id = inId;
        parents = inParents;
        scope = inScope;
        type = NT_Class;
    }

    ModuleNode::ModuleNode(uint32_t inLine, uint32_t inCol, const std::vector<std::shared_ptr<Node>>& inStatements) : Node(inLine,inCol)
    {
        statements = inStatements;
        type = ENodeType::NT_Module;
    }

    void parseFunctionBody(std::list<Token>& tokens, std::vector<std::shared_ptr<Node>>& body)
    {
        tokens.pop_front(); // should be a {
        std::list<Token> statement;
        size_t scope = 1;
        while (scope != 0)
        {
            const auto tok = tokens.front();

            switch (tok.type)
            {
            case TT_Function:
                body.push_back(parseFunction(tokens));
                break;
            case TT_StatementEnd:
                tokens.pop_front();
                body.push_back(parseStatement(statement));
                statement.clear();
                
                continue;
            case TT_OpenBrace:
                scope++;
                break;
            case TT_CloseBrace:
                scope--;
                if (scope == 0)
                {
                    tokens.pop_front();
                    return;
                }
                break;
            default:
                statement.push_back(tok);
            }
            tokens.pop_front();
        }
    }

    std::shared_ptr<Node> parseParen(std::list<Token>& tokens)
    {
        tokens.pop_front();
        std::list<Token> inParen;
        getTokensTill(inParen,tokens,{TT_CloseParen},1);
        

        return parseExpression(inParen);
    }

    // std::shared_ptr<Node> parseAccess2(std::list<Token>& tokens)
    // {
    //     auto left= parsePrimary(tokens);
    //
    //     while (!tokens.empty() && tokens.front().type == TT_OpenBracket)
    //     {
    //         auto token = tokens.front();
    //         tokens.pop_front();
    //         std::list<Token> within;
    //         getTokensTill(within,tokens,{TT_CloseBracket},1);
    //         auto right = parseExpression(within);
    //         left = std::make_shared<AccessNode2>(token.line,token.col,left, right);
    //     }
    //
    //     return left;
    // }
    //
    // std::shared_ptr<Node> parseAccess(std::list<Token>& tokens)
    // {
    //     auto left= parseAccess2(tokens);
    //
    //     while (!tokens.empty() && tokens.front().type == TT_Access)
    //     {
    //         auto token = tokens.front();
    //         tokens.pop_front();
    //         auto right = parseAccess2(tokens);
    //         left = std::make_shared<AccessNode>(token.line,token.col,left, right);
    //     }
    //
    //     return left;
    // }

    std::shared_ptr<Node> parsePrimary(std::list<Token>& tokens)
    {
        if(tokens.empty())
        {
            throw std::runtime_error("Expected token");
        }
        
        auto tok = tokens.front();
        
        switch (tok.type)
        {
        case TT_Identifier:
            tokens.pop_front();
            return std::make_shared<VariableNode>(tok.line,tok.col,tok.value);
        case TT_NumericLiteral:
            tokens.pop_front();
            return std::make_shared<LiteralNode>(tok.line,tok.col,tok.value,NT_NumericLiteral);
        case TT_StringLiteral:
            tokens.pop_front();
            return std::make_shared<LiteralNode>(tok.line,tok.col,tok.value,NT_StringLiteral);
        case TT_BooleanLiteral:
            tokens.pop_front();
            return std::make_shared<LiteralNode>(tok.line,tok.col,tok.value,NT_BooleanLiteral);
        case TT_OpenParen:
            return parseParen(tokens);
        case TT_OpenBrace:
                return parseScope(tokens);
        case TT_When:
            return parseWhen(tokens);
        case TT_Function:
            return parseFunction(tokens);
        case TT_Break:
                return std::make_shared<Node>(tok.line,tok.col,NT_Break);
        case TT_Continue:
                return std::make_shared<Node>(tok.line,tok.col,NT_Continue);
        case TT_Null:
            return std::make_shared<LiteralNode>(tok.line,tok.col,tok.value,NT_NullLiteral);
        default:
            throw std::runtime_error("Unknown primary token");
        }
    }

    std::shared_ptr<Node> parseAccessors(std::list<Token>& tokens)
    {
        // auto left= parseAccess(tokens);
        //
        // while (!tokens.empty() && tokens.front().type == TT_CallBegin)
        // {
        //     auto token = tokens.front();
        //     tokens.pop_front();
        //     auto right = parseCallArguments(tokens);
        //     left = std::make_shared<CallNode>(token.line,token.col,left, right);
        // }
        //
        // return left;

        auto left= parsePrimary(tokens);
        
        while (!tokens.empty() && (tokens.front().type == TT_CallBegin || tokens.front().type == TT_Access || tokens.front().type == TT_OpenBracket))
        {
            switch (tokens.front().type)
            {
            case TT_CallBegin:
                {
                    auto token = tokens.front();
                    tokens.pop_front();
                    auto right = parseCallArguments(tokens);
                    left = std::make_shared<CallNode>(token.line,token.col,left, right);
                }
                break;
            case TT_Access:
                {
                    auto token = tokens.front();
                    tokens.pop_front();
                    auto right = parsePrimary(tokens);
                    left = std::make_shared<AccessNode>(token.line,token.col,left, right);
                }
                break;
            case TT_OpenBracket:
                {
                    auto token = tokens.front();
                    tokens.pop_front();
                    std::list<Token> within;
                    getTokensTill(within,tokens,{TT_CloseBracket},1);
                    auto right = parseExpression(within);
                    left = std::make_shared<AccessNode2>(token.line,token.col,left, right);
                }
            }
        }

        return left;
    }

    std::shared_ptr<Node> parseMultiplicativeExpression(std::list<Token>& tokens)
    {
        auto left = parseAccessors(tokens);

        while (!tokens.empty() && (tokens.front().type == TT_OpMultiply || tokens.front().type == TT_OpDivide || tokens.
            front().type == TT_OpMod))
        {
            auto token = tokens.front();
            tokens.pop_front();
            auto right = parseAccessors(tokens);
            left = std::make_shared<BinaryOpNode>(token.line,token.col,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseAdditiveExpression(std::list<Token>& tokens)
    {
        auto left = parseMultiplicativeExpression(tokens);

        while (!tokens.empty() && (tokens.front().type == TT_OpAdd || tokens.front().type == TT_OpSubtract))
        {
            auto token = tokens.front();
            tokens.pop_front();
            auto right = parseMultiplicativeExpression(tokens);
            left = std::make_shared<BinaryOpNode>(token.line,token.col,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseComparisonExpression(std::list<Token>& tokens)
    {
        auto left = parseAdditiveExpression(tokens);

        while (!tokens.empty() && (tokens.front().type == TT_OpEqual || tokens.front().type == TT_OpNotEqual ||
            tokens.front().type == TT_OpLess || tokens.front().type == TT_OpLessEqual || tokens.front().type ==
            TT_OpGreater || tokens.front().type == TT_OpGreaterEqual))
        {
            auto token = tokens.front();
            tokens.pop_front();
            auto right = parseExpression(tokens);
            left = std::make_shared<BinaryOpNode>(token.line,token.col,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseLogicalExpression(std::list<Token>& tokens)
    {
        auto left = parseComparisonExpression(tokens);

        while (!tokens.empty() && (tokens.front().type == TT_OpAdd || tokens.front().type == TT_OpOr ||
            tokens.front().type == TT_OpNot))
        {
            auto token = tokens.front();
            tokens.pop_front();
            auto right = parseExpression(tokens);
            left = std::make_shared<BinaryOpNode>(token.line,token.col,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseAssignmentExpression(std::list<Token>& tokens)
    {
        auto left = parseLogicalExpression(tokens);

        while (!tokens.empty() && tokens.front().type == TT_Assign)
        {
            auto token = tokens.front();
            tokens.pop_front();
            auto right = parseLogicalExpression(tokens);
            left = std::make_shared<AssignNode>(token.line,token.col,left, right);
        }

        return left;
    }

    std::shared_ptr<Node> parseExpression(std::list<Token>& tokens)
    {
        return parseAssignmentExpression(tokens);
    }

    std::shared_ptr<ReturnNode> parseReturn(std::list<Token>& tokens)
    {
        auto token = tokens.front();
        tokens.pop_front();
        return std::make_shared<ReturnNode>(token.line,token.col,parseExpression(tokens));
    }

    std::shared_ptr<ForNode> parseFor(std::list<Token>& tokens)
    {
        auto token = tokens.front();
        tokens.pop_front();
        tokens.pop_front();
        auto initStatement = parseStatement(tokens);
        auto conditionStatement = parseStatement(tokens);
        auto updateStatement = parseStatement(tokens);
        tokens.pop_front();
        
        return std::make_shared<ForNode>(token.line,token.col,initStatement,conditionStatement,updateStatement,parseScope(tokens));
    }

    std::shared_ptr<WhileNode> parseWhile(std::list<Token>& tokens)
    {
        auto token = tokens.front();
        tokens.pop_front();
        tokens.pop_front();
        auto conditionStatement = parseStatement(tokens);
        tokens.pop_front();
        
        return std::make_shared<WhileNode>(token.line,token.col,conditionStatement,parseScope(tokens));
    }

    std::shared_ptr<Node> parseStatement(std::list<Token>& tokens)
    {
        switch (tokens.front().type)
        {
        case TT_Return:
            {
                auto token = tokens.front();
                tokens.pop_front();
                std::list<Token> statement;
                getStatementTokens(statement, tokens);
                return std::make_shared<ReturnNode>(token.line,token.col,parseExpression(statement));
            }
        case TT_Let:
            {
                auto token = tokens.front();
                std::list<Token> statement;
                getStatementTokens(statement, tokens);
                statement.pop_front();
                auto name = statement.front();
                statement.pop_front();
                statement.pop_front(); // pop the assign
                
                return std::make_shared<CreateAndAssignNode>(token.line,token.col,name.value, parseExpression(statement));
            }
        case TT_CallBegin:
            {
                std::list<Token> statement;
                getStatementTokens(statement, tokens);
                return parseExpression(statement);
            }
        case TT_When:
            {
                std::list<Token> statement;
                getStatementTokens(statement, tokens);
                return parseWhenStatement(statement);
            }
        case TT_OpenBrace:
            {
                std::list<Token> statement;
                getStatementTokens(statement, tokens);
                return parsePrimary(statement); // primary handles braces
            }
        case TT_Function:
            {
                std::list<Token> statement;
                getStatementTokens(statement, tokens);
                return parseFunction(statement);
            }
        case TT_For:
            {
                std::list<Token> statement;
                getStatementTokens(statement, tokens);
                
                return parseFor(statement);
            }
        case TT_While:
            {
                std::list<Token> statement;
                getStatementTokens(statement, tokens);
                
                return parseWhile(statement);
            }
        case TT_Class:
            {
                std::list<Token> statement;
                getStatementTokens(statement, tokens);
                
                return parseClass(statement);
            }
        }

        {
            std::list<Token> statement;
            getStatementTokens(statement, tokens);
            return parseExpression(statement);
        }
    }

    std::shared_ptr<FunctionNode> parseFunction(std::list<Token>& tokens)
    {
        auto id = tokens.front();
        tokens.pop_front();
        
        std::string name;
        
        if(tokens.front().type == TT_Identifier)
        {
            name = tokens.front().value;
            tokens.pop_front();
        }

        tokens.pop_front(); // skip first bracket

        std::vector<std::string> args;
        while (tokens.front().type != ETokenType::TT_CloseParen)
        {
            auto token = tokens.front();

            switch (token.type)
            {
            case TT_FunctionArgumentBegin:
                {
                    tokens.pop_front();
                    std::list<Token> argsToken;
                    getTokensTill(argsToken,tokens,[](const Token& tok, int scope)
                    {
                        return (tok.type == TT_FunctionArgumentBegin && scope == 0) || tok.type == TT_CloseParen && scope == -1;
                    },0,false);
                    
                    auto arg = argsToken.front();
                    args.push_back(arg.value);
                    continue;
                }
            }

            tokens.pop_front();
        }

        auto token = tokens.front();
        tokens.pop_front();

        return std::make_shared<FunctionNode>(token.line,token.col,name, args,parseScope(tokens));
    }

    std::vector<std::shared_ptr<Node>> parseCallArguments(std::list<Token>& tokens)
    {
        
        const auto tok = tokens.front();
        auto scope = 1;
        auto argumentScope = 0;
        std::list<Token> argument;
        std::vector<std::shared_ptr<Node>> args;

        while (!tokens.empty() && scope != 0)
        {
            auto tok = tokens.front();
            switch (tok.type)
            {
            case TT_CallBegin:
                scope++;
                break;
            case TT_CallEnd:
                scope--;
                if (scope == 0)
                {
                    tokens.pop_front();
                    continue;
                }
                break;
            case TT_CallArgumentBegin:
                argumentScope++;
                if (argumentScope == 1)
                {
                    tokens.pop_front();
                    continue;
                }
                break;
            case TT_CallArgumentEnd:
                argumentScope--;
                if (argumentScope == 0)
                {
                    args.push_back(parseExpression(argument));
                    tokens.pop_front();
                    continue;
                }

                break;
            }

            argument.push_back(tok);

            tokens.pop_front();
        }
        return args;
    }

    std::shared_ptr<WhenNode> parseWhen(std::list<Token>& tokens)
    {
        auto token = tokens.front();
        std::list<Token> targetTokens;
        getTokensTill(targetTokens,tokens,{TT_CloseBrace});
        targetTokens.pop_front(); // Pop when token
        targetTokens.pop_front(); // Pop opening brace
        std::vector<WhenNode::Branch> branches;

        while(!targetTokens.empty())
        {
            targetTokens.pop_front(); // Pop condition begin
            std::list<Token> conditionTokens;
            getTokensTill(conditionTokens,targetTokens,{TT_WhenConditionEnd},0);
            targetTokens.pop_front(); // pop action begin
            std::list<Token> expressionTokens;
            getTokensTill(expressionTokens,targetTokens,{TT_WhenActionEnd},0);
            expressionTokens.pop_back();
            branches.emplace_back(parseExpression(conditionTokens),parseExpression(expressionTokens));
        }
        
        return std::make_shared<WhenNode>(token.line,token.col,branches);
    }

    std::shared_ptr<WhenNode> parseWhenStatement(std::list<Token>& tokens)
    {
        auto r = parseWhen(tokens);
        return r;
    }

    std::shared_ptr<PrototypeNode> parseClass(std::list<Token>& tokens)
    {
        auto token = tokens.front();
        tokens.pop_front();
        auto className = tokens.front().value;
        tokens.pop_front();
        std::vector<std::string> parents;
        while(tokens.front().type == TT_Identifier)
        {
            parents.push_back(tokens.front().value);
            tokens.pop_front();
        }

        auto scope = parseScope(tokens);
        return  std::make_shared<PrototypeNode>(token.line,token.col,className,parents,scope);
    }
    

    void getStatementTokens(std::list<Token>& statement, std::list<Token>& tokens)
    {
        return getTokensTill(statement,tokens,{TT_StatementEnd},0);
    }

    void getTokensTill(std::list<Token>& result, std::list<Token>& tokens,
        const std::function<bool(const Token&, int)>& evaluator, int initialScope, bool popEnd)
    {
        auto scope = initialScope;
        
        while (!tokens.empty())
        {
            auto tok = tokens.front();
            switch (tok.type)
            {
            case TT_OpenBrace:
            case TT_OpenParen:
            case TT_OpenBracket:
                scope++;
                break;
            case TT_CloseBrace:
            case TT_CloseParen:
            case TT_CloseBracket:
                scope--;
                break;
            default:
                break;
            }

            if(evaluator(tok,scope))
            {
                if(popEnd)
                {
                    tokens.pop_front();
                }
                return;
            }

            result.push_back(tok);
            tokens.pop_front();
        }
    }

    void getTokensTill(std::list<Token>& result, std::list<Token>& tokens,const std::set<ETokenType>& ends,int initialScope,bool popEnd)
    {
        auto scope = initialScope;
        
        while (!tokens.empty())
        {
            auto tok = tokens.front();
            switch (tok.type)
            {
            case TT_OpenBrace:
            case TT_OpenParen:
            case TT_OpenBracket:
                scope++;
                break;
            case TT_CloseBrace:
            case TT_CloseParen:
            case TT_CloseBracket:
                scope--;
                break;
            default:
                break;
            }

            if(ends.contains(tok.type) && scope == 0)
            {
                if(popEnd)
                {
                    tokens.pop_front();
                }
                return;
            }

            result.push_back(tok);
            tokens.pop_front();
        }
    }

    std::shared_ptr<ScopeNode> parseScope(std::list<Token>& tokens)
    {
        auto token = tokens.front();
        tokens.pop_front(); // remove first brace
        std::list<Token> content;
        getTokensTill(content,tokens,{TT_CloseBrace},1);

        auto node = std::make_shared<ScopeNode>(token.line,token.col);
        
        while (!content.empty())
        {
            //const Token tok = content.front();
            node->statements.push_back(parseStatement(content));
            // switch (tok.type)
            // {
            // default:
            //     node->statements.push_back(parseStatement(content));
            //     continue;
            // }
            //content.pop_front();
        }
        
        return node;
    }

    std::shared_ptr<ModuleNode> parse(std::list<Token> tokens)
    {
        auto node = std::make_shared<ModuleNode>(0,0,std::vector<std::shared_ptr<Node>>());
        while (!tokens.empty())
        {
            node->statements.push_back(parseStatement(tokens));
        }
        return node;
    }
}
