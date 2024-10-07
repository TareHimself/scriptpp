#include "scriptpp/runtime/List.hpp"

#include <algorithm>
#include <iostream>

#include "scriptpp/utils.hpp"
#include "scriptpp/runtime/Exception.hpp"
#include "scriptpp/runtime/Null.hpp"
#include "scriptpp/runtime/Number.hpp"
#include "scriptpp/runtime/Number.hpp"
#include "scriptpp/runtime/String.hpp"

namespace spp::runtime
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

        AddNativeMemberFunction("pop",this,vectorOf<std::string>(),&List::Pop);
        AddNativeMemberFunction("push",this,vectorOf<std::string>(),&List::Push);
        AddNativeMemberFunction("map",this,vectorOf<std::string>("callback"),&List::Map);
        AddNativeMemberFunction("forEach",this,vectorOf<std::string>("callback"),&List::ForEach);
        AddNativeMemberFunction("filter",this,vectorOf<std::string>("callback"),&List::Filter);
        AddNativeMemberFunction("find",this,vectorOf<std::string>("callback"),&List::FindItem);
        AddNativeMemberFunction("size",this,vectorOf<std::string>(),&List::Size);
        AddNativeMemberFunction("join",this,vectorOf<std::string>("delimiter"),&List::Join);
        AddNativeMemberFunction("findIndex",this,vectorOf<std::string>("callback"),&List::FindIndex);
        AddNativeMemberFunction("sort",this,vectorOf<std::string>("callback"),&List::Sort);
        AddNativeMemberFunction("reverse",this,vectorOf<std::string>(),&List::Reverse);
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
            result += _vec.at(i)->GetType() == EObjectType::String ? "\"" + _vec.at(i)->ToString(scope) + "\"" : _vec.at(i)->ToString(scope);
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

    std::shared_ptr<Object> List::Get(const std::shared_ptr<Object>& key, const std::shared_ptr<ScopeLike>& scope) const
    {
        if(key->GetType() == EObjectType::Number)
        {
            const auto i = cast<Number>(key)->GetValueAs<int>();
            if(i >= _vec.size())
            {
                throw makeException(scope,"Index out of range " + std::to_string(i));
            }
            
            return makeListReference(this,i);
        }
        return DynamicObject::Get(key, scope);
    }

    void List::Set(const std::shared_ptr<Object>& key, const std::shared_ptr<Object>& val, const std::shared_ptr<ScopeLike>& scope)
    {
        if(key->GetType() == EObjectType::Number)
        {
            Set(cast<Number>(key)->GetValueAs<int>(),val);
            return;
        }
        DynamicObject::Set(key, val,scope);
    }

    void List::Set(const std::string& key, const std::shared_ptr<Object>& val)
    {
        DynamicObject::Set(key, val);
    }

    void List::Set(const size_t& index, const std::shared_ptr<Object>& val)
    {
        if(index >= _vec.size())
        {
            throw makeException({},"Index out of range " + std::to_string(index));
        }

        _vec[index] = val;
    }

    std::shared_ptr<Object> List::Push(const std::shared_ptr<FunctionScope>& fnScope)
    {
        for(auto &[_,arg] : fnScope->GetNamedArgs())
        {
            _vec.push_back(resolveReference(arg));
        }
        
        return this->GetRef();
    }

    std::shared_ptr<Object> List::Pop(const std::shared_ptr<FunctionScope>& fnScope)
    {
        if(_vec.empty())
        {
            throw makeException(fnScope,"Attempted to pop from empty list");
        }

        auto last = _vec.back();

        _vec.pop_back();

        return last;
    }

    std::shared_ptr<Object> List::Map(const std::shared_ptr<FunctionScope>& fnScope)
    {
        const auto arg = resolveReference(fnScope->GetArg(0));
        if (arg == makeNull())
        {
            throw makeException(fnScope,"No Callback passed to map");
        }

        if (const auto fn = cast<Function>(arg))
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
        const auto arg = resolveReference(fnScope->GetArg(0));
        if (arg == makeNull())
        {
            throw makeException(fnScope,"No Callback passed to forEach");
        }

        if (const auto fn = cast<Function>(arg))
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
        const auto arg = resolveReference(fnScope->GetArg(0));
        if (arg == makeNull())
        {
            throw makeException(fnScope,"No Callback passed to filter");
        }

        if (const auto fn = cast<Function>(arg))
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
        const auto arg = resolveReference(fnScope->GetArg(0));
        if (arg == makeNull())
        {
            throw makeException(fnScope,"No Callback passed to find");
        }

        if (const auto fn = cast<Function>(arg))
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
        const auto arg = resolveReference(fnScope->GetArg(0));
        if (arg == makeNull())
        {
            throw makeException(fnScope,"No Callback passed to findIndex");
        }

        if (const auto fn = cast<Function>(arg))
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

        const auto args = fnScope->GetNamedArgs();

        std::function<int(const void*,const void*)> sortFn;
        if (const auto fn = args.empty() ? std::shared_ptr<Function>(nullptr) : cast<Function>(resolveReference(fnScope->GetArg(0))))
        {
            const auto self = cast<DynamicObject>(this->GetRef());
            std::ranges::sort(_vec,[fn,self] (const std::shared_ptr<Object>& a,const std::shared_ptr<Object>& b)
            {
                if(const auto r = fn->Call(self,a,b); r->GetType() == EObjectType::Number)
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

    size_t List::GetHashCode(const std::shared_ptr<ScopeLike>& scope)
    {
        auto result = DynamicObject::GetHashCode(scope);
        for (auto &object : _vec)
        {
            result = hashCombine(result,object->GetHashCode());
        }
        return result;
    }

    ListPrototype::ListPrototype() : Prototype(makeScope(), makeNativeFunction(
                                                   {}, ReservedDynamicFunctions::CALL, vectorOf<std::string>(),
                                                   [](const std::shared_ptr<FunctionScope>& fnScope)
                                                   {
                                                       return makeList(fnScope->GetPositionalArgs());
                                                   }))
    {
    }

    std::string ListPrototype::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        return "<Prototype : List>";
    }

    std::shared_ptr<List> makeList()
    {
        return makeList(std::vector<std::shared_ptr<Object>>{});
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
