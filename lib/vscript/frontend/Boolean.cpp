#include "vscript/frontend/Boolean.hpp"

namespace vs::frontend
{
    Boolean::Boolean(const bool& val)
    {
        _value = val;
    }

    EObjectType Boolean::GetType() const
    {
        return OT_Boolean;
    }

    bool Boolean::ToBoolean() const
    {
        return _value;
    }

    std::string Boolean::ToString()
    {
        return _value ? "true" : "false";
    }

    TSmartPtrType<Boolean> makeBoolean(const std::string& val)
    {
        return manage<Boolean>(val == "true");
    }

    TSmartPtrType<Boolean> makeBoolean(const bool& val)
    {
        return manage<Boolean>(val);
    }
}
