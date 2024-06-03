#include "vscript/runtime/List.hpp"

#include <algorithm>
#include <iostream>

#include "vscript/utils.hpp"
#include "vscript/runtime/Null.hpp"
#include "vscript/runtime/Number.hpp"
#include "vscript/runtime/Number.hpp"
#include "vscript/runtime/String.hpp"

namespace vs::runtime
{
    ListItemReference::ListItemReference(const size_t& index, const std::shared_ptr<ScopeLike>& scope,
        const std::shared_ptr<Object>& val) : Reference(scope,val)
    {
        _index = index;
    }

    void ListItemReference::Set(const std::shared_ptr<Object>& val)
    {
        Reference::Set(val);
        if(const auto asList = cast<List>(_scope))
        {
            asList->Set(_index,val);
        }
    }

    void List::Init()
    {
        DynamicObject::Init();

        AddNativeMemberFunction("pop",this,{},&List::Pop);
        AddNativeMemberFunction("push",this,{},&List::Push);
        AddNativeMemberFunction("map",this,{"callback"},&List::Map);
        AddNativeMemberFunction("forEach",this,{"callback"},&List::ForEach);
        AddNativeMemberFunction("filter",this,{"callback"},&List::Filter);
        AddNativeMemberFunction("find",this,{"callback"},&List::FindItem);
        AddNativeMemberFunction("size",this,{},&List::Size);
        AddNativeMemberFunction("join",this,{"delimiter"},&List::Join);
        AddNativeMemberFunction("findIndex",this,{"callback"},&List::FindIndex);
        AddNativeMemberFunction("sort",this,{"callback"},&List::Sort);
        AddNativeMemberFunction("reverse",this,{},&List::Reverse);
    }

    List::List(const std::vector<std::shared_ptr<Object>>& vec) : DynamicObject(makeScope())
    {
        _vec = vec;
    }

    std::string List::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        std::string result = "[";
        for(auto i = 0; i < _vec.size(); i++)
        {
            result += _vec.at(i)->GetType() == OT_String ? "\"" + _vec.at(i)->ToString(scope) + "\"" : _vec.at(i)->ToString(scope);
            if(i != _vec.size() - 1)
            {
                result += " , ";
            }
        }

        result += "]";
        return result;
    }

    bool List::ToBoolean(const std::shared_ptr<ScopeLike>& scope) const
    {
        return !_vec.empty();
    }

    std::shared_ptr<Object> List::Get(const std::shared_ptr<Object>& key) const
    {
        if(key->GetType() == OT_Number)
        {
            const auto i = cast<Number>(key)->GetValueAs<int>();
            if(i >= _vec.size())
            {
                throw std::runtime_error("Index out of range " + std::to_string(i));
            }
            
            return makeListReference(this,i);
        }
        return DynamicObject::Get(key);
    }

    void List::Set(const std::shared_ptr<Object>& key, const std::shared_ptr<Object>& val)
    {
        if(key->GetType() == OT_Number)
        {
            Set(cast<Number>(key)->GetValueAs<int>(),val);
            return;
        }
        DynamicObject::Set(key, val);
    }

    void List::Set(const std::string& key, const std::shared_ptr<Object>& val)
    {
        DynamicObject::Set(key, val);
    }

    void List::Set(const size_t& index, const std::shared_ptr<Object>& val)
    {
        if(index >= _vec.size())
        {
            throw std::runtime_error("Index out of range " + std::to_string(index));
        }

        _vec[index] = val;
    }

    std::shared_ptr<Object> List::Push(const std::shared_ptr<FunctionScope>& fnScope)
    {
        for(auto &arg : fnScope->GetArgs())
        {
            _vec.push_back(resolveReference(arg));
        }
        
        return this->GetRef();
    }

    std::shared_ptr<Object> List::Pop(const std::shared_ptr<FunctionScope>& fnScope)
    {
        if(_vec.empty())
        {
            throw std::runtime_error("Attempted to pop from empty list");
        }

        auto last = _vec.back();

        _vec.pop_back();

        return last;
    }

    std::shared_ptr<Object> List::Map(const std::shared_ptr<FunctionScope>& fnScope)
    {
        const auto args = fnScope->GetArgs();
        if (args.empty())
        {
            throw std::runtime_error("No Callback passed to findIndex");
        }

        if (const auto fn = cast<Function>(resolveReference(args[0])))
        {
            const auto self = cast<DynamicObject>(this->GetRef());
            std::vector<std::shared_ptr<Object>> mapped;
            for (auto i = 0; i < _vec.size(); i++)
            {
                mapped.push_back(fn->Call(self,_vec.at(i), makeNumber(i),self));
            }

            return makeList(mapped);
        }

        return makeNull();
    }

    std::shared_ptr<Object> List::ForEach(const std::shared_ptr<FunctionScope>& fnScope)
    {
        const auto args = fnScope->GetArgs();
        if (args.empty())
        {
            throw std::runtime_error("No Callback passed to findIndex");
        }

        if (const auto fn = cast<Function>(resolveReference(args[0])))
        {
            const auto self = cast<DynamicObject>(this->GetRef());
            for (auto i = 0; i < _vec.size(); i++)
            {
                fn->Call(self,_vec.at(i), makeNumber(i),self);
            }
        }

        return makeNull();
    }

    std::shared_ptr<Object> List::Filter(const std::shared_ptr<FunctionScope>& fnScope)
    {
        const auto args = fnScope->GetArgs();
        if (args.empty())
        {
            throw std::runtime_error("No Callback passed to ");
        }

        if (auto fn = cast<Function>(resolveReference(args[0])))
        {
            const auto self = cast<DynamicObject>(this->GetRef());
            std::vector<std::shared_ptr<Object>> filtered;
            for (auto i = 0; i < _vec.size(); i++)
            {
                if (fn->Call(self,_vec.at(i), makeNumber(i),self)->ToBoolean(fnScope))
                {
                    filtered.push_back(_vec.at(i));
                }
            }

            return makeList(filtered);
        }

        return makeNull();
    }

    std::shared_ptr<Object> List::FindItem(const std::shared_ptr<FunctionScope>& fnScope)
    {
        const auto args = fnScope->GetArgs();
        if (args.empty())
        {
            throw std::runtime_error("No Callback passed to find");
        }

        if (auto fn = cast<Function>(resolveReference(args[0])))
        {
            const auto self = cast<DynamicObject>(this->GetRef());
            for (auto i = 0; i < _vec.size(); i++)
            {
                if (fn->Call(self,_vec.at(i), makeNumber(i),self)->ToBoolean(fnScope))
                {
                    return _vec.at(i);
                }
            }
        }

        return makeNull();
    }

    std::shared_ptr<Object> List::FindIndex(const std::shared_ptr<FunctionScope>& fnScope)
    {
        const auto args = fnScope->GetArgs();
        if (args.empty())
        {
            throw std::runtime_error("No Callback passed to findIndex");
        }

        if (auto fn = cast<Function>(resolveReference(args[0])))
        {
            const auto self = cast<DynamicObject>(this->GetRef());
            for (auto i = 0; i < _vec.size(); i++)
            {
                if (fn->Call(self,_vec.at(i), makeNumber(i),self)->ToBoolean(fnScope))
                {
                    return makeNumber(i);
                }
            }
        }

        return makeNull();
    }

    std::shared_ptr<Object> List::Sort(const std::shared_ptr<FunctionScope>& fnScope)
    {

        const auto args = fnScope->GetArgs();

        std::function<int(const void*,const void*)> sortFn;
        if (const auto fn = args.empty() ? std::shared_ptr<Function>(nullptr) : cast<Function>(resolveReference(args[0])))
        {
            const auto self = cast<DynamicObject>(this->GetRef());
            std::ranges::sort(_vec,[fn,self] (const std::shared_ptr<Object>& a,const std::shared_ptr<Object>& b)
            {
                if(const auto r = fn->Call(self,a,b); r->GetType() == OT_Number)
                {
                    const int i = cast<Number,Object>(r)->GetValueAs<int>();
                    return i;
                }
                
                return 0;
            });
        }
        else
        {
            const auto self = cast<DynamicObject>(this->GetRef());
            
            std::ranges::sort(_vec,[fnScope] (const std::shared_ptr<Object>& a,const std::shared_ptr<Object>& b)
            {

                if(a->Less(b, fnScope))
                {
                    return -1;
                }

                if(a->Greater(b, fnScope))
                {
                    return 1;
                }

                return 0;
            });
        }
        
        return this->GetRef();
    }

    std::shared_ptr<ListPrototype> List::Prototype = makeObject<ListPrototype>();

    std::shared_ptr<Object> List::Size(const std::shared_ptr<FunctionScope>& fnScope)
    {
        return makeNumber(_vec.size());
    }

    std::shared_ptr<Object> List::Join(const std::shared_ptr<FunctionScope>& fnScope)
    {
        std::string result;

        auto delimiter = resolveReference(fnScope->Find("delimiter"));
        if(delimiter->Equal(makeNull(), fnScope))
        {
            delimiter = makeString("");
        }

        const std::string delimiterStr = delimiter->ToString(fnScope);

        for(auto i = 0; i < _vec.size(); i++)
        {
            result += _vec.at(i)->ToString(fnScope);
            if(i != _vec.size() - 1)
            {
                result += delimiterStr;
            }
        }
        
        return makeString(result);
    }

    std::shared_ptr<Object> List::Reverse(const std::shared_ptr<FunctionScope>& fnScope)
    {
        std::vector<std::shared_ptr<Object>> vec = _vec;
        std::ranges::reverse(vec);
        return makeList(vec);
    }

    std::vector<std::shared_ptr<Object>>& List::GetNative()
    {
        return _vec;
    }

    ListPrototype::ListPrototype() : Prototype(makeScope(), makeNativeFunction(
                                                   {}, ReservedDynamicFunctions::CALL, {},
                                                   [](const std::shared_ptr<FunctionScope>& fnScope)
                                                   {
                                                       return makeList(fnScope->GetArgs());
                                                   }))
    {
    }

    std::string ListPrototype::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        return "<Prototype : List>";
    }

    std::shared_ptr<List> makeList(const std::vector<std::shared_ptr<Object>>& items)
    {
        return  makeObject<List>(items);
    }

    std::shared_ptr<ListItemReference> makeListReference(const List* list, uint32_t idx)
    {
        return makeObject<ListItemReference>(idx,cast<DynamicObject>(list->GetRef()), const_cast<List*>(list)->GetNative()[idx]);
    }
}
