#pragma once
#include "Object.hpp"
#include <cmath>
#include <stdexcept>

#include "vscript/utils.hpp"
namespace vs::frontend
{
    
#ifndef TNUMBER_SANITY_MACRO
#define TNUMBER_SANITY_MACRO(operation) \
if(const auto n = cast<Number>(other)) \
{ \
    switch (n->GetNumberType()) \
    { \
    case NT_Int: \
        { \
            if(const auto o = castStatic<TNumber<int>>(n)) \
            { \
                operation \
            } \
        } \
    case NT_Int64: \
        { \
            if(const auto o = castStatic<TNumber<uint64_t>>(n)) \
            { \
                operation \
            } \
        } \
    case NT_Float: \
        { \
            if(const auto o = castStatic<TNumber<float>>(n)) \
            { \
                operation \
            } \
        } \
    case NT_Double: \
        { \
            if(const auto o = castStatic<TNumber<double>>(n)) \
            { \
                operation \
            } \
        } \
    } \
}
#endif
    template<typename  T,typename  = std::enable_if_t<std::is_integral_v<T> | std::is_floating_point_v<T>>>
    class TNumber;
    
    template<typename  T,typename  = std::enable_if_t<std::is_integral_v<T> | std::is_floating_point_v<T>>>
    std::shared_ptr<TNumber<T>> makeNumber(const T& num);
    
    enum ENumberType
    {
        NT_Int,
        NT_Int64,
        NT_Float,
        NT_Double
    };

    
    
    template<typename T,typename  = std::enable_if_t<std::is_integral_v<T>>>
    inline T mod(T a, T b,int d = 0)
    {
        return a % b;
    }

    template<typename T,typename  = std::enable_if_t<std::is_floating_point_v<T>>>
    inline T mod(T a, T b, float e = 0)
    {
        return std::fmod(a,b);
    }
    
    class Number : public  Object
    {
        
    public:
        EObjectType GetType() const override;

        std::shared_ptr<Object> Add(const std::shared_ptr<Object>& other) override;
        std::shared_ptr<Object> Subtract(const std::shared_ptr<Object>& other) override;
        std::shared_ptr<Object> Divide(const std::shared_ptr<Object>& other) override;
        std::shared_ptr<Object> Multiply(const std::shared_ptr<Object>& other) override;

        virtual ENumberType GetNumberType() const = 0;

        template<typename T>
       T GetValueAs();
    };

    template<typename  T,typename>
    class TNumber : public Number
    {
        T _value = 0;
    public:
        TNumber(const T& val);

        bool ToBoolean() const override;
        std::string ToString() const override;
        std::shared_ptr<Object> Add(const std::shared_ptr<Object>& other) override;
        std::shared_ptr<Object> Subtract(const std::shared_ptr<Object>& other) override;
        std::shared_ptr<Object> Mod(const std::shared_ptr<Object>& other) override;
        std::shared_ptr<Object> Divide(const std::shared_ptr<Object>& other) override;
        std::shared_ptr<Object> Multiply(const std::shared_ptr<Object>& other) override;

        bool Equal(const std::shared_ptr<Object>& other) const override;
        bool Less(const std::shared_ptr<Object>& other) const override;
        bool Greater(const std::shared_ptr<Object>& other) const override;

        
        ENumberType GetNumberType() const override;

        virtual T GetValue() const;
    };

    template <typename T>
    T Number::GetValueAs()
    {
        const auto other = this->GetRef();
        TNUMBER_SANITY_MACRO(return static_cast<T>(o->GetValue());)
        throw std::runtime_error("Unknown number type");
    }

    template <typename T, typename T0>
    TNumber<T, T0>::TNumber(const T& val)
    {
        _value = val;
    }

    template <typename T, typename T0>
    bool TNumber<T, T0>::ToBoolean() const
    {
        return _value > 0;
    }

    template <typename T, typename T0>
    std::string TNumber<T, T0>::ToString() const
    {
        return std::to_string(_value);
    }

    template <typename T, typename T0>
    std::shared_ptr<Object> TNumber<T, T0>::Add(const std::shared_ptr<Object>& other)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(GetValue() + o->GetValue());)
        return Number::Add(other);
    }

    template <typename T, typename T0>
    std::shared_ptr<Object> TNumber<T, T0>::Subtract(const std::shared_ptr<Object>& other)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(GetValue() - o->GetValue());)
        return Number::Subtract(other);
    }

    template <typename T, typename T0>
    std::shared_ptr<Object> TNumber<T, T0>::Mod(const std::shared_ptr<Object>& other)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(mod(GetValue(),static_cast<T>(o->GetValue())));)
        return Number::Mod(other);
    }

    template <typename T, typename T0>
    std::shared_ptr<Object> TNumber<T, T0>::Divide(const std::shared_ptr<Object>& other)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(GetValue() / o->GetValue());)
        return Number::Divide(other);
    }

    template <typename T, typename T0>
    std::shared_ptr<Object> TNumber<T, T0>::Multiply(const std::shared_ptr<Object>& other)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(GetValue() * o->GetValue());)
        return Number::Multiply(other);
    }

    template <typename T, typename T0>
    bool TNumber<T, T0>::Equal(const std::shared_ptr<Object>& other) const
    {
        TNUMBER_SANITY_MACRO(return GetValue() == o->GetValue();)
        return Number::Equal(other);
    }

    template <typename T, typename T0>
    bool TNumber<T, T0>::Less(const std::shared_ptr<Object>& other) const
    {
        TNUMBER_SANITY_MACRO(return GetValue() < o->GetValue();)
        return Number::Less(other);
    }

    template <typename T, typename T0>
    bool TNumber<T, T0>::Greater(const std::shared_ptr<Object>& other) const
    {
        TNUMBER_SANITY_MACRO(return GetValue() > o->GetValue();)
        return Number::Greater(other);
    }

    template <typename T, typename T0>
    ENumberType TNumber<T, T0>::GetNumberType() const
    {
        if(std::is_same_v<T,int>)
        {
            return NT_Int;
        }

        if(std::is_same_v<T,uint64_t>)
        {
            return NT_Int64;
        }

        if(std::is_same_v<T,float>)
        {
            return NT_Float;
        }

        if(std::is_same_v<T,double>)
        {
            return NT_Double;
        }

        return NT_Int;
    }

    template <typename T, typename T0>
    T TNumber<T, T0>::GetValue() const
    {
        return _value;
    }



    std::shared_ptr<Number> makeNumber(const std::string& num);

    template<typename  T,typename>
    std::shared_ptr<TNumber<T>> makeNumber(const T& num)
    {
        return std::make_shared<TNumber<T>>(num);
    }
}
