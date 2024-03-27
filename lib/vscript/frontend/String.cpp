#include "vscript/frontend/String.hpp"

#include "vscript/utils.hpp"
#include "vscript/frontend/List.hpp"
#include "vscript/frontend/Null.hpp"
#include "vscript/frontend/Number.hpp"

namespace vs::frontend
{
    String::String(const std::string& str) : DynamicObject({})
    {
        _str = str;
    }

    EObjectType String::GetType() const
    {
        return OT_String;
    }

    std::string String::ToString()
    {
        return _str;
    }

    bool String::ToBoolean() const
    {
        return _str.empty();
    }

    void String::OnRefSet()
    {
        DynamicObject::OnRefSet();
        AddNativeMemberFunction("split",this,{"delimiter"},&String::Split);
        AddNativeMemberFunction("size",this,{},&String::Size);
        AddNativeMemberFunction("size",this,{},&String::Size);
    }

    TSmartPtrType<Object> String::Split(const TSmartPtrType<FunctionScope>& fnScope)
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
        
        std::vector<TSmartPtrType<Object>> items;
        for(auto &p : result)
        {
            items.emplace_back(makeString(p));
        }
        
        return makeList(items);
    }

    TSmartPtrType<Object> String::Size(const TSmartPtrType<FunctionScope>& fnScope)
    {
        return makeNumber(_str.size());
    }

    TSmartPtrType<Object> String::Trim(const TSmartPtrType<FunctionScope>& fnScope)
    {
        return makeNumber(trim(_str));
    }

    TSmartPtrType<Object> String::Get(const TSmartPtrType<Object>& key)
    {
        if(key->GetType() == OT_Number)
        {
            const auto i = key.Cast<Number>()->GetValueAs<int>();
            if(i >= _str.size())
            {
                throw std::runtime_error("Index out of range " + std::to_string(i));
            }
            
            return makeReferenceWithSetter(this->ToRef().Reserve().Cast<DynamicObject>(),makeString(_str.substr(i,1)),[this,i](const TSmartPtrType<ScopeLike>& s,const TSmartPtrType<Object>& v)
            {
                this->_str[i] = v->ToString().at(0);
            });
        }
        
        return DynamicObject::Get(key);
    }

    void String::Set(const TSmartPtrType<Object>& key, const TSmartPtrType<Object>& val)
    {
        if(key->GetType() == OT_Number)
        {
            const auto idx =key.Cast<Number>()->GetValueAs<int>();
            if(idx > 0 && idx < _str.size())
            {
                _str[key.Cast<Number>()->GetValueAs<int>()] = val->ToString().at(0);
                return;
            }
            
        }
        DynamicObject::Set(key, val);
    }

    TSmartPtrType<Object> String::Add(const TSmartPtrType<Object>& other)
    {
        return makeString(_str + other->ToString());
    }

    TSmartPtrType<Object> String::Multiply(const TSmartPtrType<Object>& other)
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

    TSmartPtrType<String> makeString(const std::string& str)
    {
        return manage<String>(str);
    }
}
