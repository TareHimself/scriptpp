#include "vscript/runtime/Null.hpp"

namespace vs::runtime
{
    std::string Null::ToString()
    {
        return "null";
    }

    bool Null::ToBoolean() const
    {
        return false;
    }

    EObjectType Null::GetType() const
    {
        return EObjectType::OT_Null;
    }
    

    TSmartPtrType<Null> makeNull()
    {
        static auto universalNull = manage<Null>();
        return universalNull;
    }
}
