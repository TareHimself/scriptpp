#include "scriptpp/frontend/tokenizer.hpp"

#include <fstream>
#include <ranges>
#include "scriptpp/utils.hpp"

namespace spp::frontend
{
    std::optional<Token> joinTokensTill(TokenList& tokens, const std::set<std::string>& search)
    {
        if(!tokens) return {};

        if(search.contains(tokens.Front().value)) return {};

        auto pending = tokens.RemoveFront();
        
        if(search.contains(pending.value)) return pending;
        
        while(tokens)
        {
            auto front = tokens.Front();
            pending = Token{pending.value + front.value,pending.debugInfo + front.debugInfo};
            tokens.RemoveFront();
            for (auto &item : search)
            {
                if(pending.value.size() >= item.size() && pending.value.substr(pending.value.size() - item.size(),item.size()) == item)
                {
                    pending.value = pending.value.substr(0,pending.value.size() - item.size());
                    pending.debugInfo.endCol -= item.size();
                    return pending;
                }
            } 
        }

        return pending;
    }

    bool isSplitToken(const Token& token)
    {
        switch (token.type)
        {
        case TokenType::OpenBrace:
        case TokenType::OpenParen:
        case TokenType::OpenBracket:
        case TokenType::CloseBrace:
        case TokenType::CloseParen:
        case TokenType::CloseBracket:
        case TokenType::Comma:
        case TokenType::Colon:
        case TokenType::StatementEnd:
            return true;
        default:
            return false;
        }
    }

    bool isSeparatorToken(const Token& token)
    {
        switch (token.type)
        {
        case TokenType::OpAdd:
        case TokenType::OpSubtract:
        case TokenType::OpDivide:
        case TokenType::OpMultiply:
        case TokenType::OpAnd:
        case TokenType::OpOr:
        case TokenType::OpNot:
        case TokenType::OpLess:
        case TokenType::OpGreater:
        case TokenType::Access:
            return true;
            default:
                return isSplitToken(token) || token.value == " " || token.value == "\n" || token.value == "\r";
        }
        
    }

    TokenList tokenize(const std::string& data, const std::string& fileName)
    {
        TokenList rawTokens{};
        {
            std::stringstream dStream{data};
            uint32_t lineNo = 1;
            std::string line{};
            while(std::getline(dStream,line))
            {
                uint32_t colNo = 1;
                for (const char& a : line)
                {
                    rawTokens.InsertBack({std::string{a},TokenDebugInfo{fileName,lineNo,colNo}});
                    colNo++;
                }
                
                lineNo++;
            }
        }

        TokenList result{};
        
        while(rawTokens)
        {
            auto curToken = rawTokens.Front();

            if(curToken.value == "/")
            {
                rawTokens.RemoveFront();
                
                if(rawTokens)
                {
                    auto nextToken = rawTokens.Front();
                    if(nextToken.value == "/")
                    {
                        rawTokens.RemoveFront();
                        auto startLine = nextToken.debugInfo.startLine;
                        auto combined = rawTokens.RemoveFront();
                        while(rawTokens && rawTokens.Front().debugInfo.startLine == startLine)
                        {
                            auto next = rawTokens.RemoveFront();
                            combined = Token{combined.value + next.value,combined.debugInfo + next.debugInfo};
                        }
                        //result.InsertBack(Token{TokenType::StringLiteral,combined});
                        continue;
                    }

                    if(nextToken.value == "*")
                    {
                        rawTokens.RemoveFront();
                        joinTokensTill(rawTokens,std::set<std::string>{"*/"});
                        continue;
                    }
                }

                rawTokens.InsertFront(curToken);
            }
            
            if(curToken.value == " " || curToken.value == "\n" || curToken.value == "\r")
            {
                rawTokens.RemoveFront();
                continue;
            }

            if(curToken.value == "\"" || curToken.value == "\'")
            {
                auto tok = rawTokens.RemoveFront();
                auto consumed = joinTokensTill(rawTokens,std::set{tok.value});
                if(consumed.has_value())
                {
                    auto consumedTok = consumed.value();
                    
                    result.InsertBack({TokenType::StringLiteral,consumedTok.value,consumedTok.debugInfo});
                }
            }

            auto maxSize = std::ranges::reverse_view(Token::Sizes).begin()->first;

            std::string combinedStr{};
            std::list<Token> searchTokens{};

            while(combinedStr.size() < static_cast<std::string::size_type>(maxSize) && rawTokens)
            {
                auto front = rawTokens.Front();
                searchTokens.push_back(rawTokens.RemoveFront());
                combinedStr += front.value;
            }

            auto matchedSize = false;

            for(const auto& [size,matches] : std::ranges::reverse_view(Token::Sizes))
            {
                while(static_cast<int>(combinedStr.size()) > size)
                {
                    rawTokens.InsertFront(searchTokens.back());
                    combinedStr = combinedStr.substr(0,combinedStr.size() - searchTokens.back().value.size());
                    searchTokens.pop_back();
                }

                if(static_cast<int>(combinedStr.size()) < size) continue;

                if(matches.contains(combinedStr))
                {
                    matchedSize = true;
                    break;
                }
            }

             if(matchedSize)
             {
                 auto debugSpan = searchTokens.front().debugInfo;
                 searchTokens.pop_front();
                 while(!searchTokens.empty())
                 {
                     debugSpan += searchTokens.front().debugInfo;
                     searchTokens.pop_front();
                 }

                 result.InsertBack({combinedStr,debugSpan});
             }
             else
             {
                 if(!searchTokens.empty())
                {
                    while(!searchTokens.empty())
                    {
                        rawTokens.InsertFront(searchTokens.back());
                        searchTokens.pop_back();
                    }
                }
                
                combinedStr.clear();

                searchTokens.push_back(rawTokens.RemoveFront());
                combinedStr += searchTokens.back().value;
                auto debugSpan = searchTokens.front().debugInfo;

                if(isInteger(searchTokens.front().value))
                {
                    while(rawTokens && isInteger(rawTokens.Front().value))
                    {
                        auto tok = rawTokens.RemoveFront();
                        combinedStr += tok.value;
                        debugSpan += tok.debugInfo;
                        searchTokens.push_back(tok);
                    }

                    if(rawTokens && rawTokens.Front().value == ".")
                    {
                        auto tok = rawTokens.RemoveFront();
                        combinedStr += tok.value;
                        debugSpan += tok.debugInfo;
                        searchTokens.emplace_back(tok);
                        
                        while(!rawTokens && isInteger(rawTokens.Front().value))
                        {
                            tok = rawTokens.RemoveFront();
                            combinedStr += tok.value;
                            debugSpan += tok.debugInfo;
                            searchTokens.push_back(tok);
                        }
                    }

                    result.InsertBack(Token{TokenType::NumericLiteral,combinedStr,debugSpan});
                    continue;
                }

                while(rawTokens && !isSeparatorToken(rawTokens.Front()))
                {
                    auto tok = rawTokens.RemoveFront();
                    combinedStr += tok.value;
                    debugSpan += tok.debugInfo;
                    
                    searchTokens.push_back(tok);
                }
                
                result.InsertBack(Token{combinedStr,debugSpan});
                searchTokens.pop_front();
            }
        }
        
        return result;
    }

    TokenList tokenize(const std::filesystem::path& file)
    {
        std::ifstream fileStream(file, std::ios::binary);
        const std::string fileContent((std::istreambuf_iterator<char>(fileStream)),
                                      std::istreambuf_iterator<char>());
        return tokenize(fileContent,file.string());
    }
}
