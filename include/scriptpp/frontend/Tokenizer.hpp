#pragma once
#include <filesystem>
#include <functional>
#include <optional>
#include <vector>
#include "Token.hpp"
#include <sstream>

#include "TokenList.hpp"

namespace spp::frontend
{
    std::optional<Token> joinTokensTill(TokenList& tokens,const std::set<std::string>& search);
    
    bool isSplitToken(const Token& token);
    
    bool isSeparatorToken(const Token& token);
    
    TokenList tokenize(const std::string& data,const std::string& fileName);

    TokenList tokenize(const std::filesystem::path& file);
}
