#pragma once
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "TokenType.hpp"

namespace spp::frontend
{
    struct TokenDebugInfo
    {
        std::string file = "<unknown>";
        uint32_t startLine = 0;
        uint32_t startCol = 0;
        uint32_t endLine = 0;
        uint32_t endCol = 0;
        TokenDebugInfo() = default;
        TokenDebugInfo(const uint32_t& inLine,const uint32_t& inCol);
        TokenDebugInfo(const std::string& inFile,const uint32_t& inLine,const uint32_t& inCol);

        TokenDebugInfo(const uint32_t& inStartLine,const uint32_t& inStartCol,const uint32_t& inEndLine,const uint32_t& inEndCol);
        TokenDebugInfo(const std::string& inFile,const uint32_t& inStartLine,const uint32_t& inStartCol,const uint32_t& inEndLine,const uint32_t& inEndCol);

        std::string ToString() const;

        TokenDebugInfo operator +(const TokenDebugInfo& other) const;

        TokenDebugInfo& operator +=(const TokenDebugInfo& other);
    };

    struct Token
    {
        TokenType type;
        std::string value;
        TokenDebugInfo debugInfo;
        
        Token(TokenType inType,uint32_t inLine,uint32_t inCol);

        Token(TokenType inType,const TokenDebugInfo& inDebugInfo);

        Token(TokenType inType,const Token& otherToken);

        Token(TokenType inType,const std::string& inValue,const TokenDebugInfo& inDebugInfo); 
        
        
        Token(const std::string& data,const TokenDebugInfo& inDebugInfo);

        static std::unordered_map<std::string, TokenType> TokenMap;

        static std::unordered_map<TokenType, std::string> KeyWordMap;

        static std::map<int, std::set<std::string>> Sizes;
    };

    
    
}
