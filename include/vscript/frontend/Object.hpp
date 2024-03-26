#pragma once
#include <memory>
#include <string>
#include "vscript/Managed.hpp"

namespace vs::frontend
{
    template<typename T>
    using TSmartPtrType = Managed<T>;
    
    enum EObjectType
    {
        OT_Null,
        OT_Number,
        OT_String,
        OT_Boolean,
        OT_Callable,
        OT_Function,
        OT_Scope,
        OT_Module,
        OT_ReturnValue,
        OT_FlowControl,
        OT_Array,
        OT_Dynamic,
        OT_Reference
    };
    
    class Object : public RefThis<Object> {
    public:
        virtual ~Object() = default;

        virtual EObjectType GetType() const = 0;

        virtual std::string ToString() = 0;

        virtual bool ToBoolean() const;

        virtual bool Equal(const TSmartPtrType<Object>& other) const;
        virtual bool NotEqual(const TSmartPtrType<Object>& other) const;
        virtual bool Less(const TSmartPtrType<Object>& other) const;
        virtual bool LessEqual(const TSmartPtrType<Object>& other) const;
        virtual bool Greater(const TSmartPtrType<Object>& other) const;
        virtual bool GreaterEqual(const TSmartPtrType<Object>& other) const;

        virtual TSmartPtrType<Object> Add(const TSmartPtrType<Object>& other);
        virtual TSmartPtrType<Object> Subtract(const TSmartPtrType<Object>& other);
        virtual TSmartPtrType<Object> Mod(const TSmartPtrType<Object>& other);
        virtual TSmartPtrType<Object> Divide(const TSmartPtrType<Object>& other);
        virtual TSmartPtrType<Object> Multiply(const TSmartPtrType<Object>& other);

        virtual unsigned long long GetAddress() const;

        TSmartPtrType<Object> GetRef() const;
    };
    


    class ReturnValue : public  Object
    {
        TSmartPtrType<Object> _value;
    public:
        ReturnValue(const TSmartPtrType<Object>& val);

        EObjectType GetType() const override;

        std::string ToString() override;

        TSmartPtrType<Object> GetValue() const;
    };

    class FlowControl : public Object
    {
    public:
        enum EFlowControlOp
        {
            Break,
            Continue
        };

    private:

        EFlowControlOp _op = EFlowControlOp::Break;
    public:
        FlowControl(const EFlowControlOp& val);

        EObjectType GetType() const override;

        std::string ToString() override;

        EFlowControlOp GetValue() const;
    };


    TSmartPtrType<ReturnValue> makeReturnValue(const TSmartPtrType<Object>& val);

    TSmartPtrType<FlowControl> makeFlowControl(const FlowControl::EFlowControlOp& val);
}
