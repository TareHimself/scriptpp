#pragma once
#include "Object.hpp"

namespace vs::frontend
{
    class Boolean : public Object
    {
        bool _value;
    public:
        Boolean(const bool&val);
        EObjectType GetType() const override;
        bool ToBoolean() const override;
        std::string ToString() const override;
        
    };


    std::shared_ptr<Boolean> makeBoolean(const std::string& val);
    
    std::shared_ptr<Boolean> makeBoolean(const bool& val);
}
