#pragma once
#include "Object.hpp"

namespace vs::frontend
{
    class Null : public Object
    {
    public:
        std::string ToString() const override;
        bool ToBoolean() const override;
        EObjectType GetType() const override;
    };


    std::shared_ptr<Null> makeNull();
}
