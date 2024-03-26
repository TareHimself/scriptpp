#pragma once
#include "Object.hpp"

namespace vs::frontend
{
    class Null : public Object
    {
    public:
        std::string ToString() override;
        bool ToBoolean() const override;
        EObjectType GetType() const override;
    };


    TSmartPtrType<Null> makeNull();
}
