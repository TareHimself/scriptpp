#pragma once
#include <memory>
#include <string>

#include "vscript/utils.hpp"


namespace vs::frontend
{
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
    
    class Object : public std::enable_shared_from_this<Object> {
    public:
        virtual ~Object() = default;

        virtual EObjectType GetType() const = 0;

        virtual std::string ToString() const = 0;

        virtual bool ToBoolean() const;

        virtual void Init();

        virtual bool Equal(const std::shared_ptr<Object>& other) const;
        virtual bool NotEqual(const std::shared_ptr<Object>& other) const;
        virtual bool Less(const std::shared_ptr<Object>& other) const;
        virtual bool LessEqual(const std::shared_ptr<Object>& other) const;
        virtual bool Greater(const std::shared_ptr<Object>& other) const;
        virtual bool GreaterEqual(const std::shared_ptr<Object>& other) const;

        virtual std::shared_ptr<Object> Add(const std::shared_ptr<Object>& other);
        virtual std::shared_ptr<Object> Subtract(const std::shared_ptr<Object>& other);
        virtual std::shared_ptr<Object> Mod(const std::shared_ptr<Object>& other);
        virtual std::shared_ptr<Object> Divide(const std::shared_ptr<Object>& other);
        virtual std::shared_ptr<Object> Multiply(const std::shared_ptr<Object>& other);

        virtual unsigned long long GetAddress() const;

        std::shared_ptr<Object> GetRef() const;
    };
    


    class ReturnValue : public  Object
    {
        std::shared_ptr<Object> _value;
    public:
        ReturnValue(const std::shared_ptr<Object>& val);

        EObjectType GetType() const override;

        std::string ToString() const override;

        std::shared_ptr<Object> GetValue() const;
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

        std::string ToString() const override;

        EFlowControlOp GetValue() const;
    };

    template<typename T,typename ...TArgs>
    std::enable_if_t<std::is_constructible_v<T, TArgs...>,std::shared_ptr<T>> makeObject(TArgs&&... args)
    {
        static_assert(std::is_base_of_v<Object,T>,"T must inherit from Object");
        auto d = std::make_shared<T>(std::forward<TArgs>(args)...);
        castStatic<Object>(d)->Init();
        return d;
    }

    std::shared_ptr<ReturnValue> makeReturnValue(const std::shared_ptr<Object>& val);

    std::shared_ptr<FlowControl> makeFlowControl(const FlowControl::EFlowControlOp& val);

    
}
