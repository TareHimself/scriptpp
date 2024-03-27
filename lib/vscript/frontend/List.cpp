#include "vscript/frontend/List.hpp"

#include <algorithm>
#include <iostream>

#include "vscript/frontend/Null.hpp"
#include "vscript/frontend/Number.hpp"
#include "vscript/frontend/Number.hpp"
#include "vscript/frontend/String.hpp"

namespace vs::frontend
{
    ListItemReference::ListItemReference(const size_t& index, const TSmartPtrType<ScopeLike>& scope,
        const TSmartPtrType<Object>& val) : Reference(scope,val)
    {
        _index = index;
    }

    void ListItemReference::Set(const TSmartPtrType<Object>& val)
    {
        Reference::Set(val);
        if(auto asList = _scope.Cast<List>(); asList.IsValid())
        {
            asList->Set(_index,val);
        }
    }

    List::List(const std::vector<TSmartPtrType<Object>>& vec) : DynamicObject(makeScope())
    {
        _vec = vec;

        AddNativeMemberFunction("pop",this,{},&List::Pop);
        AddNativeMemberFunction("push",this,{},&List::Push);
        AddNativeMemberFunction("map",this,{"callback"},&List::Map);
        AddNativeMemberFunction("forEach",this,{"callback"},&List::ForEach);
        AddNativeMemberFunction("filter",this,{"callback"},&List::Filter);
        AddNativeMemberFunction("find",this,{"callback"},&List::FindItem);
        AddNativeMemberFunction("findIndex",this,{"callback"},&List::FindIndex);
        AddNativeMemberFunction("sort",this,{"callback"},&List::Sort);
        AddNativeMemberFunction("size",this,{},&List::Size);
        AddNativeMemberFunction("join",this,{"delimiter"},&List::Join);
    }

    std::string List::ToString()
    {
        std::string result = "[";
        for(auto i = 0; i < _vec.size(); i++)
        {
            result += _vec.at(i)->GetType() == OT_String ? "\"" + _vec.at(i)->ToString() + "\"" : _vec.at(i)->ToString();
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
            
            return manage<ListItemReference>(i,this->ToRef().Reserve().Cast<DynamicObject>(),_vec[i]);
        }
        return DynamicObject::Get(key);
    }

    void List::Set(const TSmartPtrType<Object>& key, const TSmartPtrType<Object>& val)
    {
        if(key->GetType() == OT_Number)
        {
            Set(key.Cast<Number>()->GetValueAs<int>(),val);
            return;
        }
        DynamicObject::Set(key, val);
    }

    void List::Set(const size_t& index, const TSmartPtrType<Object>& val)
    {
        if(index >= _vec.size())
        {
            throw std::runtime_error("Index out of range " + std::to_string(index));
        }

        _vec[index] = val;
    }

    TSmartPtrType<Object> List::Push(const TSmartPtrType<FunctionScope>& fnScope)
    {
        for(auto &arg : fnScope->GetArgs())
        {
            _vec.push_back(resolveReference(arg));
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

        if (auto fn = resolveReference(args[0]).Cast<Function>(); fn.IsValid())
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

        if (auto fn = resolveReference(args[0]).Cast<Function>(); fn.IsValid())
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
            throw std::runtime_error("No Callback passed to ");
        }

        if (auto fn = resolveReference(args[0]).Cast<Function>(); fn.IsValid())
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

    TSmartPtrType<Object> List::FindItem(const TSmartPtrType<FunctionScope>& fnScope)
    {
        const auto args = fnScope->GetArgs();
        if (args.empty())
        {
            throw std::runtime_error("No Callback passed to find");
        }

        if (auto fn = resolveReference(args[0]).Cast<Function>(); fn.IsValid())
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

        if (auto fn = resolveReference(args[0]).Cast<Function>(); fn.IsValid())
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

    TSmartPtrType<Object> List::Sort(const TSmartPtrType<FunctionScope>& fnScope)
    {

        const auto args = fnScope->GetArgs();

        std::function<int(const void*,const void*)> sortFn;
        if (const auto fn = args.empty() ? TSmartPtrType<Function>(nullptr) : resolveReference(args[0]).Cast<Function>(); fn.IsValid())
        {

            std::ranges::sort(_vec,[fn] (const TSmartPtrType<Object>& a,const TSmartPtrType<Object>& b)
            {
                if(auto r = fn->Call(a,b); r->GetType() == OT_Number)
                {
                    return r.Cast<Number>()->GetValueAs<int>();
                }
                
                return 0;
            });
        }
        else
        {
            std::qsort(_vec.data(),_vec.size(),sizeof(decltype(_vec)::value_type),[](const void* a,const void* b)
            {
                const auto aObj = *static_cast<const TSmartPtrType<Object> *>(a);
                const auto bObj = *static_cast<const TSmartPtrType<Object> *>(b);

                if(aObj->Less(bObj))
                {
                    return -1;
                }

                if(aObj->Greater(bObj))
                {
                    return 1;
                }

                return 0;
            });
        }
        
        return this->ToRef().Reserve();
    }

    TSmartPtrType<ListPrototype> List::Prototype = manage<ListPrototype>();

    TSmartPtrType<Object> List::Size(const TSmartPtrType<FunctionScope>& fnScope)
    {
        return makeNumber(_vec.size());
    }

    TSmartPtrType<Object> List::Join(const TSmartPtrType<FunctionScope>& fnScope)
    {
        std::string result;

        auto delimiter = resolveReference(fnScope->Find("delimiter"));
        if(delimiter->Equal(makeNull()))
        {
            delimiter = makeString("");
        }

        const std::string delimiterStr = delimiter->ToString();

        for(auto i = 0; i < _vec.size(); i++)
        {
            result += _vec.at(i)->ToString();
            if(i != _vec.size() - 1)
            {
                result += delimiterStr;
            }
        }
        
        return makeString(result);
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
