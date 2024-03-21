#pragma once
#include "DynamicObject.hpp"
#include "Function.hpp"
#include "Object.hpp"
#include "Prototype.hpp"

namespace vs::runtime
{
    class ListPrototype;
    
    class List : public DynamicObject
    {
        std::vector<TSmartPtrType<Object>> _vec;
    public:
        List(const std::vector<TSmartPtrType<Object>>& vec);
        std::string ToString() override;
        bool ToBoolean() const override;

        TSmartPtrType<Object> Get(const TSmartPtrType<Object>& key) override;
        void Set(const TSmartPtrType<Object>& key, const TSmartPtrType<Object>& val) override;

        TSmartPtrType<Object> Push(const TSmartPtrType<FunctionScope>& fnScope);
        TSmartPtrType<Object> Pop(const TSmartPtrType<FunctionScope>& fnScope);
        TSmartPtrType<Object> Map(const TSmartPtrType<FunctionScope>& fnScope);
        TSmartPtrType<Object> ForEach(const TSmartPtrType<FunctionScope>& fnScope);
        TSmartPtrType<Object> Filter(const TSmartPtrType<FunctionScope>& fnScope);
        TSmartPtrType<Object> Find(const TSmartPtrType<FunctionScope>& fnScope);
        TSmartPtrType<Object> FindIndex(const TSmartPtrType<FunctionScope>& fnScope);
        TSmartPtrType<Object> Size(const TSmartPtrType<FunctionScope>& fnScope);
        

        static TSmartPtrType<ListPrototype> Prototype;
    };

    class ListPrototype : public Prototype
    {
    public:
        ListPrototype();

        std::string ToString() override;
    };


    TSmartPtrType<List> makeList(const std::vector<TSmartPtrType<Object>>& items);
}
