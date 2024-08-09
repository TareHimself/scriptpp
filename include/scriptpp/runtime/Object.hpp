#pragma once
#include <memory>
#include <string>

#include "scriptpp/utils.hpp"


namespace spp::runtime
{
    class ScopeLike;

    enum class EObjectType
    {
        Null,
        Number,
        String,
        Boolean,
        Callable,
        Function,
        Scope,
        Module,
        ReturnValue,
        FlowControl,
        Array,
        Dynamic,
        Reference
    };
    
    class Object : public std::enable_shared_from_this<Object> {
    public:
        virtual ~Object() = default;

        virtual EObjectType GetType() const = 0;

        virtual std::string ToString(const std::shared_ptr<ScopeLike>&  = {}) const = 0;

        virtual bool ToBoolean(const std::shared_ptr<ScopeLike>& scope = {}) const;

        virtual void Init();

        virtual bool Equal(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope = {}) const;
        virtual bool NotEqual(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope = {}) const;
        virtual bool Less(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope = {}) const;
        virtual bool LessEqual(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope = {}) const;
        virtual bool Greater(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope = {}) const;
        virtual bool GreaterEqual(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope = {}) const;

        virtual std::shared_ptr<Object> Add(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope = {});
        virtual std::shared_ptr<Object> Subtract(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope = {});
        virtual std::shared_ptr<Object> Mod(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope = {});
        virtual std::shared_ptr<Object> Divide(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope = {});
        virtual std::shared_ptr<Object> Multiply(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope = {});

        virtual bool IsCallable() const;

        virtual unsigned long long GetAddress() const;

        std::shared_ptr<Object> GetRef() const;
    };
    


    class ReturnValue : public  Object
    {
        std::shared_ptr<Object> _value;
    public:
        ReturnValue(const std::shared_ptr<Object>& val);

        EObjectType GetType() const override;

        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;

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

        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;

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
