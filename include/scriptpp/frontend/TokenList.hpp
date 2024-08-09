#pragma once
#include "Token.hpp"

namespace spp::frontend
{
    struct TokenList final
    {
    protected:
        std::list<Token> _tokens;
        std::list<Token> _lastFront;
    public:
        virtual ~TokenList();

        void SetLastFront(const Token& token);
        
        [[noreturn]] void ThrowExpectedInput() const;

        [[noreturn]] static void ThrowAt(const std::string& message,const TokenDebugInfo& debugInfo);

        Token RemoveFront();

        Token RemoveBack();

        TokenList& ExpectFront(const ETokenType& token);
        
        TokenList& ExpectFront(const std::string& token);

        TokenList& ExpectBack(const ETokenType& token);
        
        TokenList& ExpectBack(const std::string& token);

        Token& Front();

        Token& Back();
        
        size_t Size() const;

        TokenList& InsertFront(const TokenList& tokens);

        TokenList& InsertFront(const Token& token);
        
        TokenList& InsertBack(const Token& token);

        TokenList& Clear();

        std::list<Token>& GetList();


        template<typename ...TArgs>
        std::enable_if_t<std::is_constructible_v<Token, TArgs...>, TokenList&> EmplaceFront(TArgs&&... args);

        template<typename ...TArgs>
        std::enable_if_t<std::is_constructible_v<Token, TArgs...>, TokenList&> EmplaceBack(TArgs&&... args);

        operator bool() const;
    };
}
