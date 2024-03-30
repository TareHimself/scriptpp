#include "vscript/frontend/Null.hpp"

namespace vs::frontend
{
    std::string Null::ToString() const
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
    

    std::shared_ptr<Null> makeNull()
    {
        static auto universalNull = makeObject<Null>();
        return universalNull;
    }
}
