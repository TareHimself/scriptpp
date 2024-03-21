#pragma once
#include <functional>
#include <optional>
#include <vector>
#include "Token.hpp"
#include <sstream>

namespace vs::parser
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
        uint32_t lineNo = 0;
        uint32_t colNo = 0;
        TextStream(const std::string& text);
        int Peak();
        bool Get(char & result);
        bool IsEmpty();
    };

    struct Tokenizer
    {
        std::optional<RawTokens> FindNext(RawTokens& input, const std::string& search);

        std::optional<TokenMatchResult> FindNextToken(RawTokens& input);

        void TokenizeAssign(std::list<Token>& tokens, RawTokens& input);

        void TokenizeFor(std::list<Token>& tokens, RawTokens& input);

        void TokenizeWhile(std::list<Token>& tokens, RawTokens& input);
        
        void TokenizeWhenBranch(std::list<Token>& tokens, RawTokens& input);
        
        void TokenizeWhen(std::list<Token>& tokens, RawTokens& input);
    
        void TokenizeLet(std::list<Token>& tokens, RawTokens& input);

        void TokenizeLiteral(std::list<Token>& tokens, RawTokens& input);

        void TokenizeExpression(std::list<Token>& tokens, RawTokens& input);
        
        void TokenizeFunctionCall(std::list<Token>& tokens, RawTokens& input);
        
        void TokenizeFunction(std::list<Token>& tokens, RawTokens& input);

        void TokenizeClass(std::list<Token>& tokens, RawTokens& input);
    
        void TokenizeStatement(std::list<Token>& tokens, RawTokens& input);
    
        void TokenizeScope(std::list<Token>& tokens, RawTokens& input);
    
        void TokenizeAll(std::list<Token>& tokens, RawTokens& input);

        void ConsumeTill(RawTokens& result, RawTokens& input, const std::string& target, int initialScope = 0);

        std::string ConsumeTill(TextStream& data, char token);

        std::string ConsumeTill(TextStream& data, const std::string& token);

        
        
        void Preprocess(RawTokens& tokens, TextStream& data);

        void operator()(std::list<Token>& tokens,const std::string& data);
    };

    

    void tokenize(std::list<Token>& tokens,const std::string& data);
}
