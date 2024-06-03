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
        bool ToBoolean(const std::shared_ptr<ScopeLike>& scope) const override;
        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;
        
    };


    std::shared_ptr<Boolean> makeBoolean(const std::string& val);
    
    std::shared_ptr<Boolean> makeBoolean(const bool& val);
}
