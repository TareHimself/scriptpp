#pragma once
#include "DynamicObject.hpp"
#include "Function.hpp"
#include "Object.hpp"
#include "Prototype.hpp"

namespace spp::runtime
{
    class ListPrototype;

    class ListItemReference : public Reference
    {
    protected:
        size_t _index = 0;

    public:
        ListItemReference(const size_t& index,const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<Object>& val);

        void Set(const std::shared_ptr<Object>& val) override;
    };
    class List : public DynamicObject
    {
        std::vector<std::shared_ptr<Object>> _vec;
    public:
        void Init() override;
        List(const std::vector<std::shared_ptr<Object>>& vec);
        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;
        bool ToBoolean(const std::shared_ptr<ScopeLike>& scope) const override;

        std::shared_ptr<Object> Get(const std::shared_ptr<Object>& key, const std::shared_ptr<ScopeLike>& scope) const override;
        void Set(const std::shared_ptr<Object>& key, const std::shared_ptr<Object>& val, const std::shared_ptr<ScopeLike>& scope) override;
        void Set(const std::string& key, const std::shared_ptr<Object>& val) override;
        virtual void Set(const size_t& index,const std::shared_ptr<Object>& val);

        std::shared_ptr<Object> Push(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> Pop(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> Map(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> ForEach(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> Filter(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> FindItem(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> FindIndex(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> Sort(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> Size(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> Join(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> Reverse(const std::shared_ptr<FunctionScope>& fnScope);

        virtual std::vector<std::shared_ptr<Object>>& GetNative();
        static std::shared_ptr<ListPrototype> Prototype;

        size_t GetHashCode(const std::shared_ptr<ScopeLike>& scope) override;
    };

    class ListPrototype : public Prototype
    {
    public:
        ListPrototype();

        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;

        std::shared_ptr<DynamicObject> CreateInstance(std::shared_ptr<FunctionScope>& scope) override;
        std::string GetName() const override;
    };


    std::shared_ptr<List> makeList();
    
    std::shared_ptr<List> makeList(const std::vector<std::shared_ptr<Object>>& items);

    std::shared_ptr<ListItemReference> makeListReference(const List * list,uint32_t idx);
}
