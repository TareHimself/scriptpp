#include "scriptpp/runtime/Object.hpp"

#include <iostream>

#include "scriptpp/runtime/Null.hpp"

namespace spp::runtime
{
    bool Object::ToBoolean(const std::shared_ptr<ScopeLike>& scope) const
    {
        return true;
    }

    void Object::Init()
    {
    }

    bool Object::Equal(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const
    {
        return GetType() == other->GetType();
    }

    bool Object::NotEqual(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const
    {
        return !Equal(other, scope);
    }

    bool Object::Less(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const
    {
        return this < other.get();
    }

    bool Object::LessEqual(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const
    {
        return Less(other, scope) || Equal(other, scope);
    }

    bool Object::Greater(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const
    {
        return this > other.get();
    }

    bool Object::GreaterEqual(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const
    {
        return Greater(other, scope) || Equal(other, scope);
    }

    std::shared_ptr<Object> Object::Add(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        return makeNull();
    }

    std::shared_ptr<Object> Object::Subtract(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        return makeNull();
    }

    std::shared_ptr<Object> Object::Mod(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        return makeNull();
    }

    std::shared_ptr<Object> Object::Divide(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        return makeNull();
    }

    std::shared_ptr<Object> Object::Multiply(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        return makeNull();
    }

    size_t Object::GetHashCode(const std::shared_ptr<ScopeLike>& scope)
    {
        return static_cast<size_t>(GetType());
    }

    bool Object::IsCallable() const
    {
        return false;
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
        return EObjectType::ReturnValue;
    }

    std::string ReturnValue::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        return "Return Value >> " + _value->ToString(scope);
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
        return EObjectType::FlowControl;
    }

    std::string FlowControl::ToString(const std::shared_ptr<ScopeLike>& scope) const
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
