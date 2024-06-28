#include "scriptpp/runtime/Null.hpp"

namespace spp::runtime
{
    std::string Null::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        return "null";
    }

    bool Null::ToBoolean(const std::shared_ptr<ScopeLike>& scope) const
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
