#pragma once
#include <vector>
#include <string>

#include "runtime/Object.hpp"

namespace vs
{
    inline void split(std::vector<std::string>& result,const std::string& str,const std::string& delimiter = " ")
    {
        std::string remaining = str.substr();
        if(delimiter.empty())
        {
            for(auto &r : remaining)
            {
                result.emplace_back(1,r);
            }
            remaining = "";
        }
        else
        {
            auto pos = remaining.find_first_of(delimiter);
            while(pos != std::string::npos)
            {
                result.push_back(remaining.substr(0,pos));
                remaining = remaining.substr(pos + delimiter.size());
                pos = remaining.find_first_of(delimiter);
            }
        }

        if(!remaining.empty())
        {
            result.push_back(remaining);
        }
    }

    inline std::string trim(const std::string& str)
    {
        auto copied = str;
        constexpr auto toTrim = {' ','\n','\t','\r'};
        size_t diff = 0;
        do
        {
            const auto start = copied.size();
            for(auto &c : toTrim)
            {
                copied.erase(copied.find_last_not_of(c)+1); //suffixing chars
            }
            
            diff = copied.size() - start;
        } while(diff != 0);

        diff = 0;
        do
        {
            const auto start = copied.size();
            for(auto &c : toTrim)
            {
                copied.erase(0, copied.find_first_not_of(c)); //prefixing chars
            }
            
            diff = copied.size() - start;
        } while(diff != 0);
                 
               
        return copied;
    }

    inline bool isNum(const char c)
    {
        return c >= 48 && c <= 57;
    }

    inline bool isInteger(const std::string& str)
    {
        std::string test = str;
        if(test.starts_with('-'))
        {
            test = test.substr(1,test.size() - 1);
        }
        
        for(const auto &c : test)
        {
            if(!isNum(c))
            {
                return false;
            }
        }

        return true;
    }

    inline bool isFractional(const std::string& str)
    {
        std::string test = str;
        if(test.starts_with('-'))
        {
            test = test.substr(1,test.size() - 1);
        }

        uint32_t numFractional = 0;
        for(const auto &c : test)
        {
            if(!isNum(c))
            {
                if( c == '.')
                {
                    numFractional += 1;
                    continue;
                }
                
                return false;
            }
            
        }

        return numFractional == 1;
    }

    inline std::string join(const std::vector<std::string>& arr,const std::string& sep)
    {
        std::string result;
        for(auto i = 0; i < arr.size(); i++)
        {
            result += arr[i];
            if(i != arr.size() - 1)
            {
                result += sep;
            }
        }

        return result;
    }

    template <typename To,typename From>
std::shared_ptr<To> cast(const std::shared_ptr<From>& ptr) {
        return std::dynamic_pointer_cast<To,From>(ptr);
    }

    template <typename To,typename From>
    std::shared_ptr<To> castStatic(const std::shared_ptr<From>& ptr) {
        return std::static_pointer_cast<To,From>(ptr);
    }
    
}
