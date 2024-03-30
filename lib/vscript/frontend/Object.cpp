#include "vscript/frontend/Object.hpp"

#include <iostream>

#include "vscript/frontend/Null.hpp"

namespace vs::frontend
{

    bool Object::ToBoolean() const
    {
        return true;
    }

    void Object::Init()
    {
    }

    bool Object::Equal(const std::shared_ptr<Object>& other) const
    {
        return GetType() == other->GetType();
    }

    bool Object::NotEqual(const std::shared_ptr<Object>& other) const
    {
        return !Equal(other);
    }

    bool Object::Less(const std::shared_ptr<Object>& other) const
    {
        return this < other.get();
    }

    bool Object::LessEqual(const std::shared_ptr<Object>& other) const
    {
        return Less(other) || Equal(other);
    }

    bool Object::Greater(const std::shared_ptr<Object>& other) const
    {
        return this > other.get();
    }

    bool Object::GreaterEqual(const std::shared_ptr<Object>& other) const
    {
        return Greater(other) || Equal(other);
    }

    std::shared_ptr<Object> Object::Add(const std::shared_ptr<Object>& other)
    {
        return makeNull();
    }

    std::shared_ptr<Object> Object::Subtract(const std::shared_ptr<Object>& other)
    {
        return makeNull();
    }

    std::shared_ptr<Object> Object::Mod(const std::shared_ptr<Object>& other)
    {
        return makeNull();
    }

    std::shared_ptr<Object> Object::Divide(const std::shared_ptr<Object>& other)
    {
        return makeNull();
    }

    std::shared_ptr<Object> Object::Multiply(const std::shared_ptr<Object>& other)
    {
        return makeNull();
    }

    unsigned long long Object::GetAddress() const
    {
        return reinterpret_cast<unsigned long long>((void**)this);
    }

    std::shared_ptr<Object> Object::GetRef() const
    {
        return std::const_pointer_cast<Object>(shared_from_this());
    }
    

    ReturnValue::ReturnValue(const std::shared_ptr<Object>& val)
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

    std::shared_ptr<Object> ReturnValue::GetValue() const
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

    std::shared_ptr<ReturnValue> makeReturnValue(const std::shared_ptr<Object>& val)
    {
        return makeObject<ReturnValue>(val);
    }

    std::shared_ptr<FlowControl> makeFlowControl(const FlowControl::EFlowControlOp& val)
    {
        return makeObject<FlowControl>(val);
    }
}
