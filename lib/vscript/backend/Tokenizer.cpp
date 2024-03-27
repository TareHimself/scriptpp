#include "vscript/backend/Tokenizer.hpp"

#include "vscript/utils.hpp"

namespace vs::backend
{
    TextStream::TextStream(const std::string& text)
    {
        _currentLine = std::stringstream("");
        std::stringstream ss(text);
        while (ss)
        {
            std::string content;
            std::getline(ss, content);
            _lines.push_back(content);
        }

        IsEmpty();
    }

    int TextStream::Peak()
    {
        if (!IsEmpty())
        {
            return _currentLine.peek();
        }

        return -1;
    }

    bool TextStream::Get(char& result)
    {
        if (!IsEmpty())
        {
            if (static_cast<bool>(_currentLine.get(result)))
            {
                colNo++;
                return true;
            }
        }

        return false;
    }

    bool TextStream::IsEmpty()
    {
        if (_currentLine.rdbuf()->in_avail() == 0 && !_lines.empty())
        {
            const auto content = _lines.front();
            _currentLine = std::stringstream(content);
            _lines.pop_front();
            lineNo++;
            colNo = 0;
        }

        return _currentLine.rdbuf()->in_avail() == 0 && _lines.empty();
    }


    std::optional<RawTokens> Tokenizer::FindNext(RawTokens& input, const std::string& search)
    {
        RawTokens extra;
        while (input)
        {
            auto test = input.Front();
            input.RemoveFront();

            if (test.data == search)
            {
                return {extra};
            }

            extra.InsertBack(test);
        }

        input.InsertFront(extra);

        return {};
    }

    std::optional<TokenMatchResult> Tokenizer::FindNextToken(RawTokens& input)
    {
        RawTokens extra;
        while (input)
        {
            auto test = input.Front();


            if (Token::Sizes.contains(test.data.size()) && Token::TokenMap.contains(test.data))
            {
                return {{{extra}, Token::TokenMap[test.data],test}};
            }

            input.RemoveFront();

            extra.InsertBack(test);
        }

        input.InsertFront(extra);

        return {};
    }

    void Tokenizer::TokenizeAssign(std::list<Token>& tokens, RawTokens& input)
    {
        tokens.emplace_back(input.ExpectFront(TT_Assign).RemoveFront());
        TokenizeExpression(tokens, input);
    }

    void Tokenizer::TokenizeFor(std::list<Token>& tokens, RawTokens& input)
    {
        tokens.emplace_back(input.ExpectFront(TT_For).RemoveFront());
        
        tokens.emplace_back(input.ExpectFront(TT_OpenParen).RemoveFront());

        RawTokens init;
        RawTokens condition;
        RawTokens update;

        ConsumeTill(init, input, Token::KeyWordMap[TT_StatementEnd]);
        ConsumeTill(condition, input, Token::KeyWordMap[TT_StatementEnd]);
        ConsumeTill(update, input, Token::KeyWordMap[TT_CloseParen], 1);
        auto closeParen = update.ExpectBack(TT_CloseParen).RemoveBack();
        update.InsertBack({Token::KeyWordMap[TT_StatementEnd], 0, 0});

        TokenizeStatement(tokens, init);
        TokenizeStatement(tokens, condition);
        TokenizeStatement(tokens, update);

        tokens.emplace_back(closeParen);

        TokenizeScope(tokens, input);
    }

    void Tokenizer::TokenizeWhile(std::list<Token>& tokens, RawTokens& input)
    {
        tokens.emplace_back(input.ExpectFront(TT_While).RemoveFront());
        auto openParen = input.ExpectFront(TT_OpenParen).RemoveFront();

        RawTokens init;
        ConsumeTill(init, input, Token::KeyWordMap[TT_CloseParen], 1);
        auto closeParen = init.ExpectBack(TT_CloseParen).RemoveBack();
        init.InsertBack({Token::KeyWordMap[TT_StatementEnd], 0, 0});
        tokens.emplace_back(openParen);
        TokenizeStatement(tokens, init);
        tokens.emplace_back(closeParen);
        TokenizeScope(tokens, input);
    }

    void Tokenizer::TokenizeWhenBranch(std::list<Token>& tokens, RawTokens& input)
    {
        auto before = FindNext(input, "->");
        if (!before.has_value())
        {
            RawTokens::ThrowAtToken("Missing -> in when statement", {" -> ", input.Front().line, input.Front().col});
        }
        tokens.emplace_back(TT_WhenConditionBegin,0,0);
        TokenizeExpression(tokens, before.value());
        tokens.emplace_back(TT_WhenConditionEnd,0,0);
        tokens.emplace_back(TT_WhenActionBegin,0,0);

        TokenizeStatement(tokens, input);
        tokens.emplace_back(TT_WhenActionEnd,0,0);
        TokenizeStatement(tokens, input);
    }

    void Tokenizer::TokenizeWhen(std::list<Token>& tokens, RawTokens& input)
    {
        tokens.emplace_back(input.ExpectFront(TT_When).RemoveFront());
        
        tokens.emplace_back(input.ExpectFront(TT_OpenBrace).RemoveFront());

        auto closeBrace = input.ExpectBack(TT_CloseBrace).RemoveBack();
        while (input)
        {
            RawTokens branch;
            ConsumeTill(branch, input, Token::KeyWordMap[TT_StatementEnd], 0);
            TokenizeWhenBranch(tokens, branch);
        }

        tokens.emplace_back(closeBrace);
    }

    void Tokenizer::TokenizeLet(std::list<Token>& tokens, RawTokens& input)
    {
        tokens.emplace_back(input.ExpectFront(TT_Let).RemoveFront());
        tokens.emplace_back(TT_Identifier, input.RemoveFront());
        tokens.emplace_back(input.ExpectFront(TT_Assign).RemoveFront());
        TokenizeExpression(tokens, input);
    }

    void Tokenizer::TokenizeLiteral(std::list<Token>& tokens, RawTokens& input)
    {
        if (!input)
        {
            return;
        }

        auto tok = input.Front();
        input.RemoveFront();
        
        if (tok.data.starts_with(Token::KeyWordMap[TT_SingleQuote]))
        {
            tok.data.pop_back();
            tok.data = tok.data.substr(1, tok.data.size());
            tokens.emplace_back(TT_StringLiteral, RawToken{tok.data,tok.line,tok.col});
        }
        else if (isFractional(tok.data) || isInteger(tok.data))
        {
            tokens.emplace_back(TT_NumericLiteral, tok);
        }
        else if (tok.data == "true" || tok.data == "false")
        {
            tokens.emplace_back(TT_BooleanLiteral, tok);
        }
        else if (tok.data == "else")
        {
            tokens.emplace_back(TT_BooleanLiteral, RawToken{"true",tok.line,tok.col});
        }
        else
        {
            tokens.emplace_back(TT_Identifier, tok);
        }
    }

    void Tokenizer::TokenizeExpression(std::list<Token>& tokens, RawTokens& input)
    {
        while (input)
        {
            if (auto r = FindNextToken(input))
            {
                switch (r->type)
                {
                case TT_OpenParen:
                    {
                        if (r->before && r->before.Back().data != " ")
                        {
                            input.InsertFront(r->before);
                            TokenizeExpression(tokens,r->before);
                            
                            TokenizeFunctionCall(tokens, input);
                        }
                        else
                        {
                            tokens.emplace_back(input.ExpectFront(TT_OpenParen).RemoveFront());
                            RawTokens inParen;
                            ConsumeTill(inParen, input, Token::KeyWordMap[TT_CloseParen], 1);

                            auto closeParen = inParen.ExpectBack(TT_CloseParen).RemoveBack();

                            TokenizeExpression(tokens, inParen);
                            tokens.emplace_back(closeParen);
                        }
                        break;
                    }
                case TT_OpDivide:
                case TT_OpMultiply:
                case TT_OpAdd:
                case TT_OpSubtract:
                case TT_OpMod:
                case TT_OpEqual:
                case TT_OpNotEqual:
                case TT_OpLess:
                case TT_OpLessEqual:
                case TT_OpGreater:
                case TT_OpGreaterEqual:
                case TT_OpAnd:
                case TT_OpOr:
                    {
                        input.RemoveFront();

                        TokenizeExpression(tokens, r->before);
                        tokens.emplace_back(r->token);
                        TokenizeExpression(tokens, input);
                    }
                    break;

                case TT_OpNot:
                    {
                        tokens.emplace_back(input.RemoveFront());
                    }
                    break;
                case TT_Comma:
                case TT_Null:
                    if(r->before)
                    {
                        TokenizeExpression(tokens,r->before);
                    }
                    tokens.emplace_back(input.RemoveFront());
                    break;
                case TT_When:
                    TokenizeWhen(tokens, input);
                    break;
                case TT_Function:
                    TokenizeFunction(tokens, input);
                    break;
                case TT_For:
                    TokenizeFor(tokens, input);
                    break;
                case TT_While:
                    TokenizeWhile(tokens, input);
                    break;
                case TT_Return:
                    {
                        tokens.emplace_back(input.RemoveFront());
                    }
                    break;
                case TT_Access:
                    {
                        TokenizeExpression(tokens,r->before);
                        tokens.emplace_back(input.RemoveFront());
                    }
                    break;
                case TT_Assign:
                    {
                        if(r->before)
                        {
                            TokenizeExpression(tokens,r->before);
                        }
                        TokenizeAssign(tokens, input);
                        break;
                    }
                case TT_OpenBracket:
                    {
                        if(r->before)
                        {
                            TokenizeExpression(tokens,r->before);
                        }
                        tokens.emplace_back(input.RemoveFront());
                        RawTokens within;
                        ConsumeTill(within,input,Token::KeyWordMap[TT_CloseBracket],1);
                        auto c = within.ExpectBack(TT_CloseBracket).RemoveBack();
                        TokenizeExpression(tokens,within);
                        tokens.emplace_back(c);
                        break;
                    }
                default:
                    TokenizeLiteral(tokens, input);
                }
            }
            else
            {
                TokenizeLiteral(tokens, input);
            }
        }
    }


    void Tokenizer::TokenizeFunctionCall(std::list<Token>& tokens, RawTokens& input)
    {
        
        input.RemoveFront();
        RawTokens args;
        tokens.emplace_back(TT_CallBegin,input.ExpectFront(TT_OpenParen).RemoveFront());
        tokens.back().value = "";
        ConsumeTill(args, input, Token::KeyWordMap[TT_CloseParen], 1);
        auto end = args.RemoveBack();
        while (args)
        {
            tokens.emplace_back(TT_CallArgumentBegin,0,0);
            RawTokens argExpr;
            ConsumeTill(argExpr, args, ",");

            if (argExpr.Back().data == ",")
            {
                argExpr.RemoveBack();
            }

            TokenizeExpression(tokens, argExpr);

            tokens.emplace_back(TT_CallArgumentEnd,0,0);
        }
        tokens.emplace_back(TT_CallEnd,end);
    }

    void Tokenizer::TokenizeFunction(std::list<Token>& tokens, RawTokens& input)
    {
        tokens.emplace_back(input.ExpectFront(TT_Function).RemoveFront());

        if (input.Front().data != Token::KeyWordMap[TT_OpenParen])
        {
            tokens.emplace_back(TT_Identifier, input.RemoveFront()); // push function id
        }
        
        tokens.emplace_back(input.ExpectFront(TT_OpenParen).RemoveFront());
        RawTokens args;
        ConsumeTill(args, input, Token::KeyWordMap[TT_CloseParen], 1);
        auto close = args.ExpectBack(TT_CloseParen).RemoveBack();
        
        while (args)
        {
            tokens.emplace_back(TT_FunctionArgumentBegin,0,0);
            RawTokens argExpr;
            ConsumeTill(argExpr, args, ",");
            if (argExpr.Back().data == ",")
            {
                argExpr.RemoveBack();
            }
            TokenizeExpression(tokens, argExpr);
        }
        tokens.emplace_back(close);
        TokenizeScope(tokens, input);
    }

    void Tokenizer::TokenizeClass(std::list<Token>& tokens, RawTokens& input)
    {
        tokens.emplace_back(input.ExpectFront(TT_Class).RemoveFront());
        tokens.emplace_back(TT_Identifier,input.RemoveFront());
        if(input.Front().data == ":")
        {
            RawTokens inheritance;
            input.RemoveFront();
            ConsumeTill(inheritance,input,Token::KeyWordMap[TT_OpenBrace],-1);
            inheritance.ExpectBack(TT_OpenBrace);
            
            input.InsertFront(inheritance.Back());
            
            inheritance.RemoveBack();
            
            while (inheritance)
            {
                auto tok = inheritance.Front();
                inheritance.RemoveFront();
                if(tok.data == ",")
                {
                    continue;
                }
                tokens.emplace_back(TT_Identifier,tok);
            }
        }
        
        RawTokens body;
        ConsumeTill(body,input,Token::KeyWordMap[TT_CloseParen],0);

        TokenizeScope(tokens,body);
    }

    void Tokenizer::TokenizeStatement(std::list<Token>& tokens, RawTokens& input)
    {
        if (auto r = FindNextToken(input))
        {
            switch (r->type)
            {
            case TT_Let:
                {
                    auto e = input.ExpectBack(TT_StatementEnd).RemoveBack(); // remove end token
                    TokenizeLet(tokens, input); // tokenize let statement
                    tokens.emplace_back(e); // add end token back
                }
                break;
            case TT_OpenBrace:
                {
                    RawTokens statement;
                    ConsumeTill(statement, input, Token::KeyWordMap[TT_CloseBrace]);
                    TokenizeScope(tokens, statement);
                    tokens.emplace_back(input.ExpectBack(TT_StatementEnd).RemoveBack());
                }
                break;
            case TT_Assign:
                {
                    if (r->before.Size() == 1)
                    {
                        input.InsertFront(r->before);
                        RawTokens statement;
                        ConsumeTill(statement, input, Token::KeyWordMap[TT_StatementEnd]);
                        auto e = statement.ExpectBack(TT_StatementEnd).RemoveBack();
                        TokenizeExpression(tokens, statement);
                        tokens.emplace_back(e);
                        break;
                    }
                }
            case TT_Return:
            case TT_Function:
            case TT_For:
            case TT_While:
                {
                    input.InsertFront(r->before);

                    RawTokens statement;
                    ConsumeTill(statement, input, Token::KeyWordMap[TT_StatementEnd]);
                    auto e = statement.ExpectBack(TT_StatementEnd).RemoveBack();
                    TokenizeExpression(tokens, statement);
                    tokens.emplace_back(e);
                    break;
                }
            case TT_Break:
            case TT_Continue:
                {
                    input.RemoveFront();
                    input.ExpectFront(Token::KeyWordMap[TT_StatementEnd]).RemoveBack();
                    tokens.emplace_back(r->token);
                }
                break;
            case TT_Class:
                {
                    RawTokens statement;
                    ConsumeTill(statement, input, Token::KeyWordMap[TT_StatementEnd]);
                    auto e = statement.ExpectBack(TT_StatementEnd).RemoveBack();
                    TokenizeClass(tokens,statement);
                    tokens.emplace_back(e);
                }
                break;
            case TT_Access: // Not yet handled
                {
                    input.InsertFront(r->before);
                    RawTokens statement;
                    ConsumeTill(statement, input, Token::KeyWordMap[TT_StatementEnd]);
                    auto e = statement.ExpectBack(TT_StatementEnd).RemoveBack();
                    TokenizeExpression(tokens,statement);
                    tokens.emplace_back(e);
                }
            default:
                input.InsertFront(r->before);

                auto end = RawToken{Token::KeyWordMap[TT_StatementEnd],input.Back().line,input.Back().col};
                if (input.Back().data == ";")
                {
                    end = input.ExpectBack(TT_StatementEnd).RemoveBack();
                }
                TokenizeExpression(tokens, input); // tokenize expression
                tokens.emplace_back(TT_StatementEnd,end); // add end token back
                break;
            }
        }
    }

    void Tokenizer::TokenizeScope(std::list<Token>& tokens, RawTokens& input)
    {
        tokens.emplace_back(input.ExpectFront(TT_OpenBrace).RemoveFront());
        auto c = input.ExpectBack(TT_CloseBrace).RemoveBack();
        TokenizeAll(tokens, input);
        tokens.emplace_back(c);
    }

    void Tokenizer::TokenizeAll(std::list<Token>& tokens, RawTokens& input)
    {
        while (input)
        {
            if (const auto r = FindNextToken(input))
            {
                switch (r->type)
                {
                case TT_For:
                case TT_Function:
                case TT_When:
                case TT_While:
                    {
                        RawTokens statement;
                        if (r->type != TT_When)
                        {
                            ConsumeTill(statement, input, Token::KeyWordMap[TT_CloseParen]);
                        }
                        ConsumeTill(statement, input, Token::KeyWordMap[TT_CloseBrace]);
                        statement.InsertBack({";", 0, 0});
                        if (input && input.Front().data == ";")
                        {
                            input.RemoveFront();
                        }
                        TokenizeStatement(tokens, statement);
                        continue;
                    }
                case TT_OpenBrace:
                    {
                        RawTokens statement;
                        if (r->type != TT_When)
                        {
                            ConsumeTill(statement, input, Token::KeyWordMap[TT_CloseParen]);
                        }
                        auto e = RawToken{Token::KeyWordMap[TT_StatementEnd],statement.Back().line,statement.Back().col};
                        TokenizeScope(tokens, statement);
                        tokens.emplace_back(e);
                    }
                }

                input.InsertFront(r->before);
            }


            RawTokens statement;
            ConsumeTill(statement, input, Token::KeyWordMap[TT_StatementEnd]);
            TokenizeStatement(tokens, statement);
        }
    }

    void Tokenizer::ConsumeTill(RawTokens& result, RawTokens& input,
                                const std::string& target, int initialScope)
    {
        auto scope = initialScope;
        do
        {
            auto tok = input.Front();

            input.RemoveFront();

            if (tok.data == Token::KeyWordMap[TT_OpenParen] || tok.data == Token::KeyWordMap[TT_OpenBrace] || tok.data
                == Token::KeyWordMap[TT_OpenBracket])
            {
                scope++;
            }
            else if (tok.data == Token::KeyWordMap[TT_CloseParen] || tok.data == Token::KeyWordMap[TT_CloseBrace] ||
                tok.data == Token::KeyWordMap[TT_CloseBracket])
            {
                scope--;
            }

            result.InsertBack(tok);
            if (scope == 0 && tok.data == target)
            {
                break;
            }
        }
        while (input);
    }

    std::string Tokenizer::ConsumeTill(TextStream& data, char token)
    {
        char tok;
        std::string pending;
        bool escaped = false;
        while (!data.IsEmpty() && data.Get(tok))
        {
            pending += tok;

            if (tok == token && !escaped)
            {
                return pending;
            }

            escaped = tok == '\\';
        }

        return pending;
    }

    std::string Tokenizer::ConsumeTill(TextStream& data, const std::string& token)
    {
        std::string window;
        std::string pending;
        bool escaped = false;
        char tok;
        while (!data.IsEmpty() && data.Get(tok))
        {
            pending += tok;
            window += tok;
            if (window.size() > token.size())
            {
                window = window.substr(1, window.size() - 1);
            }

            if (window == token && !escaped)
            {
                return pending;
            }

            escaped = tok == '\\';
        }

        return pending;
    }

    void Tokenizer::Preprocess(RawTokens& tokens, TextStream& data)
    {
        char tok;
        RawToken pending;

        const auto storePending = [&]
        {
            if (!pending.data.empty())
            {
                tokens.InsertBack(pending);
                pending.data.clear();
            }
        };


        while (!data.IsEmpty() && data.Get(tok))
        {
            const auto lineNo = data.lineNo;
            const auto colNo = data.colNo;

            switch (tok)
            {
            case ' ':
            case '\n':
            case '\r':
                storePending();
                break;
            case '\"':
                {
                    storePending();

                    auto consumed = pending.data + ConsumeTill(data, '\"');
                    consumed.pop_back();
                    pending.data = "";

                    tokens.InsertBack({'\'' + consumed + '\'', lineNo, colNo});
                }
                break;
            case '\'':
                {
                    storePending();
                    auto consumed = pending.data + ConsumeTill(data, '\'');
                    consumed.pop_back();
                    pending.data = "";

                    tokens.InsertBack({'\'' + consumed + '\'', lineNo, colNo});
                }
                break;
            case ';':
            case '(':
            case ')':
            case '{':
            case '}':
            case ',':
            case '.':
            case '[':
            case ']':
                storePending();
                tokens.InsertBack({std::string{tok}, lineNo, colNo});
                break;
            case '/':
                {
                    if (data.Peak() == '/')
                    {
                        auto comment = ConsumeTill(data, '\n');
                        continue;
                    }

                    if (data.Peak() == '*')
                    {
                        auto comment = ConsumeTill(data, "*/");
                        continue;
                    }
                }

            default:
                if (!pending.data.empty())
                {
                    pending.data += tok;
                }
                else
                {
                    pending = RawToken{std::string() + tok, lineNo, colNo};
                }
                break;
            }
        }

        if (!pending.data.empty())
        {
            tokens.InsertBack(pending);
            pending.data.clear();
        }
    }

    void Tokenizer::operator()(std::list<Token>& tokens, const std::string& data)
    {
        RawTokens rawTokens;
        TextStream ts(data);
        Preprocess(rawTokens, ts);
        TokenizeAll(tokens, rawTokens);
    }


    void tokenize(std::list<Token>& tokens, const std::string& data)
    {
        return Tokenizer()(tokens, data);
    }
}
