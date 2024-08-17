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

    Token::Token(ETokenType inType, uint32_t inLine, uint32_t inCol) : Token(inType,TokenDebugInfo{inLine,inCol})
    {
    }

    Token::Token(ETokenType inType, const TokenDebugInfo& inDebugInfo) : Token(inType,KeyWordMap.contains(inType) ? KeyWordMap[inType] : "",inDebugInfo)
    {
    }

    Token::Token(ETokenType inType, const Token& otherToken) : Token(inType,otherToken.value,otherToken.debugInfo)
    {

    }

    Token::Token(ETokenType inType, const std::string& inValue, const TokenDebugInfo& inDebugInfo)
    {
        type = inType;
        value = inValue;
        debugInfo = inDebugInfo;
    }

    Token::Token(const std::string& data, const TokenDebugInfo& inDebugInfo)
    {
        type = TokenMap.contains(data) ? TokenMap[data] : (data == "true" || data == "false" ? ETokenType::BooleanLiteral : ETokenType::Unknown);
        value = data;
        debugInfo = inDebugInfo;
    }

    std::unordered_map<std::string, ETokenType> Token::TokenMap = {
        {"=", ETokenType::Assign},
        {"let", ETokenType::Let},
        {"{", ETokenType::OpenBrace},
        {"}", ETokenType::CloseBrace},
        {"(", ETokenType::OpenParen},
        {")", ETokenType::CloseParen},
        {"fn", ETokenType::Function},
        {"\"", ETokenType::DoubleQuote},
        {"\'", ETokenType::SingleQuote},
        {";", ETokenType::StatementEnd},
        {"null", ETokenType::Null},
        {"return", ETokenType::Return},
        {"==", ETokenType::OpEqual},
        {"!=", ETokenType::OpNotEqual},
        {"<", ETokenType::OpLess},
        {"<=", ETokenType::OpLessEqual},
        {">", ETokenType::OpGreater},
        {">=", ETokenType::OpGreaterEqual},
        {"/", ETokenType::OpDivide},
        {"*", ETokenType::OpMultiply},
        {"+", ETokenType::OpAdd},
        {"-", ETokenType::OpSubtract},
        {"%", ETokenType::OpMod},
        {"when", ETokenType::When},
        {"[", ETokenType::OpenBracket},
        {"]", ETokenType::CloseBracket},
        {"proto", ETokenType::Proto},
        {"||",ETokenType::OpOr},
        {"&&",ETokenType::OpAdd},
        {"!",ETokenType::OpNot},
        {"for",ETokenType::For},
        {"while",ETokenType::While},
        {".",ETokenType::Access},
        {",",ETokenType::Comma},
        {"break",ETokenType::Break},
        {"continue",ETokenType::Continue},
        {"try",ETokenType::Try},
        {"catch",ETokenType::Catch},
        {"throw",ETokenType::Throw},
        {"->",ETokenType::Arrow}
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
