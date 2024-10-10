#include "scriptpp/frontend/parser.hpp"
#include <stdexcept>

namespace spp::frontend
{

    Node::Node(const TokenDebugInfo& inDebugInfo)
    {
        debugInfo = inDebugInfo;
    }

    Node::Node(const TokenDebugInfo& inDebugInfo, NodeType inType) : Node(inDebugInfo)
    {
        type = inType;
    }

    HasLeft::HasLeft(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft) : Node(inDebugInfo)
    {
        left = inLeft;
    }
    
    EBinaryOp BinaryOpNode::TokenTypeToBinaryOp(const TokenType& tokType)
    {
        switch (tokType)
        {
        case TokenType::OpAdd:
            return  EBinaryOp::Add;
        case TokenType::OpSubtract:
            return  EBinaryOp::Subtract;
        case TokenType::OpDivide:
            return  EBinaryOp::Divide;
        case  TokenType::OpMultiply:
            return EBinaryOp::Multiply;
        case  TokenType::OpMod:
            return EBinaryOp::Mod;
        case  TokenType::OpAnd:
            return EBinaryOp::And;
        case  TokenType::OpOr:
            return EBinaryOp::Or;
        case  TokenType::OpNot:
            return EBinaryOp::Not;
        case  TokenType::OpEqual:
            return EBinaryOp::Equal;
        case  TokenType::OpNotEqual:
            return EBinaryOp::NotEqual;
        case  TokenType::OpLess:
            return EBinaryOp::Less;
        case  TokenType::OpLessEqual:
            return EBinaryOp::LessEqual;
        case  TokenType::OpGreater:
            return EBinaryOp::Greater;
        case  TokenType::OpGreaterEqual:
            return EBinaryOp::GreaterEqual;
        }

        return EBinaryOp::Add;
    }

    BinaryOpNode::BinaryOpNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                               const std::shared_ptr<Node>& inRight, const EBinaryOp& inOp) : HasLeft(inDebugInfo,inLeft)
    {
        right = inRight;
        op = inOp;
        type = NodeType::BinaryOp;
        isStatic = left->isStatic && right->isStatic;
    }

    BinaryOpNode::BinaryOpNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                               const std::shared_ptr<Node>& inRight, const TokenType& inOp) : HasLeft(inDebugInfo,inLeft)
    {
        right = inRight;
        type = NodeType::BinaryOp;
            
        op = TokenTypeToBinaryOp(inOp);
        isStatic = left->isStatic && right->isStatic;
    }

    NumericLiteralNode::NumericLiteralNode(const TokenDebugInfo& inDebugInfo, const std::string& inValue) : Node(inDebugInfo,NodeType::NumericLiteral)
    {
        value = inValue;
    }

    BooleanLiteralNode::BooleanLiteralNode(const TokenDebugInfo& inDebugInfo, bool inValue) : Node(inDebugInfo,NodeType::BooleanLiteral)
    {
        value = inValue;
    }

    StringLiteralNode::StringLiteralNode(const TokenDebugInfo& inDebugInfo, const std::string& inValue) : Node(inDebugInfo,NodeType::StringLiteral)
    {
        value = inValue;
    }

    NullLiteralNode::NullLiteralNode(const TokenDebugInfo& inDebugInfo) : Node(inDebugInfo,NodeType::NullLiteral)
    {
        
    }

    ListLiteralNode::ListLiteralNode(const TokenDebugInfo& inDebugInfo, const std::vector<std::shared_ptr<Node>>& inValues) : Node(inDebugInfo)
    {
        values = inValues;
        type = NodeType::ListLiteral;
    }

    CreateAndAssignNode::CreateAndAssignNode(const TokenDebugInfo& inDebugInfo,const std::vector<std::string>& inIdentifiers,
                                             const std::shared_ptr<Node>& inValue) : Node(inDebugInfo)
    {
        identifiers = inIdentifiers;
        value = inValue;
        type = NodeType::CreateAndAssign;
    }

    AssignNode::AssignNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inTarget,
                           const std::shared_ptr<Node>& inValue) : HasLeft(inDebugInfo,inTarget)
    {
        value = inValue;
        type = NodeType::Assign;
    }

    NoOpNode::NoOpNode() : Node()
    {
        type = NodeType::NoOp;
    }

    IdentifierNode::IdentifierNode(const TokenDebugInfo& inDebugInfo, const std::string& inValue) : Node(inDebugInfo)
    {
        value = inValue;
        type = NodeType::Identifier;
    }

    ScopeNode::ScopeNode(const TokenDebugInfo& inDebugInfo, const std::vector<std::shared_ptr<Node>>& inStatements) : Node(inDebugInfo)
    {
        statements = inStatements;
        type = NodeType::Scope;
    }

    AccessNode::AccessNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                           const std::shared_ptr<Node>& inRight) : HasLeft(inDebugInfo,inLeft)
    {
        right = inRight;
        type = NodeType::Access;
    }

    IndexNode::IndexNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                             const std::shared_ptr<Node>& inWithin) : HasLeft(inDebugInfo,inLeft)
    {
        within = inWithin;
        type = NodeType::Index;
    }

    WhenNode::WhenNode(const TokenDebugInfo& inDebugInfo, const std::vector<Branch>& inBranches) : Node(inDebugInfo)
    {
        branches = inBranches;
        type = NodeType::When;
    }

    ReturnNode::ReturnNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inExpression) : Node(inDebugInfo)
    {
        expression = inExpression;
        type = NodeType::Return;
    }

    ThrowNode::ThrowNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inExpression) : Node(inDebugInfo,NodeType::Throw)
    {
        expression = inExpression;
    }

    TryCatchNode::TryCatchNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<ScopeNode>& inTryScope,
        const std::shared_ptr<ScopeNode>& inCatchScope, const std::string& inCatchArgName) : Node(inDebugInfo,NodeType::TryCatch)
    {
        tryScope = inTryScope;
        catchScope = inCatchScope;
        catchArgumentName = inCatchArgName;
    }

    ParameterNode::ParameterNode(const TokenDebugInfo& inDebugInfo, const std::string& inName,
        const std::shared_ptr<Node>& inDefaultValue) : Node(inDebugInfo,NodeType::FunctionParameter)
    {
        name = inName;
        defaultValue = inDefaultValue;
    }

    FunctionNode::FunctionNode(const TokenDebugInfo& inDebugInfo, const std::string& inName,
                               const std::vector<std::shared_ptr<ParameterNode>>& inParams, const std::shared_ptr<ScopeNode>& inBody) : Node(inDebugInfo)
    {
        name = inName;
        params = inParams;
        body = inBody;
        type = NodeType::Function;
    }

    CallNode::CallNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
        const std::vector<std::shared_ptr<Node>>& inPositionalArguments,
        const std::unordered_map<std::string, std::shared_ptr<Node>>& inNamedArguments)  : HasLeft(inDebugInfo,inLeft)
    {
        positionalArguments = inPositionalArguments;
        namedArguments = inNamedArguments;
        type = NodeType::Call;
    }

    ForNode::ForNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inInit,
                     const std::shared_ptr<Node>& inCondition, const std::shared_ptr<Node>& inUpdate,
                     const std::shared_ptr<ScopeNode>& inBody) : Node(inDebugInfo)
    {
        init = inInit;
        condition = inCondition;
        update = inUpdate;
        body = inBody;
        type = NodeType::For;
    }

    WhileNode::WhileNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inCondition,
                         const std::shared_ptr<ScopeNode>& inBody) : Node(inDebugInfo)
    {
        condition = inCondition;
        body = inBody;
        type = NodeType::While;
    }

    PrototypeNode::PrototypeNode(const TokenDebugInfo& inDebugInfo, const std::string& inId,
                                 const std::vector<std::string>& inParents, const std::shared_ptr<ScopeNode>& inScope) : Node(inDebugInfo)
    {
        id = inId;
        parents = inParents;
        scope = inScope;
        type = NodeType::Class;
    }

    ModuleNode::ModuleNode(const TokenDebugInfo& inDebugInfo, const std::vector<std::shared_ptr<Node>>& inStatements) : Node(inDebugInfo)
    {
        statements = inStatements;
        type = NodeType::Module;
    }

    void parseFunctionBody(TokenList& tokens, std::vector<std::shared_ptr<Node>>& body)
    {
        tokens.RemoveFront(); // should be a {
        TokenList statement;
        size_t scope = 1;
        while (scope != 0)
        {
            const auto tok = tokens.Front();

            switch (tok.type)
            {
            case TokenType::Function:
                body.push_back(parseFunction(tokens));
                if(tokens && tokens.Front().type == TokenType::StatementEnd) tokens.RemoveFront();
                break;
            case TokenType::StatementEnd:
                tokens.RemoveFront();
                body.push_back(parseStatement(statement));
                statement.Clear();
                
                continue;
            case TokenType::OpenBrace:
                scope++;
                break;
            case TokenType::CloseBrace:
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

    std::shared_ptr<Node> parseParen(TokenList& tokens)
    {
        tokens.RemoveFront();
        TokenList inParen;
        getTokensTill(inParen,tokens,{TokenType::CloseParen},1);
        

        return parseExpression(inParen);
    }

    std::shared_ptr<ListLiteralNode> parseList(TokenList& tokens)
    {
        auto token = tokens.Front();
        tokens.RemoveFront();
        TokenList listTokens;
        getTokensTill(listTokens,tokens,{TokenType::CloseBracket},1);

        std::vector<std::shared_ptr<Node>> items;
        while(listTokens)
        {
            TokenList itemTokens;
            getTokensTill(itemTokens,listTokens,{TokenType::Comma},0,false);

            if(listTokens && listTokens.Front().type == TokenType::Comma)
            {
                listTokens.RemoveFront();
            }
            
            if(itemTokens)
            {
                items.push_back(parseExpression(itemTokens));
            }
        }

        return std::make_shared<ListLiteralNode>(token.debugInfo,items);
    }
    

    std::shared_ptr<Node> parsePrimary(TokenList& tokens)
    {
        if(!tokens)
        {
            tokens.ThrowExpectedInput();
        }
        
        auto tok = tokens.Front();
        
        switch (tok.type)
        {
        case TokenType::Unknown:
            tokens.RemoveFront();
            return std::make_shared<IdentifierNode>(tok.debugInfo,tok.value);
        case TokenType::NumericLiteral:
            tokens.RemoveFront();
            return std::make_shared<NumericLiteralNode>(tok.debugInfo,tok.value);
        case TokenType::StringLiteral:
            tokens.RemoveFront();
            return std::make_shared<StringLiteralNode>(tok.debugInfo,tok.value);
        case TokenType::BooleanLiteral:
            tokens.RemoveFront();
            return std::make_shared<BooleanLiteralNode>(tok.debugInfo,tok.value == "true");
        case TokenType::OpenParen:
            return parseParen(tokens);
        case TokenType::OpenBrace:
                return parseScope(tokens);
        case TokenType::When:
            return parseWhen(tokens);
        case TokenType::Function:
            return parseFunction(tokens);
        case TokenType::Break:
                return std::make_shared<Node>(tok.debugInfo,NodeType::Break);
        case TokenType::Continue:
                return std::make_shared<Node>(tok.debugInfo,NodeType::Continue);
        case TokenType::Null:
            return std::make_shared<NullLiteralNode>(tok.debugInfo);
        case TokenType::OpenBracket:
            return parseList(tokens);
        case TokenType::Throw:
            tokens.RemoveFront();
            return std::make_shared<ThrowNode>(tok.debugInfo,parseExpression(tokens));
        case TokenType::Let:
            return parseLet(tokens);
        case TokenType::OpSubtract:
            tokens.RemoveFront();
            return std::make_shared<BinaryOpNode>(tok.debugInfo,parsePrimary(tokens),std::make_shared<NumericLiteralNode>(tok.debugInfo,"-1"),EBinaryOp::Multiply);
        default:
            throw std::runtime_error("Unknown primary token");
        }
    }

    std::shared_ptr<Node> parseAccessors(TokenList& tokens)
    {
        auto left= parsePrimary(tokens);
        
        while (tokens && (tokens.Front().type == TokenType::OpenParen || tokens.Front().type == TokenType::Access || tokens.Front().type == TokenType::OpenBracket))
        {
            switch (tokens.Front().type)
            {
            case TokenType::OpenParen:
                {
                    if(left->type == NodeType::Access || left->type == NodeType::Index || left->type == NodeType::Identifier || left->type == NodeType::Call)
                    {
                        auto token = tokens.Front();
                        std::vector<std::shared_ptr<Node>> positionalArgs{};
                        std::unordered_map<std::string,std::shared_ptr<Node>> namedArgs{};
                        parseCallArguments(tokens,positionalArgs,namedArgs);
                        left = std::make_shared<CallNode>(token.debugInfo,left, positionalArgs,namedArgs);
                    }
                    else
                    {
                        return left;
                    }
                }
                break;
            case TokenType::Access:
                {
                    auto token = tokens.RemoveFront();
                    auto right = parsePrimary(tokens);
                    left = std::make_shared<AccessNode>(token.debugInfo,left, right);
                }
                break;
            case TokenType::OpenBracket:
                {
                    auto token = tokens.RemoveFront();
                    TokenList within{};
                    getTokensTill(within,tokens,{TokenType::CloseBracket},1);
                    auto right = parseExpression(within);
                    left = std::make_shared<IndexNode>(token.debugInfo,left, right);
                }
            }
        }

        return left;
    }

    std::shared_ptr<Node> parseMultiplicativeExpression(TokenList& tokens)
    {
        auto left = parseAccessors(tokens);

        while (tokens && (tokens.Front().type == TokenType::OpMultiply || tokens.Front().type == TokenType::OpDivide || tokens.
            Front().type == TokenType::OpMod))
        {
            auto token = tokens.RemoveFront();

            // For /= *= %=
            if(tokens && tokens.Front().type == TokenType::Assign)
            {
                auto assign = tokens.RemoveFront();
                auto binaryOp = std::make_shared<BinaryOpNode>(token.debugInfo,left, parseAccessors(tokens), token.type);
                left = std::make_shared<AssignNode>(assign.debugInfo,left,binaryOp);
                continue;
            }
            
            auto right = parseAccessors(tokens);
            left = std::make_shared<BinaryOpNode>(token.debugInfo,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseAdditiveExpression(TokenList& tokens)
    {
        auto left = parseMultiplicativeExpression(tokens);

        while (tokens && (tokens.Front().type == TokenType::OpAdd || tokens.Front().type == TokenType::OpSubtract))
        {
            auto token = tokens.RemoveFront();

            // For += -=
            if(tokens && tokens.Front().type == TokenType::Assign)
            {
                auto assign = tokens.RemoveFront();
                auto binaryOp = std::make_shared<BinaryOpNode>(token.debugInfo,left, parseAccessors(tokens), token.type);
                left = std::make_shared<AssignNode>(assign.debugInfo,left,binaryOp);
                continue;
            }
            
            auto right = parseMultiplicativeExpression(tokens);
            left = std::make_shared<BinaryOpNode>(token.debugInfo,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseComparisonExpression(TokenList& tokens)
    {
        auto left = parseAdditiveExpression(tokens);

        while (tokens && (tokens.Front().type == TokenType::OpEqual || tokens.Front().type == TokenType::OpNotEqual ||
            tokens.Front().type == TokenType::OpLess || tokens.Front().type == TokenType::OpLessEqual || tokens.Front().type ==
            TokenType::OpGreater || tokens.Front().type == TokenType::OpGreaterEqual))
        {
            auto token = tokens.RemoveFront();
            auto right = parseAdditiveExpression(tokens);
            left = std::make_shared<BinaryOpNode>(token.debugInfo,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseLogicalExpression(TokenList& tokens)
    {
        auto left = parseComparisonExpression(tokens);

        while (tokens && (tokens.Front().type == TokenType::OpAnd || tokens.Front().type == TokenType::OpOr ||
            tokens.Front().type == TokenType::OpNot))
        {
            auto token = tokens.RemoveFront();
            auto right = parseComparisonExpression(tokens);
            left = std::make_shared<BinaryOpNode>(token.debugInfo,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseAssignmentExpression(TokenList& tokens)
    {
        auto left = parseLogicalExpression(tokens);

        while (tokens && tokens.Front().type == TokenType::Assign)
        {
            auto token = tokens.RemoveFront();
            auto right = parseLogicalExpression(tokens);
            left = std::make_shared<AssignNode>(token.debugInfo,left, right);
        }

        return left;
    }

    std::shared_ptr<Node> parseExpression(TokenList& tokens)
    {
        return tokens ? parseAssignmentExpression(tokens) : std::make_shared<NoOpNode>();
    }

    std::shared_ptr<ReturnNode> parseReturn(TokenList& tokens)
    {
        auto token = tokens.RemoveFront();
        return std::make_shared<ReturnNode>(token.debugInfo,parseExpression(tokens));
    }

    std::shared_ptr<ForNode> parseFor(TokenList& tokens)
    {
        auto token = tokens.RemoveFront();
        TokenList targetTokens{};
        getTokensTill(targetTokens,tokens,std::set{TokenType::CloseParen},1);
        auto initStatement = parseStatement(targetTokens);
        auto conditionStatement = parseStatement(targetTokens);
        auto updateStatement = parseExpression(targetTokens);
        return std::make_shared<ForNode>(token.debugInfo,initStatement,conditionStatement,updateStatement,parseScope(tokens));
    }

    std::shared_ptr<WhileNode> parseWhile(TokenList& tokens)
    {
        auto token = tokens.RemoveFront();
        TokenList targetTokens{};
        getTokensTill(targetTokens,tokens,std::set{TokenType::CloseParen},1);
        
        auto conditionStatement = parseStatement(targetTokens);
        
        return std::make_shared<WhileNode>(token.debugInfo,conditionStatement,parseScope(tokens));
    }

    std::shared_ptr<TryCatchNode> parseTryCatch(TokenList& tokens)
    {
        std::string catchArg{};
        auto tryTok = tokens.ExpectFront(TokenType::Try).RemoveFront();
        TokenList tryScopeTokens;
        getTokensTill(tryScopeTokens,tokens,{TokenType::CloseBrace},false);
        auto tryScope = parseScope(tryScopeTokens);
        tokens.ExpectFront(TokenType::Catch).RemoveFront();
        if(tokens.Front().type == TokenType::Unknown)
        {
            catchArg = tokens.RemoveFront().value;
        }

        TokenList catchScopeTokens;
        getTokensTill(catchScopeTokens,tokens,{TokenType::CloseBrace},false);
        auto catchScope = parseScope(catchScopeTokens);

        
        return std::make_shared<TryCatchNode>(tryTok.debugInfo,tryScope,catchScope,catchArg);
    }

    std::shared_ptr<CreateAndAssignNode> parseLet(TokenList& tokens)
    {
        auto token = tokens.ExpectFront(TokenType::Let).RemoveFront();
        std::vector<std::string> ids{};
        while(tokens.Front().type != TokenType::Assign)
        {
            ids.push_back(tokens.ExpectFront(TokenType::Unknown).RemoveFront().value);
        }
        tokens.RemoveFront();
        return std::make_shared<CreateAndAssignNode>(token.debugInfo,ids, parseExpression(tokens));
    }

    std::shared_ptr<Node> parseStatement(TokenList& tokens)
    {
        switch (tokens.Front().type)
        {
        case TokenType::Return:
            {
                auto token = tokens.RemoveFront();
                TokenList statement;
                getStatementTokens(statement, tokens);
                return std::make_shared<ReturnNode>(token.debugInfo,parseExpression(statement));
            }
        case TokenType::Throw:
            {
                auto token = tokens.RemoveFront();
                TokenList statement;
                getStatementTokens(statement, tokens);
                return std::make_shared<ThrowNode>(token.debugInfo,parseExpression(statement));
            }
        case TokenType::Let:
            {
                TokenList statement;
                getStatementTokens(statement, tokens);
                
                return parseLet(statement);
            }
        case TokenType::When:
            {
                auto when = parseWhen(tokens);
                tokens.ExpectFront(TokenType::StatementEnd).RemoveFront();
                return when;
            }
        case TokenType::Function:
            {
                auto fn = parseFunction(tokens);
                if(tokens && tokens.Front().type == TokenType::StatementEnd) tokens.RemoveFront();
                return fn;
            }
        case TokenType::For:
            {
                return parseFor(tokens);
            }
        case TokenType::While:
            {
                return parseWhile(tokens);
            }
        case TokenType::Proto:
            {
                return parseClass(tokens);
            }
        case TokenType::OpenBrace:
            {
                return parseScope(tokens);
            }
        case TokenType::Try:
            {
                return parseTryCatch(tokens);
            }
        }

        {
            TokenList statement;
            getStatementTokens(statement, tokens);
            return parseExpression(statement);
        }
    }

    std::vector<std::shared_ptr<ParameterNode>> parseFunctionParameters(TokenList& tokens)
    {
        TokenList argumentTokens{};
        
        getTokensTill(argumentTokens,tokens,std::set{TokenType::CloseParen},0,false);
        
        argumentTokens.ExpectFront(TokenType::OpenParen).RemoveFront();
        
        tokens.ExpectFront(TokenType::CloseParen).RemoveFront();
        
        std::vector<std::shared_ptr<ParameterNode>> args;
        
        while(argumentTokens)
        {
            TokenList argumentExpression{};
            
            getTokensTill(argumentExpression,argumentTokens,std::set{TokenType::Comma},0,false);

            if(argumentTokens && argumentTokens.Front().type == TokenType::Comma)
            {
                argumentTokens.RemoveFront();
            }

            auto name = argumentExpression.ExpectFront(TokenType::Unknown).RemoveFront();
            if(argumentExpression && argumentExpression.Front().type == TokenType::Assign)
            {
                argumentExpression.RemoveFront();
                auto defaultVal = parseExpression(argumentExpression);
                args.push_back(std::make_shared<ParameterNode>(name.debugInfo + defaultVal->debugInfo,name.value,defaultVal));
            }
            else
            {
                args.push_back(std::make_shared<ParameterNode>(name.debugInfo,name.value));
            }
        }
        
        return args;
    }

    std::shared_ptr<FunctionNode> parseFunction(TokenList& tokens)
    {
        auto token = tokens.ExpectFront(TokenType::Function).RemoveFront();
        std::string identifier = tokens.Front().type == TokenType::OpenParen ? ""  : tokens.ExpectFront(TokenType::Unknown).RemoveFront().value;
        
        std::vector<std::shared_ptr<ParameterNode>> args = parseFunctionParameters(tokens);
        
        if(tokens.Front().type == TokenType::OpenBrace)
        {
            return std::make_shared<FunctionNode>(token.debugInfo,identifier, args,parseScope(tokens));
        }

        auto arrow = tokens.ExpectFront(TokenType::Arrow).RemoveFront();
        
        return std::make_shared<FunctionNode>(token.debugInfo,identifier, args,std::make_shared<ScopeNode>(arrow.debugInfo,std::vector{parseExpression(tokens)}));
    }

    std::vector<std::shared_ptr<Node>> parseCallArguments(TokenList& tokens)
    {


        TokenList callArgumentTokens{};
        
        getTokensTill(callArgumentTokens,tokens,std::set{TokenType::CloseParen},0,false);
        
        callArgumentTokens.ExpectFront(TokenType::OpenParen).RemoveFront();
        
        tokens.ExpectFront(TokenType::CloseParen).RemoveFront();
        
        std::vector<std::shared_ptr<Node>> args;
        
        while(callArgumentTokens)
        {
            TokenList argumentExpression{};
            
            getTokensTill(argumentExpression,callArgumentTokens,std::set{TokenType::Comma},0,false);

            if(callArgumentTokens && callArgumentTokens.Front().type == TokenType::Comma)
            {
                callArgumentTokens.RemoveFront();
            }
            
            args.push_back(parseExpression(argumentExpression));
        }
        
        return args;
    }

    void parseCallArguments(TokenList& tokens, std::vector<std::shared_ptr<Node>>& positionalArgs,
        std::unordered_map<std::string, std::shared_ptr<Node>>& namedArgs)
    {
        TokenList callArgumentTokens{};
        
        getTokensTill(callArgumentTokens,tokens,std::set{TokenType::CloseParen},0,false);
        
        callArgumentTokens.ExpectFront(TokenType::OpenParen).RemoveFront();
        
        tokens.ExpectFront(TokenType::CloseParen).RemoveFront();

        while(callArgumentTokens)
        {
            TokenList argumentExpression{};
            
            getTokensTill(argumentExpression,callArgumentTokens,std::set{TokenType::Comma},0,false);

            if(callArgumentTokens && callArgumentTokens.Front().type == TokenType::Comma)
            {
                callArgumentTokens.RemoveFront();
            }

            {
                if(argumentExpression)
                {
                    auto name = argumentExpression.RemoveFront();
                    if(argumentExpression && argumentExpression.Front().type == TokenType::Colon)
                    {
                        argumentExpression.RemoveFront();
                        namedArgs.insert_or_assign(name.value,parseExpression(argumentExpression));
                        continue;
                    }
                    argumentExpression.InsertFront(name);
                }
            }
            positionalArgs.push_back(parseExpression(argumentExpression));
        }
    }

    std::shared_ptr<WhenNode> parseWhen(TokenList& tokens)
    {
        auto token = tokens.ExpectFront(TokenType::When).RemoveFront();
        
        tokens.ExpectFront(TokenType::OpenBrace).RemoveFront();
        
        std::vector<WhenNode::Branch> branches;
        
        while(tokens.Front().type != TokenType::CloseBrace)
        {
            TokenList condTokens;
            getTokensTill(condTokens,tokens,{TokenType::Arrow},0,false);
            
            tokens.ExpectFront(TokenType::Arrow).RemoveFront();
            
            TokenList exprTokens;
            getTokensTill(exprTokens,tokens,{TokenType::StatementEnd},0,false);
            
            exprTokens.InsertBack(tokens.ExpectFront(TokenType::StatementEnd).RemoveFront());
            
            branches.emplace_back(parseExpression(condTokens),parseStatement(exprTokens));
        }

        tokens.RemoveFront();
        
        return std::make_shared<WhenNode>(token.debugInfo,branches);
    }

    std::shared_ptr<PrototypeNode> parseClass(TokenList& tokens)
    {
        auto debug = tokens.ExpectFront(TokenType::Proto).RemoveFront().debugInfo;
        
        auto className = tokens.ExpectFront(TokenType::Unknown).RemoveFront().value;
        
        std::vector<std::string> parents;

        auto scope = parseScope(tokens);
        
        return  std::make_shared<PrototypeNode>(debug,className,parents,scope);
    }
    

    void getStatementTokens(TokenList& statement, TokenList& tokens)
    {
        getTokensTill(statement,tokens,{TokenType::StatementEnd},0,false);
        if(tokens) tokens.ExpectFront(TokenType::StatementEnd).RemoveFront();
    }

    void getTokensTill(TokenList& result, TokenList& tokens,
        const std::function<bool(const Token&, int)>& evaluator, int initialScope, bool popEnd)
    {
        auto scope = initialScope;
        
        while (tokens)
        {
            auto tok = tokens.Front();
            switch (tok.type)
            {
            case TokenType::OpenBrace:
            case TokenType::OpenParen:
            case TokenType::OpenBracket:
                scope++;
                break;
            case TokenType::CloseBrace:
            case TokenType::CloseParen:
            case TokenType::CloseBracket:
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

    void getTokensTill(TokenList& result, TokenList& tokens,const std::set<TokenType>& ends,int initialScope,bool popEnd)
    {
        auto scope = initialScope;
        
        while (tokens)
        {
            auto tok = tokens.Front();
            switch (tok.type)
            {
            case TokenType::OpenBrace:
            case TokenType::OpenParen:
            case TokenType::OpenBracket:
                scope++;
                break;
            case TokenType::CloseBrace:
            case TokenType::CloseParen:
            case TokenType::CloseBracket:
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

    std::shared_ptr<ScopeNode> parseScope(TokenList& tokens)
    {
        auto token = tokens.RemoveFront(); // remove first brace
 
        TokenList content;
        getTokensTill(content,tokens,{TokenType::CloseBrace},1);

        auto node = std::make_shared<ScopeNode>(token.debugInfo);
        
        while (content)
        {
            node->statements.push_back(parseStatement(content));
        }
        
        return node;
    }

    std::shared_ptr<ModuleNode> parse(TokenList tokens)
    {
        auto node = std::make_shared<ModuleNode>(tokens ? TokenDebugInfo{tokens.Front().debugInfo.file,0,0} : TokenDebugInfo{},std::vector<std::shared_ptr<Node>>());

        while (tokens)
        {
            node->statements.push_back(parseStatement(tokens));
        }
        return node;
    }
}
