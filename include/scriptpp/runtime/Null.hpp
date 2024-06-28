#pragma once
#include "Object.hpp"

namespace spp::runtime
{
    class Null : public Object
    {
    public:
        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;
        bool ToBoolean(const std::shared_ptr<ScopeLike>& scope) const override;
        EObjectType GetType() const override;
    };


    std::shared_ptr<Null> makeNull();
}
