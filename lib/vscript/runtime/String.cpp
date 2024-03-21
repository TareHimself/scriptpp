#include "vscript/runtime/String.hpp"

#include "vscript/runtime/Number.hpp"

namespace vs::runtime
{
    String::String(const std::string& str)
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
