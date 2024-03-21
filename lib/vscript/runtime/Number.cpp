#include "vscript/runtime/Number.hpp"
#include <cmath>

#include "vscript/utils.hpp"
#include "vscript/runtime/Null.hpp"
#include "vscript/runtime/String.hpp"

namespace vs::runtime
{
    EObjectType Number::GetType() const
    {
        return EObjectType::OT_Number;
    }

    TSmartPtrType<Object> Number::Add(const TSmartPtrType<Object>& other)
    {
        if(const auto str = other.Cast<String>(); other.IsValid())
        {
            return makeString(ToString() + str->ToString());
        }
        return Object::Add(other);
    }

    TSmartPtrType<Object> Number::Subtract(const TSmartPtrType<Object>& other)
    {
        return Object::Subtract(other);
    }

    TSmartPtrType<Object> Number::Divide(const TSmartPtrType<Object>& other)
    {
        return Object::Divide(other);
    }

    TSmartPtrType<Object> Number::Multiply(const TSmartPtrType<Object>& other)
    {
        return Object::Multiply(other);
    }

    TSmartPtrType<Number> makeNumber(const std::string& num)
    {
        const auto numSize = num.starts_with("-") ? num.size() - 1 : num.size();
        
        if(isFractional(num))
        {
            if(const auto floatMax = std::to_string(std::numeric_limits<float>::max()).size(); numSize > floatMax)
            {
                return makeNumber(std::stod(num));
            }

            return makeNumber(std::stof(num));
        }

        if(const auto intMax = std::to_string(std::numeric_limits<int>::max()).size(); numSize > intMax)
        {
            return makeNumber(std::stoi(num));
        }

        return makeNumber(std::stol(num));
    }
}
