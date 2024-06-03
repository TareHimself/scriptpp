#pragma once
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace vs::frontend
{
    enum ETokenType
    {
        TT_Assign,
        TT_OpAnd,
        TT_OpOr,
        TT_OpNot,
        TT_OpAdd,
        TT_OpSubtract,
        TT_OpDivide,
        TT_OpMultiply,
        TT_OpEqual,
        TT_OpNotEqual,
        TT_OpLess,
        TT_OpGreater,
        TT_OpLessEqual,
        TT_OpGreaterEqual,
        TT_Let,
        TT_CallBegin,
        TT_CallEnd,
        TT_CallArgumentBegin,
        TT_CallArgumentEnd,
        TT_OpenBrace,
        TT_CloseBrace,
        TT_OpenParen,
        TT_CloseParen,
        TT_Identifier,
        TT_Function,
        TT_FunctionEnd,
        TT_FunctionArgumentBegin,
        TT_FunctionArgumentEnd,
        TT_DoubleQuote,
        TT_SingleQuote,
        TT_StringLiteral,
        TT_NumericLiteral,
        TT_StatementEnd,
        TT_Null,
        TT_Return,
        TT_Comma,
        TT_OpMod,
        TT_BooleanLiteral,
        TT_When,
        TT_OpenBracket,
        TT_CloseBracket,
        TT_Class,
        TT_WhenConditionBegin,
        TT_WhenConditionEnd,
        TT_WhenActionBegin,
        TT_WhenActionEnd,
        TT_For,
        TT_While,
        TT_Access,
        TT_Break,
        TT_Continue,
        TT_Throw,
        TT_Try,
        TT_Catch
    };

    struct TokenDebugInfo
    {
        std::string file = "<unknown>";
        uint32_t line = 0;
        uint32_t col = 0;
        TokenDebugInfo() = default;
        TokenDebugInfo(const uint32_t& inLine,const uint32_t& inCol);
        TokenDebugInfo(const std::string& inFile,const uint32_t& inLine,const uint32_t& inCol);

        std::string ToString() const;
    };

    struct RawToken
    {
        std::string data;
        TokenDebugInfo debugInfo;
    };

    
    template<typename TokenType>
    struct TokenList
    {
    protected:
        std::list<TokenType> _tokens;
        std::list<TokenType> _lastFront;
    public:

        void SetLastFront(const TokenType& token);
        [[noreturn]] virtual void ThrowExpectedInput();

        [[noreturn]] virtual void ThrowAt(const std::string& message,const TokenType& token) = 0;

        TokenType RemoveFront();

        TokenType RemoveBack();

        virtual TokenList& ExpectFront(const ETokenType& token) = 0;
        
        virtual TokenList& ExpectFront(const std::string& token) = 0;

        virtual TokenList& ExpectBack(const ETokenType& token) = 0;
        
        virtual TokenList& ExpectBack(const std::string& token) = 0;

        TokenType& Front();

        TokenType& Back();
        
        size_t Size() const;

        TokenList& InsertFront(const TokenList& tokens);

        TokenList& InsertFront(const TokenType& token);
        
        TokenList& InsertBack(const TokenType& token);

        TokenList& Clear();

        std::list<TokenType>& GetList();


        template<typename ...TArgs>
        std::enable_if_t<std::is_constructible_v<TokenType, TArgs...>, TokenList&> EmplaceFront(TArgs&&... args);

        template<typename ...TArgs>
        std::enable_if_t<std::is_constructible_v<TokenType, TArgs...>, TokenList&> EmplaceBack(TArgs&&... args);

        operator bool() const;
    };


    template <typename TokenType>
    void TokenList<TokenType>::SetLastFront(const TokenType& token)
    {
        if(!_lastFront.empty())
        {
            _lastFront.pop_front();
        }
        _lastFront.push_front(token);
        
    }

    template <typename TokenType>
    void TokenList<TokenType>::ThrowExpectedInput()
    {
        ThrowAt("Unexpected end of input",_lastFront.front());
    }

    template <typename TokenType>
    TokenType TokenList<TokenType>::RemoveFront()
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }
        auto a = Front();
        _tokens.pop_front();
        if(!_tokens.empty())
        {
            SetLastFront(_tokens.front());
        }
        return a;
    }

    template <typename TokenType>
    TokenType TokenList<TokenType>::RemoveBack()
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }
        auto a = Back();
        _tokens.pop_back();
        return a;
    }

    template <typename TokenType>
    TokenType& TokenList<TokenType>::Front()
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        return _tokens.front();
    }

    template <typename TokenType>
    TokenType& TokenList<TokenType>::Back()
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        return _tokens.back();
    }

    template <typename TokenType>
    size_t TokenList<TokenType>::Size() const
    {
        return _tokens.size();
    }

    template <typename TokenType>
    TokenList<TokenType>& TokenList<TokenType>::InsertFront(const TokenList& tokens)
    {
        std::list<TokenType> other = tokens._tokens;
        while(!other.empty())
        {
            _tokens.push_front(other.back());
            other.pop_back();
        }

        SetLastFront(_tokens.front());

        return *this;
    }

    template <typename TokenType>
    TokenList<TokenType>& TokenList<TokenType>::InsertFront(const TokenType& token)
    {
        _tokens.push_front(token);
        SetLastFront(_tokens.front());
        return *this;
    }

    template <typename TokenType>
    TokenList<TokenType>& TokenList<TokenType>::InsertBack(const TokenType& token)
    {
        _tokens.push_back(token);
        SetLastFront(_tokens.front());
        return *this;
    }

    template <typename TokenType>
    TokenList<TokenType>& TokenList<TokenType>::Clear()
    {
        _tokens.clear();
        return *this;
    }

    template <typename TokenType>
    std::list<TokenType>& TokenList<TokenType>::GetList()
    {
        return _tokens;
    }

    template <typename TokenType>
    template <typename ... TArgs>
    std::enable_if_t<std::is_constructible_v<TokenType, TArgs...>, TokenList<TokenType>&> TokenList<TokenType>::
    EmplaceFront(TArgs&&... args)
    {
        _tokens.emplace_front(std::forward<TArgs>(args)...);
        return *this;
    }

    template <typename TokenType>
    template <typename ... TArgs>
    std::enable_if_t<std::is_constructible_v<TokenType, TArgs...>, TokenList<TokenType>&> TokenList<TokenType>::
    EmplaceBack(TArgs&&... args)
    {
        _tokens.emplace_back(std::forward<TArgs>(args)...);
        return *this;
    }

    template <typename TokenType>
    TokenList<TokenType>::operator bool() const
    {
        return !_tokens.empty();
    }

    struct RawTokens : TokenList<RawToken>
    {
        void ThrowAt(const std::string& message, const RawToken& token) override;
        TokenList& ExpectFront(const ETokenType& token) override;
        TokenList& ExpectFront(const std::string& token) override;
        TokenList& ExpectBack(const ETokenType& token) override;
        TokenList& ExpectBack(const std::string& token) override;
    };

    

    struct Token
    {
        ETokenType type;
        std::string value;
        TokenDebugInfo debugInfo;
        
        Token(ETokenType inType,uint32_t inLine,uint32_t inCol);

        Token(ETokenType inType,const TokenDebugInfo& inDebugInfo);

        Token(ETokenType inType,const RawToken& token);

        Token(const RawToken& token);

        static std::unordered_map<std::string, ETokenType> TokenMap;

        static std::unordered_map<ETokenType, std::string> KeyWordMap;

        static std::map<uint32_t, std::set<std::string>> Sizes;
    };

    struct Tokenized : TokenList<Token>
    {
        void ThrowAt(const std::string& message, const Token& token) override;
        TokenList& ExpectFront(const ETokenType& token) override;
        TokenList& ExpectFront(const std::string& token) override;
        TokenList& ExpectBack(const ETokenType& token) override;
        TokenList& ExpectBack(const std::string& token) override;
    };
}
