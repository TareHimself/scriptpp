#include "vscript/backend/Token.hpp"

#include <stdexcept>

namespace vs::backend {
    void RawTokens::ThrowExpectedInput()
    {
        throw std::runtime_error("Unexpected end of input");
    }

    void RawTokens::ThrowAtToken(const std::string& message, const RawToken& token)
    {
        throw std::runtime_error("Error at " + std::to_string(token.line) + ":" + std::to_string(token.col) + "\n" + message);
    }

    RawToken RawTokens::RemoveFront()
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }
        auto a = Front();
        _tokens.pop_front();
        return a;
    }

    RawToken RawTokens::RemoveBack()
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }
        auto a = Back();
        _tokens.pop_back();
        return a;
    }

    RawTokens& RawTokens::ExpectFront(const ETokenType& token)
    {
        return ExpectFront(Token::KeyWordMap[token]);
    }

    RawTokens& RawTokens::ExpectFront(const std::string& token)
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        if(_tokens.front().data != token)
        {
            ThrowAtToken("Expected " + token + " but got " + _tokens.front().data,_tokens.front());
        }

        return *this;
    }

    RawTokens& RawTokens::ExpectBack(const ETokenType& token)
    {
        return ExpectBack(Token::KeyWordMap[token]);
    }

    RawTokens& RawTokens::ExpectBack(const std::string& token)
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        if(_tokens.back().data != token)
        {
            ThrowAtToken("Expected " + token + " but got " + _tokens.back().data,_tokens.back());
        }

        return *this;
    }

    RawToken& RawTokens::Front()
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        return _tokens.front();
    }

    RawToken& RawTokens::Back()
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        return _tokens.back();
    }

    RawToken RawTokens::Get()
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        auto tok = _tokens.front();
        _tokens.pop_front();
        return tok;
    }

    size_t RawTokens::Size() const
    {
        return _tokens.size();
    }

    RawTokens& RawTokens::InsertFront(const RawTokens& tokens)
    {
        std::list<RawToken> other = tokens._tokens;
        while(!other.empty())
        {
            _tokens.push_front(other.back());
            other.pop_back();
        }

        return *this;
    }

    RawTokens& RawTokens::InsertFront(const RawToken& token)
    {
        _tokens.push_front(token);
        return *this;
    }

    RawTokens& RawTokens::InsertBack(const RawToken& token)
    {
        _tokens.push_back(token);
        return *this;
    }

    RawTokens::operator bool() const
    {
        return !_tokens.empty();
    }

    Token::Token(ETokenType inType, uint32_t inLine, uint32_t inCol)
    {
        type = inType;
        value = KeyWordMap.contains(type) ? KeyWordMap[type] : "";
        line = inLine;
        col = inCol;
    }

    Token::Token(ETokenType inType, const RawToken& token)
    {
        type = inType;
        value = token.data;
        line = token.line;
        col = token.col;
    }

    Token::Token(const RawToken& token)
    {
        type = TokenMap[token.data];
        value = token.data;
        line = token.line;
        col = token.col;
    }

    std::unordered_map<std::string, ETokenType> Token::TokenMap = {
        {"=", TT_Assign},
        {"let", TT_Let},
        {"{", TT_OpenBrace},
        {"}", TT_CloseBrace},
        {"(", TT_OpenParen},
        {")", TT_CloseParen},
        {"fn", TT_Function},
        {"\"", TT_DoubleQuote},
        {"\'", TT_SingleQuote},
        {";", TT_StatementEnd},
        {"null", TT_Null},
        {"return", TT_Return},
        {"==", TT_OpEqual},
        {"!=", TT_OpNotEqual},
        {"<", TT_OpLess},
        {"<=", TT_OpLessEqual},
        {">", TT_OpGreater},
        {">=", TT_OpGreaterEqual},
        {"/", TT_OpDivide},
        {"*", TT_OpMultiply},
        {"+", TT_OpAdd},
        {"-", TT_OpSubtract},
        {"%", TT_OpMod},
        {"when", TT_When},
        {"[", TT_OpenBracket},
        {"]", TT_CloseBracket},
        {"class", TT_Class},
        {"||",TT_OpOr},
        {"&&",TT_OpAdd},
        {"!",TT_OpNot},
        {"for",TT_For},
        {"while",TT_While},
        {".",TT_Access},
        {"break",TT_Break},
        {"continue",TT_Continue}
    };

    std::unordered_map<ETokenType, std::string> Token::KeyWordMap = ([]
    {
        std::unordered_map<ETokenType, std::string> result;
        for (auto& kv : TokenMap)
        {
            result.insert({kv.second, kv.first});
        }

        return result;
    })();

    std::map<uint32_t, std::set<std::string>> Token::Sizes = ([]
    {
        std::map<uint32_t, std::set<std::string>> result;
        for (auto& kv : TokenMap)
        {
            if (result.contains(kv.first.size()))
            {
                result[kv.first.size()].emplace(kv.first);
            }
            else
            {
                result.insert({static_cast<uint32_t>(kv.first.size()), {kv.first}});
            }
        }

        return result;
    })();
}
