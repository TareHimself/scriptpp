#include "scriptpp/frontend/TokenList.hpp"

namespace spp::frontend
{
    TokenList::~TokenList() = default;

    void TokenList::SetLastFront(const Token& token)
    {
        if(!_lastFront.empty())
        {
            _lastFront.pop_front();
        }
        _lastFront.push_front(token);
        
    }

    
    void TokenList::ThrowExpectedInput() const
    {
        if(_lastFront.empty()) throw std::runtime_error("Unexpected end of input");
        ThrowAt("Unexpected end of input",_lastFront.front().debugInfo);
    }

    
    void TokenList::ThrowAt(const std::string& message, const TokenDebugInfo& debugInfo)
    {
         throw std::runtime_error("Error at " + debugInfo.file + " " + std::to_string(debugInfo.startLine) + ":" + std::to_string(debugInfo.startCol) + "\n" + message);
    }

    
    Token TokenList::RemoveFront()
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

    
    Token TokenList::RemoveBack()
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }
        auto a = Back();
        _tokens.pop_back();
        return a;
    }

    
    Token& TokenList::Front()
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        return _tokens.front();
    }

    
    Token& TokenList::Back()
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        return _tokens.back();
    }

    
    size_t TokenList::Size() const
    {
        return _tokens.size();
    }

    
    TokenList& TokenList::InsertFront(const TokenList& tokens)
    {
        std::list other = tokens._tokens;
        while(!other.empty())
        {
            _tokens.push_front(other.back());
            other.pop_back();
        }

        SetLastFront(_tokens.front());

        return *this;
    }

    
    TokenList& TokenList::InsertFront(const Token& token)
    {
        _tokens.push_front(token);
        SetLastFront(_tokens.front());
        return *this;
    }

    
    TokenList& TokenList::InsertBack(const Token& token)
    {
        _tokens.push_back(token);
        SetLastFront(_tokens.front());
        return *this;
    }

    
    TokenList& TokenList::Clear()
    {
        _tokens.clear();
        return *this;
    }

    
    std::list<Token>& TokenList::GetList()
    {
        return _tokens;
    }

    
    template <typename ... TArgs>
    std::enable_if_t<std::is_constructible_v<Token, TArgs...>, TokenList&> TokenList::
    EmplaceFront(TArgs&&... args)
    {
        _tokens.emplace_front(std::forward<TArgs>(args)...);
        return *this;
    }

    
    template <typename ... TArgs>
    std::enable_if_t<std::is_constructible_v<Token, TArgs...>, TokenList&> TokenList::
    EmplaceBack(TArgs&&... args)
    {
        _tokens.emplace_back(std::forward<TArgs>(args)...);
        return *this;
    }

    
    TokenList::operator bool() const
    {
        return !_tokens.empty();
    }
    
    TokenList& TokenList::ExpectFront(const ETokenType& token)
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        if(_tokens.front().type != token)
        {
            ThrowAt("Expected " + (Token::KeyWordMap.contains(token) ? Token::KeyWordMap[token] : "<identifier>") + " but got " + _tokens.front().value,_tokens.front().debugInfo);
        }

        return *this;
    }

    TokenList& TokenList::ExpectFront(const std::string& token)
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        if(_tokens.front().value != token)
        {
            ThrowAt("Expected " + token + " but got " + _tokens.front().value,_tokens.front().debugInfo);
        }

        return *this;
    }

    TokenList& TokenList::ExpectBack(const ETokenType& token)
    {
        return ExpectBack(Token::KeyWordMap[token]);
    }

    TokenList& TokenList::ExpectBack(const std::string& token)
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        if(_tokens.back().value != token)
        {
            ThrowAt("Expected " + token + " but got " + _tokens.back().value,_tokens.back().debugInfo);
        }

        return *this;
    }
}