#include "scriptpp/runtime/Boolean.hpp"

namespace spp::runtime
{
    Boolean::Boolean(const bool& val)
    {
        _value = val;
    }

    EObjectType Boolean::GetType() const
    {
        return EObjectType::Boolean;
    }

    bool Boolean::ToBoolean(const std::shared_ptr<ScopeLike>& scope) const
    {
        return _value;
    }

    std::string Boolean::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        return _value ? "true" : "false";
    }

    size_t Boolean::GetHashCode(const std::shared_ptr<ScopeLike>& scope)
    {
        return hashCombine(Object::GetHashCode(scope),_value);
    }

    std::shared_ptr<Boolean> makeBoolean(const std::string& val)
    {
        return makeObject<Boolean>(val == "true");
    }

    std::shared_ptr<Boolean> makeBoolean(const bool& val)
    {
        static auto trueObj = makeObject<Boolean>(true);
        static auto falseObj = makeObject<Boolean>(false);
        return val ? trueObj : falseObj;
    }
}
