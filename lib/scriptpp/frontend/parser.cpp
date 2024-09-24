#include "scriptpp/frontend/parser.hpp"
#include <stdexcept>

namespace spp::frontend
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
    
    EBinaryOp BinaryOpNode::TokenTypeToBinaryOp(const ETokenType& tokType)
    {
        switch (tokType)
        {
        case ETokenType::OpAdd:
            return  EBinaryOp::Add;
        case ETokenType::OpSubtract:
            return  EBinaryOp::Subtract;
        case ETokenType::OpDivide:
            return  EBinaryOp::Divide;
        case  ETokenType::OpMultiply:
            return EBinaryOp::Multiply;
        case  ETokenType::OpMod:
            return EBinaryOp::Mod;
        case  ETokenType::OpAnd:
            return EBinaryOp::And;
        case  ETokenType::OpOr:
            return EBinaryOp::Or;
        case  ETokenType::OpNot:
            return EBinaryOp::Not;
        case  ETokenType::OpEqual:
            return EBinaryOp::Equal;
        case  ETokenType::OpNotEqual:
            return EBinaryOp::NotEqual;
        case  ETokenType::OpLess:
            return EBinaryOp::Less;
        case  ETokenType::OpLessEqual:
            return EBinaryOp::LessEqual;
        case  ETokenType::OpGreater:
            return EBinaryOp::Greater;
        case  ETokenType::OpGreaterEqual:
            return EBinaryOp::GreaterEqual;
        }

        return EBinaryOp::Add;
    }

    BinaryOpNode::BinaryOpNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                               const std::shared_ptr<Node>& inRight, const EBinaryOp& inOp) : HasLeft(inDebugInfo,inLeft)
    {
        right = inRight;
        op = inOp;
        type = ENodeType::BinaryOp;
        isStatic = left->isStatic && right->isStatic;
    }

    BinaryOpNode::BinaryOpNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                               const std::shared_ptr<Node>& inRight, const ETokenType& inOp) : HasLeft(inDebugInfo,inLeft)
    {
        right = inRight;
        type = ENodeType::BinaryOp;
            
        op = TokenTypeToBinaryOp(inOp);
        isStatic = left->isStatic && right->isStatic;
    }

    NumericLiteralNode::NumericLiteralNode(const TokenDebugInfo& inDebugInfo, const std::string& inValue) : Node(inDebugInfo,ENodeType::NumericLiteral)
    {
        value = inValue;
    }

    BooleanLiteralNode::BooleanLiteralNode(const TokenDebugInfo& inDebugInfo, bool inValue) : Node(inDebugInfo,ENodeType::BooleanLiteral)
    {
        value = inValue;
    }

    StringLiteralNode::StringLiteralNode(const TokenDebugInfo& inDebugInfo, const std::string& inValue) : Node(inDebugInfo,ENodeType::StringLiteral)
    {
        value = inValue;
    }

    NullLiteralNode::NullLiteralNode(const TokenDebugInfo& inDebugInfo) : Node(inDebugInfo,ENodeType::NullLiteral)
    {
        
    }

    ListLiteralNode::ListLiteralNode(const TokenDebugInfo& inDebugInfo, const std::vector<std::shared_ptr<Node>>& inValues) : Node(inDebugInfo)
    {
        values = inValues;
        type = ENodeType::ListLiteral;
    }

    CreateAndAssignNode::CreateAndAssignNode(const TokenDebugInfo& inDebugInfo,const std::vector<std::string>& inIdentifiers,
                                             const std::shared_ptr<Node>& inValue) : Node(inDebugInfo)
    {
        identifiers = inIdentifiers;
        value = inValue;
        type = ENodeType::CreateAndAssign;
    }

    AssignNode::AssignNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inTarget,
                           const std::shared_ptr<Node>& inValue) : HasLeft(inDebugInfo,inTarget)
    {
        value = inValue;
        type = ENodeType::Assign;
    }

    NoOpNode::NoOpNode() : Node()
    {
        type = ENodeType::NoOp;
    }

    IdentifierNode::IdentifierNode(const TokenDebugInfo& inDebugInfo, const std::string& inValue) : Node(inDebugInfo)
    {
        value = inValue;
        type = ENodeType::Identifier;
    }

    ScopeNode::ScopeNode(const TokenDebugInfo& inDebugInfo, const std::vector<std::shared_ptr<Node>>& inStatements) : Node(inDebugInfo)
    {
        statements = inStatements;
        type = ENodeType::Scope;
    }

    AccessNode::AccessNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                           const std::shared_ptr<Node>& inRight) : HasLeft(inDebugInfo,inLeft)
    {
        right = inRight;
        type = ENodeType::Access;
    }

    IndexNode::IndexNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                             const std::shared_ptr<Node>& inWithin) : HasLeft(inDebugInfo,inLeft)
    {
        within = inWithin;
        type = ENodeType::Index;
    }

    WhenNode::WhenNode(const TokenDebugInfo& inDebugInfo, const std::vector<Branch>& inBranches) : Node(inDebugInfo)
    {
        branches = inBranches;
        type = ENodeType::When;
    }

    ReturnNode::ReturnNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inExpression) : Node(inDebugInfo)
    {
        expression = inExpression;
        type = ENodeType::Return;
    }

    ThrowNode::ThrowNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inExpression) : Node(inDebugInfo,ENodeType::Throw)
    {
        expression = inExpression;
    }

    TryCatchNode::TryCatchNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<ScopeNode>& inTryScope,
        const std::shared_ptr<ScopeNode>& inCatchScope, const std::string& inCatchArgName) : Node(inDebugInfo,ENodeType::TryCatch)
    {
        tryScope = inTryScope;
        catchScope = inCatchScope;
        catchArgumentName = inCatchArgName;
    }

    ParameterNode::ParameterNode(const TokenDebugInfo& inDebugInfo, const std::string& inName,
        const std::shared_ptr<Node>& inDefaultValue) : Node(inDebugInfo,ENodeType::FunctionParameter)
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
        type = ENodeType::Function;
    }

    CallNode::CallNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inLeft,
                       const std::vector<std::shared_ptr<Node>>& inArgs) : HasLeft(inDebugInfo,inLeft)
    {
        args = inArgs;
        type = ENodeType::Call;
    }

    ForNode::ForNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inInit,
                     const std::shared_ptr<Node>& inCondition, const std::shared_ptr<Node>& inUpdate,
                     const std::shared_ptr<ScopeNode>& inBody) : Node(inDebugInfo)
    {
        init = inInit;
        condition = inCondition;
        update = inUpdate;
        body = inBody;
        type = ENodeType::For;
    }

    WhileNode::WhileNode(const TokenDebugInfo& inDebugInfo, const std::shared_ptr<Node>& inCondition,
                         const std::shared_ptr<ScopeNode>& inBody) : Node(inDebugInfo)
    {
        condition = inCondition;
        body = inBody;
        type = ENodeType::While;
    }

    PrototypeNode::PrototypeNode(const TokenDebugInfo& inDebugInfo, const std::string& inId,
                                 const std::vector<std::string>& inParents, const std::shared_ptr<ScopeNode>& inScope) : Node(inDebugInfo)
    {
        id = inId;
        parents = inParents;
        scope = inScope;
        type = ENodeType::Class;
    }

    ModuleNode::ModuleNode(const TokenDebugInfo& inDebugInfo, const std::vector<std::shared_ptr<Node>>& inStatements) : Node(inDebugInfo)
    {
        statements = inStatements;
        type = ENodeType::Module;
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
            case ETokenType::Function:
                body.push_back(parseFunction(tokens));
                if(tokens && tokens.Front().type == ETokenType::StatementEnd) tokens.RemoveFront();
                break;
            case ETokenType::StatementEnd:
                tokens.RemoveFront();
                body.push_back(parseStatement(statement));
                statement.Clear();
                
                continue;
            case ETokenType::OpenBrace:
                scope++;
                break;
            case ETokenType::CloseBrace:
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
        getTokensTill(inParen,tokens,{ETokenType::CloseParen},1);
        

        return parseExpression(inParen);
    }

    std::shared_ptr<ListLiteralNode> parseList(TokenList& tokens)
    {
        auto token = tokens.Front();
        tokens.RemoveFront();
        TokenList listTokens;
        getTokensTill(listTokens,tokens,{ETokenType::CloseBracket},1);

        std::vector<std::shared_ptr<Node>> items;
        while(listTokens)
        {
            TokenList itemTokens;
            getTokensTill(itemTokens,listTokens,{ETokenType::Comma},0,false);

            if(listTokens && listTokens.Front().type == ETokenType::Comma)
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
        case ETokenType::Unknown:
            tokens.RemoveFront();
            return std::make_shared<IdentifierNode>(tok.debugInfo,tok.value);
        case ETokenType::NumericLiteral:
            tokens.RemoveFront();
            return std::make_shared<NumericLiteralNode>(tok.debugInfo,tok.value);
        case ETokenType::StringLiteral:
            tokens.RemoveFront();
            return std::make_shared<StringLiteralNode>(tok.debugInfo,tok.value);
        case ETokenType::BooleanLiteral:
            tokens.RemoveFront();
            return std::make_shared<BooleanLiteralNode>(tok.debugInfo,tok.value == "true");
        case ETokenType::OpenParen:
            return parseParen(tokens);
        case ETokenType::OpenBrace:
                return parseScope(tokens);
        case ETokenType::When:
            return parseWhen(tokens);
        case ETokenType::Function:
            return parseFunction(tokens);
        case ETokenType::Break:
                return std::make_shared<Node>(tok.debugInfo,ENodeType::Break);
        case ETokenType::Continue:
                return std::make_shared<Node>(tok.debugInfo,ENodeType::Continue);
        case ETokenType::Null:
            return std::make_shared<NullLiteralNode>(tok.debugInfo);
        case ETokenType::OpenBracket:
            return parseList(tokens);
        case ETokenType::Throw:
            tokens.RemoveFront();
            return std::make_shared<ThrowNode>(tok.debugInfo,parseExpression(tokens));
        case ETokenType::Let:
            return parseLet(tokens);
        case ETokenType::OpSubtract:
            tokens.RemoveFront();
            return std::make_shared<BinaryOpNode>(tok.debugInfo,parsePrimary(tokens),std::make_shared<NumericLiteralNode>(tok.debugInfo,"-1"),EBinaryOp::Multiply);
        default:
            throw std::runtime_error("Unknown primary token");
        }
    }

    std::shared_ptr<Node> parseAccessors(TokenList& tokens)
    {
        auto left= parsePrimary(tokens);
        
        while (tokens && (tokens.Front().type == ETokenType::OpenParen || tokens.Front().type == ETokenType::Access || tokens.Front().type == ETokenType::OpenBracket))
        {
            switch (tokens.Front().type)
            {
            case ETokenType::OpenParen:
                {
                    if(left->type == ENodeType::Access || left->type == ENodeType::Index || left->type == ENodeType::Identifier || left->type == ENodeType::Call)
                    {
                        auto token = tokens.Front();
                        auto right = parseCallArguments(tokens);
                        left = std::make_shared<CallNode>(token.debugInfo,left, right);
                    }
                    else
                    {
                        return left;
                    }
                }
                break;
            case ETokenType::Access:
                {
                    auto token = tokens.Front();
                    tokens.RemoveFront();
                    auto right = parsePrimary(tokens);
                    left = std::make_shared<AccessNode>(token.debugInfo,left, right);
                }
                break;
            case ETokenType::OpenBracket:
                {
                    auto token = tokens.Front();
                    tokens.RemoveFront();
                    TokenList within;
                    getTokensTill(within,tokens,{ETokenType::CloseBracket},1);
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

        while (tokens && (tokens.Front().type == ETokenType::OpMultiply || tokens.Front().type == ETokenType::OpDivide || tokens.
            Front().type == ETokenType::OpMod))
        {
            auto token = tokens.Front();
            tokens.RemoveFront();
            auto right = parseAccessors(tokens);
            left = std::make_shared<BinaryOpNode>(token.debugInfo,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseAdditiveExpression(TokenList& tokens)
    {
        auto left = parseMultiplicativeExpression(tokens);

        while (tokens && (tokens.Front().type == ETokenType::OpAdd || tokens.Front().type == ETokenType::OpSubtract))
        {
            auto token = tokens.Front();
            tokens.RemoveFront();
            auto right = parseMultiplicativeExpression(tokens);
            left = std::make_shared<BinaryOpNode>(token.debugInfo,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseComparisonExpression(TokenList& tokens)
    {
        auto left = parseAdditiveExpression(tokens);

        while (tokens && (tokens.Front().type == ETokenType::OpEqual || tokens.Front().type == ETokenType::OpNotEqual ||
            tokens.Front().type == ETokenType::OpLess || tokens.Front().type == ETokenType::OpLessEqual || tokens.Front().type ==
            ETokenType::OpGreater || tokens.Front().type == ETokenType::OpGreaterEqual))
        {
            auto token = tokens.Front();
            tokens.RemoveFront();
            auto right = parseAdditiveExpression(tokens);
            left = std::make_shared<BinaryOpNode>(token.debugInfo,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseLogicalExpression(TokenList& tokens)
    {
        auto left = parseComparisonExpression(tokens);

        while (tokens && (tokens.Front().type == ETokenType::OpAnd || tokens.Front().type == ETokenType::OpOr ||
            tokens.Front().type == ETokenType::OpNot))
        {
            auto token = tokens.Front();
            tokens.RemoveFront();
            auto right = parseComparisonExpression(tokens);
            left = std::make_shared<BinaryOpNode>(token.debugInfo,left, right, token.type);
        }

        return left;
    }

    std::shared_ptr<Node> parseAssignmentExpression(TokenList& tokens)
    {
        auto left = parseLogicalExpression(tokens);

        while (tokens && tokens.Front().type == ETokenType::Assign)
        {
            auto token = tokens.Front();
            tokens.RemoveFront();
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
        auto token = tokens.Front();
        tokens.RemoveFront();
        return std::make_shared<ReturnNode>(token.debugInfo,parseExpression(tokens));
    }

    std::shared_ptr<ForNode> parseFor(TokenList& tokens)
    {
        auto token = tokens.RemoveFront();
        tokens.RemoveFront();
        TokenList targetTokens{};
        getTokensTill(targetTokens,tokens,std::set{ETokenType::CloseParen},1);
        auto initStatement = parseStatement(targetTokens);
        auto conditionStatement = parseStatement(targetTokens);
        auto updateStatement = parseExpression(targetTokens);
        return std::make_shared<ForNode>(token.debugInfo,initStatement,conditionStatement,updateStatement,parseScope(tokens));
    }

    std::shared_ptr<WhileNode> parseWhile(TokenList& tokens)
    {
        auto token = tokens.RemoveFront();
        tokens.RemoveFront();
        TokenList targetTokens{};
        getTokensTill(targetTokens,tokens,std::set{ETokenType::CloseParen},1);
        
        auto conditionStatement = parseStatement(targetTokens);
        
        return std::make_shared<WhileNode>(token.debugInfo,conditionStatement,parseScope(tokens));
    }

    std::shared_ptr<TryCatchNode> parseTryCatch(TokenList& tokens)
    {
        std::string catchArg;
        auto tryTok = tokens.ExpectFront(ETokenType::Try).RemoveFront();
        TokenList tryScopeTokens;
        getTokensTill(tryScopeTokens,tokens,{ETokenType::CloseBrace},false);
        auto tryScope = parseScope(tryScopeTokens);
        tokens.ExpectFront(ETokenType::Catch).RemoveFront();
        if(tokens.Front().type == ETokenType::Unknown)
        {
            catchArg = tokens.Front().value;
            tokens.RemoveFront();
        }

        TokenList catchScopeTokens;
        getTokensTill(catchScopeTokens,tokens,{ETokenType::CloseBrace},false);
        auto catchScope = parseScope(catchScopeTokens);

        
        return std::make_shared<TryCatchNode>(tryTok.debugInfo,tryScope,catchScope,catchArg);
    }

    std::shared_ptr<CreateAndAssignNode> parseLet(TokenList& tokens)
    {
        auto token = tokens.ExpectFront(ETokenType::Let).RemoveFront();
        std::vector<std::string> ids{};
        while(tokens.Front().type != ETokenType::Assign)
        {
            ids.push_back(tokens.ExpectFront(ETokenType::Unknown).RemoveFront().value);
        }
        tokens.RemoveFront();
        return std::make_shared<CreateAndAssignNode>(token.debugInfo,ids, parseExpression(tokens));
    }

    std::shared_ptr<Node> parseStatement(TokenList& tokens)
    {
        switch (tokens.Front().type)
        {
        case ETokenType::Return:
            {
                auto token = tokens.Front();
                tokens.RemoveFront();
                TokenList statement;
                getStatementTokens(statement, tokens);
                return std::make_shared<ReturnNode>(token.debugInfo,parseExpression(statement));
            }
        case ETokenType::Throw:
            {
                auto token = tokens.Front();
                tokens.RemoveFront();
                TokenList statement;
                getStatementTokens(statement, tokens);
                return std::make_shared<ThrowNode>(token.debugInfo,parseExpression(statement));
            }
        case ETokenType::Let:
            {
                TokenList statement;
                getStatementTokens(statement, tokens);
                
                return parseLet(statement);
            }
        case ETokenType::When:
            {
                auto when = parseWhen(tokens);
                tokens.ExpectFront(ETokenType::StatementEnd).RemoveFront();
                return when;
            }
        case ETokenType::Function:
            {
                auto fn = parseFunction(tokens);
                if(tokens && tokens.Front().type == ETokenType::StatementEnd) tokens.RemoveFront();
                return fn;
            }
        case ETokenType::For:
            {
                return parseFor(tokens);
            }
        case ETokenType::While:
            {
                return parseWhile(tokens);
            }
        case ETokenType::Proto:
            {
                return parseClass(tokens);
            }
        case ETokenType::OpenBrace:
            {
                return parseScope(tokens);
            }
        case ETokenType::Try:
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
        
        getTokensTill(argumentTokens,tokens,std::set{ETokenType::CloseParen},0,false);
        
        argumentTokens.ExpectFront(ETokenType::OpenParen).RemoveFront();
        
        tokens.ExpectFront(ETokenType::CloseParen).RemoveFront();
        
        std::vector<std::shared_ptr<ParameterNode>> args;
        
        while(argumentTokens)
        {
            TokenList argumentExpression{};
            
            getTokensTill(argumentExpression,argumentTokens,std::set{ETokenType::Comma},0,false);

            if(argumentTokens && argumentTokens.Front().type == ETokenType::Comma)
            {
                argumentTokens.RemoveFront();
            }

            auto name = argumentExpression.ExpectFront(ETokenType::Unknown).RemoveFront();
            if(argumentExpression && argumentExpression.Front().type == ETokenType::Assign)
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
        auto token = tokens.ExpectFront(ETokenType::Function).RemoveFront();
        std::string identifier = tokens.Front().type == ETokenType::OpenParen ? ""  : tokens.ExpectFront(ETokenType::Unknown).RemoveFront().value;
        
        std::vector<std::shared_ptr<ParameterNode>> args = parseFunctionParameters(tokens);
        
        if(tokens.Front().type == ETokenType::OpenBrace)
        {
            return std::make_shared<FunctionNode>(token.debugInfo,identifier, args,parseScope(tokens));
        }

        auto arrow = tokens.ExpectFront(ETokenType::Arrow).RemoveFront();
        
        return std::make_shared<FunctionNode>(token.debugInfo,identifier, args,std::make_shared<ScopeNode>(arrow.debugInfo,std::vector{parseExpression(tokens)}));
    }

    std::vector<std::shared_ptr<Node>> parseCallArguments(TokenList& tokens)
    {


        TokenList callArgumentTokens{};
        
        getTokensTill(callArgumentTokens,tokens,std::set{ETokenType::CloseParen},0,false);
        
        callArgumentTokens.ExpectFront(ETokenType::OpenParen).RemoveFront();
        
        tokens.ExpectFront(ETokenType::CloseParen).RemoveFront();
        
        std::vector<std::shared_ptr<Node>> args;
        
        while(callArgumentTokens)
        {
            TokenList argumentExpression{};
            
            getTokensTill(argumentExpression,callArgumentTokens,std::set{ETokenType::Comma},0,false);

            if(callArgumentTokens && callArgumentTokens.Front().type == ETokenType::Comma)
            {
                callArgumentTokens.RemoveFront();
            }
            
            args.push_back(parseExpression(argumentExpression));
        }
        
        return args;
    }

    std::shared_ptr<WhenNode> parseWhen(TokenList& tokens)
    {
        auto token = tokens.ExpectFront(ETokenType::When).RemoveFront();
        
        tokens.ExpectFront(ETokenType::OpenBrace).RemoveFront();
        
        std::vector<WhenNode::Branch> branches;
        
        while(tokens.Front().type != ETokenType::CloseBrace)
        {
            TokenList condTokens;
            getTokensTill(condTokens,tokens,{ETokenType::Arrow},0,false);
            
            tokens.ExpectFront(ETokenType::Arrow).RemoveFront();
            
            TokenList exprTokens;
            getTokensTill(exprTokens,tokens,{ETokenType::StatementEnd},0,false);
            
            exprTokens.InsertBack(tokens.ExpectFront(ETokenType::StatementEnd).RemoveFront());
            
            branches.emplace_back(parseExpression(condTokens),parseStatement(exprTokens));
        }

        tokens.RemoveFront();
        
        return std::make_shared<WhenNode>(token.debugInfo,branches);
    }

    std::shared_ptr<PrototypeNode> parseClass(TokenList& tokens)
    {
        auto debug = tokens.ExpectFront(ETokenType::Proto).RemoveFront().debugInfo;
        
        auto className = tokens.ExpectFront(ETokenType::Unknown).RemoveFront().value;
        
        std::vector<std::string> parents;

        auto scope = parseScope(tokens);
        
        return  std::make_shared<PrototypeNode>(debug,className,parents,scope);
    }
    

    void getStatementTokens(TokenList& statement, TokenList& tokens)
    {
        getTokensTill(statement,tokens,{ETokenType::StatementEnd},0,false);
        if(tokens) tokens.ExpectFront(ETokenType::StatementEnd).RemoveFront();
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
            case ETokenType::OpenBrace:
            case ETokenType::OpenParen:
            case ETokenType::OpenBracket:
                scope++;
                break;
            case ETokenType::CloseBrace:
            case ETokenType::CloseParen:
            case ETokenType::CloseBracket:
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

    void getTokensTill(TokenList& result, TokenList& tokens,const std::set<ETokenType>& ends,int initialScope,bool popEnd)
    {
        auto scope = initialScope;
        
        while (tokens)
        {
            auto tok = tokens.Front();
            switch (tok.type)
            {
            case ETokenType::OpenBrace:
            case ETokenType::OpenParen:
            case ETokenType::OpenBracket:
                scope++;
                break;
            case ETokenType::CloseBrace:
            case ETokenType::CloseParen:
            case ETokenType::CloseBracket:
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
        getTokensTill(content,tokens,{ETokenType::CloseBrace},1);

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
