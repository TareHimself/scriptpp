#include "vscript/frontend/Number.hpp"
#include <cmath>

#include "vscript/utils.hpp"
#include "vscript/frontend/Null.hpp"
#include "vscript/frontend/String.hpp"

namespace vs::frontend
{
    EObjectType Number::GetType() const
    {
        return EObjectType::OT_Number;
    }

    std::shared_ptr<Object> Number::Add(const std::shared_ptr<Object>& other)
    {
        if(const auto str = cast<String>(other))
        {
            return makeString(ToString() + str->ToString());
        }
        return Object::Add(other);
    }

    std::shared_ptr<Object> Number::Subtract(const std::shared_ptr<Object>& other)
    {
        return Object::Subtract(other);
    }

    std::shared_ptr<Object> Number::Divide(const std::shared_ptr<Object>& other)
    {
        return Object::Divide(other);
    }

    std::shared_ptr<Object> Number::Multiply(const std::shared_ptr<Object>& other)
    {
        return Object::Multiply(other);
    }

    std::shared_ptr<Number> makeNumber(const std::string& num)
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
