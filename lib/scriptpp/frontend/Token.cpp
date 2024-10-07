#include "scriptpp/frontend/Token.hpp"

#include <ranges>
#include <stdexcept>

#include "scriptpp/utils.hpp"

namespace spp::frontend {
    TokenDebugInfo::TokenDebugInfo(const uint32_t& inLine, const uint32_t& inCol) : TokenDebugInfo(inLine,inCol,inLine,inCol + 1)
    {
        
    }

    TokenDebugInfo::TokenDebugInfo(const std::string& inFile, const uint32_t& inLine, const uint32_t& inCol) : TokenDebugInfo(inLine,inCol)
    {
        file = inFile;
        
    }

    TokenDebugInfo::TokenDebugInfo(const uint32_t& inStartLine, const uint32_t& inStartCol, const uint32_t& inEndLine,
        const uint32_t& inEndCol) 
    {
        startLine = inStartLine;
        endLine = inEndLine;
        startCol = inStartCol;
        endCol = inEndCol;
    }

    TokenDebugInfo::TokenDebugInfo(const std::string& inFile, const uint32_t& inStartLine, const uint32_t& inStartCol,
        const uint32_t& inEndLine, const uint32_t& inEndCol) : TokenDebugInfo(inStartLine,inStartCol,inEndLine,inEndCol)
    {
        file = inFile;
    }

    std::string TokenDebugInfo::ToString() const
    {
        return join({file,std::to_string(startLine),std::to_string(startCol)},":");
    }

    TokenDebugInfo TokenDebugInfo::operator+(const TokenDebugInfo& other) const
    {
        auto minStartLine = std::min(startLine,other.startLine);
        auto minStartCol = startLine == minStartLine && other.startLine == minStartLine ? std::min(startCol,other.startCol) : (startLine == minStartLine ? startCol : other.startCol);

        auto maxEndLine = std::max(endLine,other.endLine);
        auto maxEndCol = startLine == maxEndLine && other.endLine == maxEndLine ? std::max(endCol,other.endCol) : (endLine == maxEndLine ? endCol : other.endCol);
        
        return TokenDebugInfo{file,minStartLine,minStartCol,maxEndLine,maxEndCol};
    }

    TokenDebugInfo& TokenDebugInfo::operator+=(const TokenDebugInfo& other)
    {
        if(this == &other) return *this;

        *this = *this + other;

        return *this;
    }

    Token::Token(TokenType inType, uint32_t inLine, uint32_t inCol) : Token(inType,TokenDebugInfo{inLine,inCol})
    {
    }

    Token::Token(TokenType inType, const TokenDebugInfo& inDebugInfo) : Token(inType,KeyWordMap.contains(inType) ? KeyWordMap[inType] : "",inDebugInfo)
    {
    }

    Token::Token(TokenType inType, const Token& otherToken) : Token(inType,otherToken.value,otherToken.debugInfo)
    {

    }

    Token::Token(TokenType inType, const std::string& inValue, const TokenDebugInfo& inDebugInfo)
    {
        type = inType;
        value = inValue;
        debugInfo = inDebugInfo;
    }

    Token::Token(const std::string& data, const TokenDebugInfo& inDebugInfo)
    {
        type = TokenMap.contains(data) ? TokenMap[data] : (data == "true" || data == "false" ? TokenType::BooleanLiteral : TokenType::Unknown);
        value = data;
        debugInfo = inDebugInfo;
    }

    std::unordered_map<std::string, TokenType> Token::TokenMap = {
        {"=", TokenType::Assign},
        {"let", TokenType::Let},
        {"{", TokenType::OpenBrace},
        {"}", TokenType::CloseBrace},
        {"(", TokenType::OpenParen},
        {")", TokenType::CloseParen},
        {"fn", TokenType::Function},
        {"\"", TokenType::DoubleQuote},
        {"\'", TokenType::SingleQuote},
        {";", TokenType::StatementEnd},
        {"null", TokenType::Null},
        {"return", TokenType::Return},
        {"==", TokenType::OpEqual},
        {"!=", TokenType::OpNotEqual},
        {"<", TokenType::OpLess},
        {"<=", TokenType::OpLessEqual},
        {">", TokenType::OpGreater},
        {">=", TokenType::OpGreaterEqual},
        {"/", TokenType::OpDivide},
        {"*", TokenType::OpMultiply},
        {"+", TokenType::OpAdd},
        {"-", TokenType::OpSubtract},
        {"%", TokenType::OpMod},
        {"when", TokenType::When},
        {"[", TokenType::OpenBracket},
        {"]", TokenType::CloseBracket},
        {"proto", TokenType::Proto},
        {"||",TokenType::OpOr},
        {"&&",TokenType::OpAdd},
        {"!",TokenType::OpNot},
        {"for",TokenType::For},
        {"while",TokenType::While},
        {".",TokenType::Access},
        {",",TokenType::Comma},
        {"break",TokenType::Break},
        {"continue",TokenType::Continue},
        {"try",TokenType::Try},
        {"catch",TokenType::Catch},
        {"throw",TokenType::Throw},
        {"->",TokenType::Arrow},
        {":",TokenType::Colon}
    };

    std::unordered_map<TokenType, std::string> Token::KeyWordMap = ([]
    {
        std::unordered_map<TokenType, std::string> result;
        for (auto& kv : TokenMap)
        {
            result.insert({kv.second, kv.first});
        }

        return result;
    })();

    std::map<int, std::set<std::string>> Token::Sizes = ([]
    {
        std::map<int, std::set<std::string>> result;
        for (const auto& fst : TokenMap | std::views::keys)
        {
            if (result.contains(fst.size()))
            {
                result[fst.size()].emplace(fst);
            }
            else
            {
                result.insert({static_cast<int>(fst.size()), {fst}});
            }
        }

        return result;
    })();
}
