#pragma once
#include "DynamicObject.hpp"
#include "Object.hpp"

namespace vs::frontend
{
    class String : public DynamicObject
    {
        std::string _str;
    public:
        String(const std::string&str);
        EObjectType GetType() const override;
        std::string ToString() override;
        bool ToBoolean() const override;
        void OnRefSet() override;
        TSmartPtrType<Object> Add(const TSmartPtrType<Object>& other) override;
        TSmartPtrType<Object> Multiply(const TSmartPtrType<Object>& other) override;
        TSmartPtrType<Object> Split(const TSmartPtrType<FunctionScope>& fnScope);
        TSmartPtrType<Object> Size(const TSmartPtrType<FunctionScope>& fnScope);
        TSmartPtrType<Object> Trim(const TSmartPtrType<FunctionScope>& fnScope);
        TSmartPtrType<Object> Get(const TSmartPtrType<Object>& key) override;
        void Set(const TSmartPtrType<Object>& key, const TSmartPtrType<Object>& val) override;
        
    };

    TSmartPtrType<String> makeString(const std::string& str);
}
