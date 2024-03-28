#include "vscript/frontend/Object.hpp"

#include <iostream>

#include "vscript/frontend/Null.hpp"

namespace vs::frontend
{

    bool Object::ToBoolean() const
    {
        return true;
    }

    bool Object::Equal(const TSmartPtrType<Object>& other) const
    {
        return GetType() == other->GetType();
    }

    bool Object::NotEqual(const TSmartPtrType<Object>& other) const
    {
        return !Equal(other);
    }

    bool Object::Less(const TSmartPtrType<Object>& other) const
    {
        return this < other.Get();
    }

    bool Object::LessEqual(const TSmartPtrType<Object>& other) const
    {
        return Less(other) || Equal(other);
    }

    bool Object::Greater(const TSmartPtrType<Object>& other) const
    {
        return this > other.Get();
    }

    bool Object::GreaterEqual(const TSmartPtrType<Object>& other) const
    {
        return Greater(other) || Equal(other);
    }

    TSmartPtrType<Object> Object::Add(const TSmartPtrType<Object>& other)
    {
        return makeNull();
    }

    TSmartPtrType<Object> Object::Subtract(const TSmartPtrType<Object>& other)
    {
        return makeNull();
    }

    TSmartPtrType<Object> Object::Mod(const TSmartPtrType<Object>& other)
    {
        return makeNull();
    }

    TSmartPtrType<Object> Object::Divide(const TSmartPtrType<Object>& other)
    {
        return makeNull();
    }

    TSmartPtrType<Object> Object::Multiply(const TSmartPtrType<Object>& other)
    {
        return makeNull();
    }

    unsigned long long Object::GetAddress() const
    {
        return reinterpret_cast<unsigned long long>((void**)this);
    }

    TSmartPtrType<Object> Object::GetRef() const
    {
        return ToRef().Reserve();
    }
    

    ReturnValue::ReturnValue(const TSmartPtrType<Object>& val)
    {
        _value = val;
    }

    EObjectType ReturnValue::GetType() const
    {
        return OT_ReturnValue;
    }

    std::string ReturnValue::ToString() const
    {
        return "Return Value >> " + _value->ToString();
    }

    TSmartPtrType<Object> ReturnValue::GetValue() const
    {
        return _value;
    }

    FlowControl::FlowControl(const EFlowControlOp& val)
    {
        _op = val;
    }

    EObjectType FlowControl::GetType() const
    {
        return OT_FlowControl;
    }

    std::string FlowControl::ToString() const
    {
        switch (_op)
        {
        case Break:
            return "break";
        case Continue:
            return "continue";
        }

        return "";
    }

    FlowControl::EFlowControlOp FlowControl::GetValue() const
    {
        return _op;
    }

    TSmartPtrType<ReturnValue> makeReturnValue(const TSmartPtrType<Object>& val)
    {
        return manage<ReturnValue>(val);
    }

    TSmartPtrType<FlowControl> makeFlowControl(const FlowControl::EFlowControlOp& val)
    {
        return manage<FlowControl>(val);
    }
}
