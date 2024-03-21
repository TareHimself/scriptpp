#pragma once
#include "Object.hpp"

namespace vs::runtime
{
    class Boolean : public Object
    {
        bool _value;
    public:
        Boolean(const bool&val);
        EObjectType GetType() const override;
        bool ToBoolean() const override;
        std::string ToString() override;
        
    };


    TSmartPtrType<Boolean> makeBoolean(const std::string& val);

    TSmartPtrType<Boolean> makeBoolean(const bool& val);
}
