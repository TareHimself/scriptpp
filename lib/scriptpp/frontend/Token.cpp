#include "scriptpp/frontend/Token.hpp"

#include <stdexcept>

#include "scriptpp/utils.hpp"

namespace spp::frontend {
    TokenDebugInfo::TokenDebugInfo(const uint32_t& inLine, const uint32_t& inCol)
    {
        line = inLine;
        col = inCol;
    }

    TokenDebugInfo::TokenDebugInfo(const std::string& inFile, const uint32_t& inLine, const uint32_t& inCol) : TokenDebugInfo(inLine,inCol)
    {
        file = inFile;
        
    }

    std::string TokenDebugInfo::ToString() const
    {
        return join({file,std::to_string(line),std::to_string(col)},":");
    }

    void RawTokens::ThrowAt(const std::string& message, const RawToken& token)
    {
        throw std::runtime_error(message + "\n" + token.debugInfo.file + " " + std::to_string(token.debugInfo.line) + ":" + std::to_string(token.debugInfo.col));
    }

    TokenList<RawToken>& RawTokens::ExpectFront(const ETokenType& token)
    {
        return ExpectFront(Token::KeyWordMap[token]);
    }

    TokenList<RawToken>& RawTokens::ExpectFront(const std::string& token)
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        if(_tokens.front().data != token)
        {
            ThrowAt("Expected " + token + " but got " + _tokens.front().data,_tokens.front());
        }

        return *this;
    }

    TokenList<RawToken>& RawTokens::ExpectBack(const ETokenType& token)
    {
        return ExpectBack(Token::KeyWordMap[token]);
    }

    TokenList<RawToken>& RawTokens::ExpectBack(const std::string& token)
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        if(_tokens.back().data != token)
        {
            ThrowAt("Expected " + token + " but got " + _tokens.back().data,_tokens.back());
        }

        return *this;
    }
    
    Token::Token(ETokenType inType, uint32_t inLine, uint32_t inCol)
    {
        type = inType;
        value = KeyWordMap.contains(type) ? KeyWordMap[type] : "";
        debugInfo.line = inLine;
        debugInfo.col = inCol;
    }

    Token::Token(ETokenType inType, const TokenDebugInfo& inDebugInfo)
    {
        debugInfo = inDebugInfo;
    }

    Token::Token(ETokenType inType, const RawToken& token)
    {
        type = inType;
        value = token.data;
        debugInfo = token.debugInfo;
    }

    Token::Token(const RawToken& token)
    {
        type = TokenMap[token.data];
        value = token.data;
        debugInfo = token.debugInfo;
    }

    void Tokenized::ThrowAt(const std::string& message, const Token& token)
    {
        throw std::runtime_error("Error at " + token.debugInfo.file + " " + std::to_string(token.debugInfo.line) + ":" + std::to_string(token.debugInfo.col) + "\n" + message);
    }

    TokenList<Token>& Tokenized::ExpectFront(const ETokenType& token)
    {
        return ExpectFront(Token::KeyWordMap[token]);
    }

    TokenList<Token>& Tokenized::ExpectFront(const std::string& token)
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        if(_tokens.front().value != token)
        {
            ThrowAt("Expected " + token + " but got " + _tokens.front().value,_tokens.front());
        }

        return *this;
    }

    TokenList<Token>& Tokenized::ExpectBack(const ETokenType& token)
    {
        return ExpectBack(Token::KeyWordMap[token]);
    }

    TokenList<Token>& Tokenized::ExpectBack(const std::string& token)
    {
        if(_tokens.empty())
        {
            ThrowExpectedInput();
        }

        if(_tokens.back().value != token)
        {
            ThrowAt("Expected " + token + " but got " + _tokens.back().value,_tokens.back());
        }

        return *this;
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
        {",",TT_Comma},
        {"break",TT_Break},
        {"continue",TT_Continue},
        {"try",TT_Try},
        {"catch",TT_Catch},
        {"throw",TT_Throw}
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
