#include "vscript/frontend/String.hpp"

#include "vscript/utils.hpp"
#include "vscript/frontend/List.hpp"
#include "vscript/frontend/Null.hpp"
#include "vscript/frontend/Number.hpp"

namespace vs::frontend
{
    void String::Init()
    {
        DynamicObject::Init();
        AddNativeMemberFunction("split",this,{"delimiter"},&String::Split);
        AddNativeMemberFunction("size",this,{},&String::Size);
        AddNativeMemberFunction("size",this,{},&String::Size);
    }

    String::String(const std::string& str) : DynamicObject({})
    {
        _str = str;
    }

    EObjectType String::GetType() const
    {
        return OT_String;
    }

    std::string String::ToString() const
    {
        return _str;
    }

    bool String::ToBoolean() const
    {
        return _str.empty();
    }

    std::shared_ptr<Object> String::Split(const std::shared_ptr<FunctionScope>& fnScope)
    {
        std::vector<std::string> result;
        auto delimiter = resolveReference(fnScope->Find("delimiter"));
        if(delimiter->Equal(makeNull()))
        {
            delimiter = makeString("");
        }
        
        if (delimiter->Equal(makeNull()))
        {
            split(result, _str, delimiter->ToString());
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

    std::shared_ptr<Object> String::Get(const std::shared_ptr<Object>& key)
    {
        if(key->GetType() == OT_Number)
        {
            const auto i = cast<Number>(key)->GetValueAs<int>();
            if(i >= _str.size())
            {
                throw std::runtime_error("Index out of range " + std::to_string(i));
            }
            
            return makeReferenceWithSetter(cast<DynamicObject>(this->GetRef()),makeString(_str.substr(i,1)),[this,i](const std::shared_ptr<ScopeLike>& s,const std::shared_ptr<Object>& v)
            {
                this->_str[i] = v->ToString().at(0);
            });
        }
        
        return DynamicObject::Get(key);
    }

    void String::Set(const std::shared_ptr<Object>& key, const std::shared_ptr<Object>& val)
    {
        if(key->GetType() == OT_Number)
        {
            const auto idx =cast<Number>(key)->GetValueAs<int>();
            if(idx > 0 && idx < _str.size())
            {
                _str[idx]= val->ToString().at(0);
                return;
            }
            
        }
        DynamicObject::Set(key, val);
    }

    std::shared_ptr<Object> String::Add(const std::shared_ptr<Object>& other)
    {
        return makeString(_str + other->ToString());
    }

    std::shared_ptr<Object> String::Multiply(const std::shared_ptr<Object>& other)
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
        
        return Object::Multiply(other);
    }

    std::shared_ptr<String> makeString(const std::string& str)
    {
        return makeObject<String>(str);
    }
}
