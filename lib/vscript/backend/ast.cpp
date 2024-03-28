#include "vscript/backend/ast.hpp"

#include <optional>
#include <stdexcept>

namespace vs::backend
{

    Node::Node(const TokenDebugInfo& inDebugInfo)
    {
        debugInfo = inDebugInfo;
    }

    Node::Node(const TokenDebugInfo& inDebugInfo, ENodeType inType) : Node(inDebugInfo)
    {
        type = inType;
    }

    HasLeft::HasLeft(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft) : Node(inDebugInfo)
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

    BinaryOpNode::BinaryOpNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                               const std::shared_ptr<Node>& inRight, const EBinaryOp& inOp) : HasLeft(inDebugInfo,inLeft)
    {
        right = inRight;
        op = inOp;
        type = ENodeType::NT_BinaryOp;
        isStatic = left->isStatic && right->isStatic;
    }

    BinaryOpNode::BinaryOpNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                               const std::shared_ptr<Node>& inRight, const ETokenType& inOp) : HasLeft(inDebugInfo,inLeft)
    {
        right = inRight;
        type = ENodeType::NT_BinaryOp;
            
        op = TokenTypeToBinaryOp(inOp);
        isStatic = left->isStatic && right->isStatic;
    }

    LiteralNode::LiteralNode(const TokenDebugInfo& inDebugInfo, const std::string& inValue,const ENodeType& inType) : Node(inDebugInfo)
    {
        value = inValue;
        type = inType;

        if(type == NT_NullLiteral || type == NT_NumericLiteral || type == NT_BooleanLiteral)
        {
            isStatic = true;
        }
    }

    ListLiteralNode::ListLiteralNode(const TokenDebugInfo& inDebugInfo, const std::vector<std::shared_ptr<Node>>& inValues) : Node(inDebugInfo)
    {
        values = inValues;
        type = NT_ListLiteral;
    }

    CreateAndAssignNode::CreateAndAssignNode(const TokenDebugInfo& inDebugInfo, const std::string& inName,
                                             const std::shared_ptr<Node>& inValue) : Node(inDebugInfo)
    {
        name = inName;
        value = inValue;
        type = ENodeType::NT_CreateAndAssign;
    }

    AssignNode::AssignNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inTarget,
                           const std::shared_ptr<Node>& inValue) : HasLeft(inDebugInfo,inTarget)
    {
        value = inValue;
        type = ENodeType::NT_Assign;
    }

    VariableNode::VariableNode(const TokenDebugInfo& inDebugInfo, const std::string& inValue) : Node(inDebugInfo)
    {
        value = inValue;
        type = ENodeType::NT_Variable;
    }

    ScopeNode::ScopeNode(const TokenDebugInfo& inDebugInfo, const std::vector<std::shared_ptr<Node>>& inStatements) : Node(inDebugInfo)
    {
        statements = inStatements;
        type = NT_Scope;
    }

    AccessNode::AccessNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                           const std::shared_ptr<Node>& inRight) : HasLeft(inDebugInfo,inLeft)
    {
        right = inRight;
        type = NT_Access;
    }

    AccessNode2::AccessNode2(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                             const std::shared_ptr<Node>& inWithin) : HasLeft(inDebugInfo,inLeft)
    {
        within = inWithin;
        type = NT_Access2;
    }

    WhenNode::WhenNode(const TokenDebugInfo& inDebugInfo, const std::vector<Branch>& inBranches) : Node(inDebugInfo)
    {
        branches = inBranches;
        type = ENodeType::NT_When;
    }

    ReturnNode::ReturnNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inExpression) : Node(inDebugInfo)
    {
        expression = inExpression;
        type = ENodeType::NT_Return;
    }

    ThrowNode::ThrowNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inExpression) : Node(inDebugInfo,NT_Throw)
    {
        expression = inExpression;
    }

    TryCatchNode::TryCatchNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<ScopeNode>& inTryScope,
        const std::shared_ptr<ScopeNode>& inCatchScope, const std::string& inCatchArgName) : Node(inDebugInfo,NT_TryCatch)
    {
        tryScope = inTryScope;
        catchScope = inCatchScope;
        catchArgumentName = inCatchArgName;
    }

    FunctionNode::FunctionNode(const TokenDebugInfo& inDebugInfo, const std::string& inName,
                               const std::vector<std::string>& inArgs, const std::shared_ptr<ScopeNode>& inBody) : Node(inDebugInfo)
    {
        name = inName;
        args = inArgs;
        body = inBody;
        type = ENodeType::NT_Function;
    }

    CallNode::CallNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                       const std::vector<std::shared_ptr<Node>>& inArgs) : HasLeft(inDebugInfo,inLeft)
    {
        args = inArgs;
        type = ENodeType::NT_Call;
    }

    ForNode::ForNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inInit,
                     const std::shared_ptr<Node>& inCondition, const std::shared_ptr<Node>& inUpdate,
                     const std::shared_ptr<ScopeNode>& inBody) : Node(inDebugInfo)
    {
        init = inInit;
        condition = inCondition;
        update = inUpdate;
        body = inBody;
        type = NT_For;
    }

    WhileNode::WhileNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inCondition,
                         const std::shared_ptr<ScopeNode>& inBody) : Node(inDebugInfo)
    {
        condition = inCondition;
        body = inBody;
        type = ENodeType::NT_While;
    }

    PrototypeNode::PrototypeNode(const TokenDebugInfo& inDebugInfo, const std::string& inId,
                                 const std::vector<std::string>& inParents, const std::shared_ptr<ScopeNode>& inScope) : Node(inDebugInfo)
    {
        id = inId;
        parents = inParents;
        scope = inScope;
        type = NT_Class;
    }

    ModuleNode::ModuleNode(const TokenDebugInfo& inDebugInfo, const std::vector<std::shared_ptr<Node>>& inStatements) : Node(inDebugInfo)
    {
        statements = inStatements;
        type = ENodeType::NT_Module;
    }

    void parseFunctionBody(Tokenized& tokens, std::vector<std::shared_ptr<Node>>& body)
    {
        tokens.RemoveFront(); // should be a {
        Tokenized statement;
        size_t scope = 1;
        while (scope != 0)
        {
            const auto tok = tokens.Front();

            switch (tok.type)
            {
            case TT_Function:
                body.push_back(parseFunction(tokens));
                break;
            case TT_StatementEnd:
                tokens.RemoveFront();
                body.push_back(parseStatement(statement));
                statement.Clear();
                
                continue;
            case TT_OpenBrace:
                scope++;
                break;
            case TT_CloseBrace:
                scope--;
                if (scope == 0)
                {
                    tokens.RemoveFront();
                    return;
                }
                break;
            default:
                statement.InsertBack(tok);
            }
            tokens.RemoveFront();
        }
    }

    std::shared_ptr<Node> parseParen(Tokenized& tokens)
    {
        tokens.RemoveFront();
        Tokenized inParen;
        getTokensTill(inParen,tokens,{TT_CloseParen},1);
        

        return parseExpression(inParen);
    }

    std::shared_ptr<ListLiteralNode> parseList(Tokenized& tokens)
    {
        auto token = tokens.Front();
        tokens.RemoveFront();
        Tokenized listTokens;
        getTokensTill(listTokens,tokens,{TT_CloseBracket},1);

        std::vector<std::shared_ptr<Node>> items;
        while(listTokens)
        {
            Tokenized itemTokens;
            getTokensTill(itemTokens,listTokens,{TT_Comma});
            if(!itemTokens)
            {
                items.push_back(parseExpression(itemTokens));
            }
        }

        return std::make_shared<ListLiteralNode>(token.debugInfo,items);
    }
    

    std::shared_ptr<Node> parsePrimary(Tokenized& tokens)
    {
        if(!tokens)
        {
            tokens.ThrowExpectedInput();
        }
        
        auto tok = tokens.Front();
        
        switch (tok.type)
        {
        case TT_Identifier:
            tokens.RemoveFront();
            return std::make_shared<VariableNode>(tok.debugInfo,tok.value);
        case TT_NumericLiteral:
            tokens.RemoveFront();
            return std::make_shared<LiteralNode>(tok.debugInfo,tok.value,NT_NumericLiteral);
        case TT_StringLiteral:
            tokens.RemoveFront();
            return std::make_shared<LiteralNode>(tok.debugInfo,tok.value,NT_StringLiteral);
        case TT_BooleanLiteral:
            tokens.RemoveFront();
            return std::make_shared<LiteralNode>(tok.debugInfo,tok.value,NT_BooleanLiteral);
        case TT_OpenParen:
            return parseParen(tokens);
        case TT_OpenBrace:
                return parseScope(tokens);
        case TT_When:
            return parseWhen(tokens);
        case TT_Function:
            return parseFunction(tokens);
        case TT_Break:
                return std::make_shared<Node>(tok.debugInfo,NT_Break);
        case TT_Continue:
                return std::make_shared<Node>(tok.debugInfo,NT_Continue);
        case TT_Null:
            return std::make_shared<LiteralNode>(tok.debugInfo,tok.value,NT_NullLiteral);
        case TT_OpenBracket:
            return parseList(tokens);
        default:
            throw std::runtime_error("Unknown primary token");
        }
    }

    std::shared_ptr<Node> parseAccessors(Tokenized& tokens)
    {
        auto left= parsePrimary(tokens);
        
        while (tokens && (tokens.Front().type == TT_CallBegin || tokens.Front().type == TT_Access || tokens.Front().type == TT_OpenBracket))
        {
            switch (tokens.Front().type)
            {
            case TT_CallBegin:
                {
                    auto token = tokens.Front();
                    tokens.RemoveFront();
                    auto right = parseCallArguments(tokens);
                    left = std::make_shared<CallNode>(token.debugInfo,left, right);
                }
                break;
            case TT_Access:
                {
                    auto token = tokens.Front();
                    tokens.RemoveFront();
                    auto right = parsePrimary(tokens);
                    left = std::make_shared<AccessNode>(token.debugInfo,left, right);
                }
                break;
            case TT_OpenBracket:
                {
                    auto token = tokens.Front();
                    tokens.RemoveFront();
                    Tokenized within;
                    getTokensTill(within,tokens,{TT_CloseBracket},1);
                    auto right = parseExpression(within);
                    left = std::make_shared<AccessNode2>(token.debugInfo,left, right);
                }
            }
        }

        return left;
    }

    std::shared_ptr<Node> parseMultiplicativeExpression(Tokenized& tokens)
    {
        auto left = parseAccessors(tokens);

        while (tokens && (tokens.Front().type == TT_OpMultiply || tokens.Front().type == TT_OpDivide || tokens.
            Front().type == TT_OpMod))
        {
            auto token = tokens.Front();
            tokens.RemoveFront();
            auto right = parseAccessors(tokens);
            left = std::make_shared<BinaryOpNode>(token.debugInfo,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseAdditiveExpression(Tokenized& tokens)
    {
        auto left = parseMultiplicativeExpression(tokens);

        while (tokens && (tokens.Front().type == TT_OpAdd || tokens.Front().type == TT_OpSubtract))
        {
            auto token = tokens.Front();
            tokens.RemoveFront();
            auto right = parseMultiplicativeExpression(tokens);
            left = std::make_shared<BinaryOpNode>(token.debugInfo,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseComparisonExpression(Tokenized& tokens)
    {
        auto left = parseAdditiveExpression(tokens);

        while (tokens && (tokens.Front().type == TT_OpEqual || tokens.Front().type == TT_OpNotEqual ||
            tokens.Front().type == TT_OpLess || tokens.Front().type == TT_OpLessEqual || tokens.Front().type ==
            TT_OpGreater || tokens.Front().type == TT_OpGreaterEqual))
        {
            auto token = tokens.Front();
            tokens.RemoveFront();
            auto right = parseAdditiveExpression(tokens);
            left = std::make_shared<BinaryOpNode>(token.debugInfo,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseLogicalExpression(Tokenized& tokens)
    {
        auto left = parseComparisonExpression(tokens);

        while (tokens && (tokens.Front().type == TT_OpAnd || tokens.Front().type == TT_OpOr ||
            tokens.Front().type == TT_OpNot))
        {
            auto token = tokens.Front();
            tokens.RemoveFront();
            auto right = parseComparisonExpression(tokens);
            left = std::make_shared<BinaryOpNode>(token.debugInfo,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseAssignmentExpression(Tokenized& tokens)
    {
        auto left = parseLogicalExpression(tokens);

        while (tokens && tokens.Front().type == TT_Assign)
        {
            auto token = tokens.Front();
            tokens.RemoveFront();
            auto right = parseLogicalExpression(tokens);
            left = std::make_shared<AssignNode>(token.debugInfo,left, right);
        }

        return left;
    }

    std::shared_ptr<Node> parseExpression(Tokenized& tokens)
    {
        return parseAssignmentExpression(tokens);
    }

    std::shared_ptr<ReturnNode> parseReturn(Tokenized& tokens)
    {
        auto token = tokens.Front();
        tokens.RemoveFront();
        return std::make_shared<ReturnNode>(token.debugInfo,parseExpression(tokens));
    }

    std::shared_ptr<ForNode> parseFor(Tokenized& tokens)
    {
        auto token = tokens.Front();
        tokens.RemoveFront();
        tokens.RemoveFront();
        auto initStatement = parseStatement(tokens);
        auto conditionStatement = parseStatement(tokens);
        auto updateStatement = parseStatement(tokens);
        tokens.RemoveFront();
        
        return std::make_shared<ForNode>(token.debugInfo,initStatement,conditionStatement,updateStatement,parseScope(tokens));
    }

    std::shared_ptr<WhileNode> parseWhile(Tokenized& tokens)
    {
        auto token = tokens.Front();
        tokens.RemoveFront();
        tokens.RemoveFront();
        auto conditionStatement = parseStatement(tokens);
        tokens.RemoveFront();
        
        return std::make_shared<WhileNode>(token.debugInfo,conditionStatement,parseScope(tokens));
    }

    std::shared_ptr<TryCatchNode> parseTryCatch(Tokenized& tokens)
    {
        std::string catchArg;
        auto tryTok = tokens.ExpectFront(TT_Try).RemoveFront();
        Tokenized tryScopeTokens;
        getTokensTill(tryScopeTokens,tokens,{TT_CloseBrace},false);
        auto tryScope = parseScope(tryScopeTokens);
        tokens.ExpectFront(TT_Catch).RemoveFront();
        if(tokens.Front().type == TT_Identifier)
        {
            catchArg = tokens.Front().value;
            tokens.RemoveFront();
        }

        Tokenized catchScopeTokens;
        getTokensTill(catchScopeTokens,tokens,{TT_CloseBrace},false);
        auto catchScope = parseScope(catchScopeTokens);

        
        return std::make_shared<TryCatchNode>(tryTok.debugInfo,tryScope,catchScope,catchArg);
    }
    
    std::shared_ptr<Node> parseStatement(Tokenized& tokens)
    {
        switch (tokens.Front().type)
        {
        case TT_Return:
            {
                auto token = tokens.Front();
                tokens.RemoveFront();
                Tokenized statement;
                getStatementTokens(statement, tokens);
                return std::make_shared<ReturnNode>(token.debugInfo,parseExpression(statement));
            }
        case TT_Throw:
            {
                auto token = tokens.Front();
                tokens.RemoveFront();
                Tokenized statement;
                getStatementTokens(statement, tokens);
                return std::make_shared<ThrowNode>(token.debugInfo,parseExpression(statement));
            }
        case TT_Let:
            {
                auto token = tokens.Front();
                Tokenized statement;
                getStatementTokens(statement, tokens);
                statement.RemoveFront();
                auto name = statement.Front();
                statement.RemoveFront();
                statement.RemoveFront(); // pop the assign
                
                return std::make_shared<CreateAndAssignNode>(token.debugInfo,name.value, parseExpression(statement));
            }
        case TT_CallBegin:
            {
                Tokenized statement;
                getStatementTokens(statement, tokens);
                return parseExpression(statement);
            }
        case TT_When:
            {
                Tokenized statement;
                getStatementTokens(statement, tokens);
                return parseWhenStatement(statement);
            }
        case TT_OpenBrace:
            {
                Tokenized statement;
                getStatementTokens(statement, tokens);
                return parsePrimary(statement); // primary handles braces
            }
        case TT_Function:
            {
                Tokenized statement;
                getStatementTokens(statement, tokens);
                return parseFunction(statement);
            }
        case TT_For:
            {
                Tokenized statement;
                getStatementTokens(statement, tokens);
                
                return parseFor(statement);
            }
        case TT_While:
            {
                Tokenized statement;
                getStatementTokens(statement, tokens);
                
                return parseWhile(statement);
            }
        case TT_Class:
            {
                Tokenized statement;
                getStatementTokens(statement, tokens);
                
                return parseClass(statement);
            }
        case TT_Try:
            {
                Tokenized statement;
                getStatementTokens(statement, tokens);
                
                return parseTryCatch(statement);
            }
        }

        {
            Tokenized statement;
            getStatementTokens(statement, tokens);
            return parseExpression(statement);
        }
    }

    std::shared_ptr<FunctionNode> parseFunction(Tokenized& tokens)
    {
        auto id = tokens.Front();
        tokens.RemoveFront();
        
        std::string name;
        
        if(tokens.Front().type == TT_Identifier)
        {
            name = tokens.Front().value;
            tokens.RemoveFront();
        }

        tokens.RemoveFront(); // skip first bracket

        std::vector<std::string> args;
        while (tokens.Front().type != ETokenType::TT_CloseParen)
        {
            auto token = tokens.Front();

            switch (token.type)
            {
            case TT_FunctionArgumentBegin:
                {
                    tokens.RemoveFront();
                    Tokenized argsToken;
                    getTokensTill(argsToken,tokens,[](const Token& tok, int scope)
                    {
                        return (tok.type == TT_FunctionArgumentBegin && scope == 0) || tok.type == TT_CloseParen && scope == -1;
                    },0,false);
                    
                    auto arg = argsToken.Front();
                    args.push_back(arg.value);
                    continue;
                }
            }

            tokens.RemoveFront();
        }

        auto token = tokens.Front();
        tokens.RemoveFront();

        return std::make_shared<FunctionNode>(token.debugInfo,name, args,parseScope(tokens));
    }

    std::vector<std::shared_ptr<Node>> parseCallArguments(Tokenized& tokens)
    {
        
        const auto tok = tokens.Front();
        auto scope = 1;
        auto argumentScope = 0;
        Tokenized argument;
        std::vector<std::shared_ptr<Node>> args;

        while (tokens && scope != 0)
        {
            auto tok = tokens.Front();
            switch (tok.type)
            {
            case TT_CallBegin:
                scope++;
                break;
            case TT_CallEnd:
                scope--;
                if (scope == 0)
                {
                    tokens.RemoveFront();
                    continue;
                }
                break;
            case TT_CallArgumentBegin:
                argumentScope++;
                if (argumentScope == 1)
                {
                    tokens.RemoveFront();
                    continue;
                }
                break;
            case TT_CallArgumentEnd:
                argumentScope--;
                if (argumentScope == 0)
                {
                    args.push_back(parseExpression(argument));
                    tokens.RemoveFront();
                    continue;
                }

                break;
            }

            argument.InsertBack(tok);

            tokens.RemoveFront();
        }
        return args;
    }

    std::shared_ptr<WhenNode> parseWhen(Tokenized& tokens)
    {
        auto token = tokens.Front();
        Tokenized targetTokens;
        getTokensTill(targetTokens,tokens,{TT_CloseBrace});
        targetTokens.RemoveFront(); // Pop when token
        targetTokens.RemoveFront(); // Pop opening brace
        std::vector<WhenNode::Branch> branches;

        while(targetTokens)
        {
            targetTokens.RemoveFront(); // Pop condition begin
            Tokenized conditionTokens;
            getTokensTill(conditionTokens,targetTokens,{TT_WhenConditionEnd},0);
            targetTokens.RemoveFront(); // pop action begin
            Tokenized expressionTokens;
            getTokensTill(expressionTokens,targetTokens,{TT_WhenActionEnd},0);
            expressionTokens.RemoveBack();
            expressionTokens.EmplaceBack(TT_StatementEnd,expressionTokens.Back().debugInfo);
            branches.emplace_back(parseExpression(conditionTokens),parseStatement(expressionTokens));
        }
        
        return std::make_shared<WhenNode>(token.debugInfo,branches);
    }

    std::shared_ptr<WhenNode> parseWhenStatement(Tokenized& tokens)
    {
        auto r = parseWhen(tokens);
        return r;
    }

    std::shared_ptr<PrototypeNode> parseClass(Tokenized& tokens)
    {
        auto token = tokens.Front();
        tokens.RemoveFront();
        auto className = tokens.Front().value;
        tokens.RemoveFront();
        std::vector<std::string> parents;
        while(tokens.Front().type == TT_Identifier)
        {
            parents.push_back(tokens.Front().value);
            tokens.RemoveFront();
        }

        auto scope = parseScope(tokens);
        return  std::make_shared<PrototypeNode>(token.debugInfo,className,parents,scope);
    }
    

    void getStatementTokens(Tokenized& statement, Tokenized& tokens)
    {
        return getTokensTill(statement,tokens,{TT_StatementEnd},0);
    }

    void getTokensTill(Tokenized& result, Tokenized& tokens,
        const std::function<bool(const Token&, int)>& evaluator, int initialScope, bool popEnd)
    {
        auto scope = initialScope;
        
        while (tokens)
        {
            auto tok = tokens.Front();
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
                    tokens.RemoveFront();
                }
                return;
            }

            result.InsertBack(tok);
            tokens.RemoveFront();
        }
    }

    void getTokensTill(Tokenized& result, Tokenized& tokens,const std::set<ETokenType>& ends,int initialScope,bool popEnd)
    {
        auto scope = initialScope;
        
        while (tokens)
        {
            auto tok = tokens.Front();
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
                    tokens.RemoveFront();
                }
                return;
            }

            result.InsertBack(tok);
            tokens.RemoveFront();
        }
    }

    std::shared_ptr<ScopeNode> parseScope(Tokenized& tokens)
    {
        auto token = tokens.Front();
        tokens.RemoveFront(); // remove first brace
        Tokenized content;
        getTokensTill(content,tokens,{TT_CloseBrace},1);

        auto node = std::make_shared<ScopeNode>(token.debugInfo);
        
        while (content)
        {
            node->statements.push_back(parseStatement(content));
        }
        
        return node;
    }

    std::shared_ptr<ModuleNode> parse(Tokenized tokens)
    {
        auto node = std::make_shared<ModuleNode>(TokenDebugInfo{tokens.Front().debugInfo.file,0,0},std::vector<std::shared_ptr<Node>>());
        while (tokens)
        {
            node->statements.push_back(parseStatement(tokens));
        }
        return node;
    }
}
