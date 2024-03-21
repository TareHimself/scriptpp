#include "vscript/runtime/List.hpp"
#include "vscript/runtime/Null.hpp"
#include "vscript/runtime/Number.hpp"
#include "vscript/runtime/Number.hpp"

namespace vs::runtime
{
    List::List(const std::vector<TSmartPtrType<Object>>& vec) : DynamicObject(makeScope())
    {
        _vec = vec;

        AddNativeMemberFunction("pop",this,{},&List::Pop);
        AddNativeMemberFunction("push",this,{},&List::Push);
        AddNativeMemberFunction("map",this,{"callback"},&List::Map);
        AddNativeMemberFunction("forEach",this,{"callback"},&List::ForEach);
        AddNativeMemberFunction("filter",this,{"callback"},&List::Filter);
        AddNativeMemberFunction("find",this,{"callback"},&List::Find);
        AddNativeMemberFunction("findIndex",this,{"callback"},&List::FindIndex);
        AddNativeMemberFunction("size",this,{},&List::Size);
    }

    std::string List::ToString()
    {
        std::string result = "[";
        for(auto i = 0; i < _vec.size(); i++)
        {
            result += _vec.at(i)->ToString();
            if(i != _vec.size() - 1)
            {
                result += " , ";
            }
        }

        result += "]";
        return result;
    }

    bool List::ToBoolean() const
    {
        return !_vec.empty();
    }

    TSmartPtrType<Object> List::Get(const TSmartPtrType<Object>& key)
    {
        if(key->GetType() == OT_Number)
        {
            const auto i = key.Cast<Number>()->GetValueAs<int>();
            if(i >= _vec.size())
            {
                throw std::runtime_error("Index out of range " + std::to_string(i));
            }

            return _vec[i];
        }
        return DynamicObject::Get(key);
    }

    void List::Set(const TSmartPtrType<Object>& key, const TSmartPtrType<Object>& val)
    {
        if(key->GetType() == OT_Number)
        {
            const auto i = key.Cast<Number>()->GetValueAs<int>();
            if(i >= _vec.size())
            {
                throw std::runtime_error("Index out of range " + std::to_string(i));
            }

            _vec[i] = val;
            return;
        }
        DynamicObject::Set(key, val);
    }

    TSmartPtrType<Object> List::Push(const TSmartPtrType<FunctionScope>& fnScope)
    {
        for(auto &arg : fnScope->GetArgs())
        {
            _vec.push_back(arg);
        }
        
        return this->ToRef().Reserve();
    }

    TSmartPtrType<Object> List::Pop(const TSmartPtrType<FunctionScope>& fnScope)
    {
        if(_vec.empty())
        {
            throw std::runtime_error("Attempted to pop from empty list");
        }

        auto last = _vec.back();

        _vec.pop_back();

        return last;
    }

    TSmartPtrType<Object> List::Map(const TSmartPtrType<FunctionScope>& fnScope)
    {
        const auto args = fnScope->GetArgs();
        if (args.empty())
        {
            throw std::runtime_error("No Callback passed to findIndex");
        }

        if (auto fn = args[0].Cast<Function>(); fn.IsValid())
        {
            const auto myRef = this->ToRef().Reserve();
            std::vector<TSmartPtrType<Object>> mapped;
            for (auto i = 0; i < _vec.size(); i++)
            {
                mapped.push_back(fn->Call(_vec.at(i), makeNumber(i),myRef));
            }

            return makeList(mapped);
        }

        return makeNull();
    }

    TSmartPtrType<Object> List::ForEach(const TSmartPtrType<FunctionScope>& fnScope)
    {
        const auto args = fnScope->GetArgs();
        if (args.empty())
        {
            throw std::runtime_error("No Callback passed to findIndex");
        }

        if (auto fn = args[0].Cast<Function>(); fn.IsValid())
        {
            const auto myRef = this->ToRef().Reserve();
            for (auto i = 0; i < _vec.size(); i++)
            {
                fn->Call(_vec.at(i), makeNumber(i),myRef);
            }
        }

        return makeNull();
    }

    TSmartPtrType<Object> List::Filter(const TSmartPtrType<FunctionScope>& fnScope)
    {
        const auto args = fnScope->GetArgs();
        if (args.empty())
        {
            throw std::runtime_error("No Callback passed to findIndex");
        }

        if (auto fn = args[0].Cast<Function>(); fn.IsValid())
        {
            const auto myRef = this->ToRef().Reserve();
            std::vector<TSmartPtrType<Object>> filtered;
            for (auto i = 0; i < _vec.size(); i++)
            {
                if (fn->Call(_vec.at(i), makeNumber(i),myRef)->ToBoolean())
                {
                    filtered.push_back(_vec.at(i));
                }
            }

            return makeList(filtered);
        }

        return makeNull();
    }

    TSmartPtrType<Object> List::Find(const TSmartPtrType<FunctionScope>& fnScope)
    {
        const auto args = fnScope->GetArgs();
        if (args.empty())
        {
            throw std::runtime_error("No Callback passed to find");
        }

        if (auto fn = args[0].Cast<Function>(); fn.IsValid())
        {
            const auto myRef = this->ToRef().Reserve();
            for (auto i = 0; i < _vec.size(); i++)
            {
                if (fn->Call(_vec.at(i), makeNumber(i),myRef)->ToBoolean())
                {
                    return _vec.at(i);
                }
            }
        }

        return makeNull();
    }

    TSmartPtrType<Object> List::FindIndex(const TSmartPtrType<FunctionScope>& fnScope)
    {
        const auto args = fnScope->GetArgs();
        if (args.empty())
        {
            throw std::runtime_error("No Callback passed to findIndex");
        }

        if (auto fn = args[0].Cast<Function>(); fn.IsValid())
        {
            const auto myRef = this->ToRef().Reserve();
            for (auto i = 0; i < _vec.size(); i++)
            {
                if (fn->Call(_vec.at(i), makeNumber(i),myRef)->ToBoolean())
                {
                    return makeNumber(i);
                }
            }
        }

        return makeNull();
    }

    TSmartPtrType<ListPrototype> List::Prototype = manage<ListPrototype>();

    TSmartPtrType<Object> List::Size(const TSmartPtrType<FunctionScope>& fnScope)
    {
        return makeNumber(_vec.size());
    }

    ListPrototype::ListPrototype() : Prototype(makeScope(), makeNativeFunction(
                                                   {}, ReservedDynamicFunctions::CALL, {},
                                                   [](const TSmartPtrType<FunctionScope>& fnScope)
                                                   {
                                                       return makeList(fnScope->GetArgs());
                                                   }))
    {
    }

    std::string ListPrototype::ToString()
    {
        return "<Prototype : List>";
    }

    TSmartPtrType<List> makeList(const std::vector<TSmartPtrType<Object>>& items)
    {
        return manage<List>(items);
    }
}
