#pragma once
#include <filesystem>
#include <functional>
#include <optional>
#include <vector>
#include "Token.hpp"
#include <sstream>

namespace vs::frontend
{
    struct SourceStream
    {
    private:
        std::string _data;
        size_t _pos = 0;
        std::list<size_t> _actions;
        size_t _actionStack = 0;

    protected:

    public:

        size_t GetPos() const
        {
            return _pos;
        }

        void SetPos(size_t pos)
        {
            _pos = pos;
        }
        
        template <typename T,typename = std::enable_if_t<!std::is_void_v<T>>>
        T WithActionStack(const std::function<T(size_t&)>& operation)
        {
            ++_actionStack;
            size_t actions = 0;
            auto r = operation(actions);
            if (--_actionStack == 0)
            {
                _actions.push_front(actions);
            }

            return r;
        }

        void WithActionStack(const std::function<void(size_t&)>& operation)
        {
            ++_actionStack;
            size_t actions = 0;
            operation(actions);
            if (--_actionStack == 0)
            {
                _actions.push_front(actions);
            }
        }

        SourceStream() = default;

        SourceStream(const std::string& data)
        {
            _data = data;
        }

        size_t Remaining() const
        {
            return _data.size() - _pos;
        }

        operator bool() const
        {
            return Remaining() > 0;
        }

        char Peak(size_t n = 0) const
        {
            return _data[_pos + n];
        }

        std::string Peak(size_t n, size_t window) const
        {
            return _data.substr(_pos + n, window);
        }

        bool NextRaw(char& result)
        {
            if (_pos >= _data.size())
            {
                return false;
            }

            result = _data.at(_pos);

            _pos++;

            return true;
        }

        bool Next(char& result)
        {
            return WithActionStack<bool>([&](size_t& actions)
            {
                while (Peak() == ' ' || Peak() == '\n' || Peak() == '\r')
                {
                    char tok;
                    if (!NextRaw(tok))
                    {
                        return false;
                    }
                    ++actions;
                }

                if (NextRaw(result))
                {
                    actions++;
                    return true;
                }

                return false;
            });
        }

        bool Next(std::string& result)
        {
            return WithActionStack<bool>([&](size_t& ac)
            {
                char temp;

                while (Peak() == ' ' || Peak() == '\n' || Peak() == '\r')
                {
                    SkipRaw();
                    ac++;
                }

                while (Peak() != ' ' && Peak() != '\n' && Peak() != '\r')
                {
                    if (!NextRaw(temp))
                    {
                        return false;
                    }

                    ac++;
                    result += temp;
                }

                if (!NextRaw(temp))
                {
                    return false;
                }

                ++ac;

                result += temp;

                return true;
            });
        }

        void SkipRaw(size_t n = 1)
        {
            WithActionStack([&](size_t& ac)
            {
                char tok;
                for (auto i = 0; i < n; i++)
                {
                    if (!NextRaw(tok))
                    {
                        break;
                    }
                    ++ac;
                }
            });
        }

        void Skip(size_t n = 1)
        {
            WithActionStack([&](size_t& ac)
            {
                char tok;
                for (auto i = 0; i < n; i++)
                {
                    if (!Next(tok))
                    {
                        break;
                    }
                }
            });
        }

        void Undo(size_t n = 1)
        {
            for (auto i = 0; i < n; i++)
            {
                if (_actions.empty())
                {
                    break;
                }

                _pos = std::max(_pos - _actions.front(), static_cast<size_t>(0));
                _actions.pop_front();
            }
        }

        bool operator>>(char& dest)
        {
            return Next(dest);
        }

        bool operator>>(std::string& dest)
        {
            return Next(dest);
        }
    };

    struct TokenMatchResult
    {
        RawTokens before;
        ETokenType type;
        RawToken token;
    };
    
    struct TextStream
    {
    private:
        std::list<std::string> _lines;
        std::stringstream _currentLine;
    public:
        std::string sourcePath = TokenDebugInfo().file;
        uint32_t lineNo = 0;
        uint32_t colNo = 0;
        TextStream() = default;
        explicit TextStream(const std::filesystem::path& path);
        explicit TextStream(const std::string& data,const std::string& file);
        void PopulateFromString(const std::string& text);
        int Peak();
        bool Get(char & result);
        bool IsEmpty();
    };

    struct Tokenizer
    {
        std::optional<RawTokens> FindNext(RawTokens& input, const std::string& search);

        std::optional<TokenMatchResult> FindNextToken(RawTokens& input);

        void TokenizeAssign(Tokenized& tokens, RawTokens& input);

        void TokenizeFor(Tokenized& tokens, RawTokens& input);

        void TokenizeWhile(Tokenized& tokens, RawTokens& input);
        
        void TokenizeWhenBranch(Tokenized& tokens, RawTokens& input);
        
        void TokenizeWhen(Tokenized& tokens, RawTokens& input);
    
        void TokenizeLet(Tokenized& tokens, RawTokens& input);

        void TokenizeLiteral(Tokenized& tokens, RawTokens& input);

        void TokenizeExpression(Tokenized& tokens, RawTokens& input);
        
        void TokenizeFunctionCall(Tokenized& tokens, RawTokens& input);
        
        void TokenizeFunction(Tokenized& tokens, RawTokens& input);

        void TokenizeClass(Tokenized& tokens, RawTokens& input);

        void TokenizeTryCatch(Tokenized& tokens,RawTokens& input);
    
        void TokenizeStatement(Tokenized& tokens, RawTokens& input);
    
        void TokenizeScope(Tokenized& tokens, RawTokens& input);
    
        void TokenizeAll(Tokenized& tokens, RawTokens& input);

        void ConsumeTill(RawTokens& result, RawTokens& input, const std::string& target, int initialScope = 0);

        std::string ConsumeTill(TextStream& data, const std::set<char>& tokens);

        std::string ConsumeTill(TextStream& data, const std::string& token);

        
        
        void Preprocess(RawTokens& tokens, TextStream& data);

        void operator()(Tokenized& tokens,const std::string& data,const std::string& fileName);

        void operator()(Tokenized& tokens,const std::filesystem::path& file);
    };

    void tokenize(Tokenized& tokens,const std::string& data,const std::string& fileName);

    void tokenize(Tokenized& tokens,const std::filesystem::path& file);
}
