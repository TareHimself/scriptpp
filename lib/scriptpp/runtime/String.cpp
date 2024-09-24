#include "scriptpp/runtime/String.hpp"

#include "scriptpp/utils.hpp"
#include "scriptpp/runtime/List.hpp"
#include "scriptpp/runtime/Null.hpp"
#include "scriptpp/runtime/Number.hpp"

namespace spp::runtime
{
    void String::Init()
    {
        DynamicObject::Init();
        AddNativeMemberFunction("split",this,{"delimiter"},&String::Split);
        AddNativeMemberFunction("size",this,vectorOf<std::string>(),&String::Size);
        AddNativeMemberFunction("size",this,vectorOf<std::string>(),&String::Size);
    }

    String::String(const std::string& str) : DynamicObject({})
    {
        _str = str;
    }

    EObjectType String::GetType() const
    {
        return EObjectType::String;
    }

    std::string String::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        
        return _str;
    }

    bool String::ToBoolean(const std::shared_ptr<ScopeLike>& scope) const
    {
        return _str.empty();
    }

    bool String::Equal(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const
    {
        return _str == other->ToString(scope);
    }

    std::shared_ptr<Object> String::Split(const std::shared_ptr<FunctionScope>& fnScope)
    {
        std::vector<std::string> result;
        auto delimiter = resolveReference(fnScope->Find("delimiter"));
        if(delimiter->Equal(makeNull(),fnScope))
        {
            delimiter = makeString("");
        }
        
        if (delimiter->Equal(makeNull(), fnScope))
        {
            split(result, _str, delimiter->ToString(fnScope));
        }
        else
        {
            split(result, _str,"");
        }
        
        std::vector<std::shared_ptr<Object>> items;
        for(auto &p : result)
        {
            items.emplace_back(makeString(p));
        }
        
        return makeList(items);
    }

    std::shared_ptr<Object> String::Size(const std::shared_ptr<FunctionScope>& fnScope)
    {
        return makeNumber(_str.size());
    }

    std::shared_ptr<Object> String::Trim(const std::shared_ptr<FunctionScope>& fnScope)
    {
        return makeNumber(trim(_str));
    }

    std::shared_ptr<Object> String::Get(const std::shared_ptr<Object>& key, const std::shared_ptr<ScopeLike>& scope) const
    {
        if(key->GetType() == EObjectType::Number)
        {
            const auto i = cast<Number>(key)->GetValueAs<int>();
            if(i >= _str.size())
            {
                throw std::runtime_error("Index out of range " + std::to_string(i));
            }
            
            return makeReferenceWithSetter(cast<DynamicObject>(this->GetRef()),makeString(_str.substr(i,1)),[this,i](const std::shared_ptr<ScopeLike>& s,const std::shared_ptr<Object>& v)
            {
                const_cast<String*>(this)->_str[i] = v->ToString().at(0);
            });
        }
        
        return DynamicObject::Get(key,scope);
    }

    void String::Set(const std::shared_ptr<Object>& key, const std::shared_ptr<Object>& val, const std::shared_ptr<ScopeLike>& scope)
    {
        if(key->GetType() == EObjectType::Number)
        {
            const auto idx =cast<Number>(key)->GetValueAs<int>();
            if(idx > 0 && idx < _str.size())
            {
                _str[idx]= val->ToString().at(0);
                return;
            }
            
        }
        DynamicObject::Set(key, val, scope);
    }

    size_t String::GetHashCode(const std::shared_ptr<ScopeLike>& scope)
    {
        return hashCombine(GetType(),_str);
    }

    std::shared_ptr<Object> String::Add(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        return makeString(_str + other->ToString(scope));
    }

    std::shared_ptr<Object> String::Multiply(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        TNUMBER_SANITY_MACRO({
            auto d = static_cast<int>(o->GetValue());
            std::string total;
            for(auto i = 0; i < d; i++)
            {
                total += _str;
            }

            return makeString(total);
        })
        
        return DynamicObject::Multiply(other, scope);
    }

    std::shared_ptr<String> makeString(const std::string& str)
    {
        return makeObject<String>(str);
    }
}
