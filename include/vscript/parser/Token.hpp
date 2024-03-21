#pragma once
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace vs::parser
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
        TT_Continue
    };

    struct RawToken
    {
        std::string data;
        uint32_t line = 0;
        uint32_t col = 0;
    };

    
    struct RawTokens
    {
    protected:
        std::list<RawToken> _tokens;
    public:
        [[noreturn]] static void ThrowExpectedInput();

        [[noreturn]] static void ThrowAtToken(const std::string& message,const RawToken& token);

        RawToken RemoveFront();

        RawToken RemoveBack();

        RawTokens& ExpectFront(const ETokenType& token);
        
        RawTokens& ExpectFront(const std::string& token);

        RawTokens& ExpectBack(const ETokenType& token);
        
        RawTokens& ExpectBack(const std::string& token);

        RawToken& Front();

        RawToken& Back();

        RawToken Get();

        size_t Size() const;

        RawTokens& InsertFront(const RawTokens& tokens);

        RawTokens& InsertFront(const RawToken& token);
        
        RawTokens& InsertBack(const RawToken& token);

        operator bool() const;
    };

    struct Token
    {
        ETokenType type;
        std::string value;
        uint32_t line;
        uint32_t col;

        Token(ETokenType inType,uint32_t inLine,uint32_t inCol);

        Token(ETokenType inType,const RawToken& token);

        Token(const RawToken& token);

        static std::unordered_map<std::string, ETokenType> TokenMap;

        static std::unordered_map<ETokenType, std::string> KeyWordMap;

        static std::map<uint32_t, std::set<std::string>> Sizes;
    };
}
