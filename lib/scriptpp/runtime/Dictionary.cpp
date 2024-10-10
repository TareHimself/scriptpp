#include "scriptpp/runtime/Dictionary.hpp"

#include "scriptpp/runtime/Boolean.hpp"
#include "scriptpp/runtime/Null.hpp"
#include "scriptpp/runtime/String.hpp"

namespace spp::runtime
{
    Dictionary::Dictionary() : DynamicObject(makeScope())
    {
    }

    Dictionary::Dictionary(const std::unordered_map<std::shared_ptr<Object>, std::shared_ptr<Object>>& data) : DynamicObject(makeScope())
    {
        _entries = data;
    }

    std::shared_ptr<DictionaryPrototype> Dictionary::Prototype = makeObject<DictionaryPrototype>();

    void Dictionary::Init()
    {
        DynamicObject::Init();
        AddNativeMemberFunction("put",this,vectorOf<std::string>("key","item"),&Dictionary::PutItem);
        AddNativeMemberFunction("get",this,vectorOf<std::string>("key"),&Dictionary::GetItem);
        AddNativeMemberFunction("has",this,vectorOf<std::string>("key"),&Dictionary::HasItem);
    }

    std::shared_ptr<Object> Dictionary::PutItem(const std::shared_ptr<FunctionScope>& fnScope)
    {
        const auto key = fnScope->GetArgument(0);
        auto val = fnScope->GetArgument(1);
        _entries.insert_or_assign(key,val);
        return this->GetRef();
    }

    std::shared_ptr<Object> Dictionary::GetItem(const std::shared_ptr<FunctionScope>& fnScope)
    {
        auto key = fnScope->GetArgument(0);
        
        if(_entries.contains(key))
        {
            return _entries.at(key);
        }

        return makeNull();
    }

    std::shared_ptr<Object> Dictionary::HasItem(const std::shared_ptr<FunctionScope>& fnScope)
    {
        auto key = fnScope->GetArgument(0);
        return makeBoolean(_entries.contains(key));
    }

    std::shared_ptr<Object> Dictionary::Get(const std::shared_ptr<Object>& key,
                                            const std::shared_ptr<ScopeLike>& scope) const
    {
        if(_entries.contains(key))
        {
            return _entries.at(key);
        }
        
        return DynamicObject::Get(key, scope);
    }

    std::string Dictionary::ToString(const std::shared_ptr<ScopeLike>& scopeLike) const
    {
        std::string result = "{ ";

        for(auto it = _entries.begin(); it != _entries.end(); ++it)
        {
            const auto [key,val] = *it;
            
            result += key->ToString(scopeLike) + " => " +  val->ToString(scopeLike);
            
            if(std::next(it) != _entries.end())
            {
                result += " , ";
            }
        }
        
        result += " }";
        
        return result;
    }

    std::shared_ptr<Dictionary> makeDictionary(const std::unordered_map<std::shared_ptr<Object>, std::shared_ptr<Object>>& data)
    {
        return makeObject<Dictionary>(data);
    }

    std::shared_ptr<Dictionary> makeDictionary(const std::unordered_map<std::string, std::shared_ptr<Object>>& data)
    {
        std::unordered_map<std::shared_ptr<Object>, std::shared_ptr<Object>> mapData{};
        
        for (auto &[id,obj] : data)
        {
            mapData.insert_or_assign(makeString(id),obj);
        }

        return makeDictionary(mapData);
    }

    std::shared_ptr<Dictionary> makeDictionary()
    {
        return makeObject<Dictionary>(); 
    }

    DictionaryPrototype::DictionaryPrototype() : Prototype(makeScope(), makeNativeFunction(
                                                               {}, ReservedDynamicFunctions::CALL,vectorOf<std::string>(),
                                                               [](const std::shared_ptr<FunctionScope>& fnScope)
                                                               {
                                                                   return makeDictionary();
                                                               }))
    {
        
    }

    std::string DictionaryPrototype::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        return "<Prototype : Dict>";
    }
}
