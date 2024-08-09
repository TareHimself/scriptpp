#pragma once
#include "Object.hpp"
#include <cmath>
#include <stdexcept>

#include "scriptpp/utils.hpp"
namespace spp::runtime
{
    
#ifndef TNUMBER_SANITY_MACRO
#define TNUMBER_SANITY_MACRO(operation) \
if(const auto n = cast<Number>(other)) \
{ \
    switch (n->GetNumberType()) \
    { \
    case ENumberType::Int: \
        { \
            if(const auto o = castStatic<TNumber<int>>(n)) \
            { \
                operation \
            } \
        } \
    case ENumberType::Int64: \
        { \
            if(const auto o = castStatic<TNumber<uint64_t>>(n)) \
            { \
                operation \
            } \
        } \
    case ENumberType::Float: \
        { \
            if(const auto o = castStatic<TNumber<float>>(n)) \
            { \
                operation \
            } \
        } \
    case ENumberType::Double: \
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
    
    enum class ENumberType
    {
        Int,
        Int64,
        Float,
        Double
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

        std::shared_ptr<Object> Add(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;
        std::shared_ptr<Object> Subtract(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;
        std::shared_ptr<Object> Divide(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;
        std::shared_ptr<Object> Multiply(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;

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

        bool ToBoolean(const std::shared_ptr<ScopeLike>& scope) const override;
        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;
        std::shared_ptr<Object> Add(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;
        std::shared_ptr<Object> Subtract(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;
        std::shared_ptr<Object> Mod(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;
        std::shared_ptr<Object> Divide(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;
        std::shared_ptr<Object> Multiply(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;

        bool Equal(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const override;
        bool Less(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const override;
        bool Greater(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const override;

        
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
    bool TNumber<T, T0>::ToBoolean(const std::shared_ptr<ScopeLike>& scope) const
    {
        return _value > 0;
    }

    template <typename T, typename T0>
    std::string TNumber<T, T0>::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        return std::to_string(_value);
    }

    template <typename T, typename T0>
    std::shared_ptr<Object> TNumber<T, T0>::Add(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(GetValue() + o->GetValue());)
        return Number::Add(other, scope);
    }

    template <typename T, typename T0>
    std::shared_ptr<Object> TNumber<T, T0>::Subtract(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(GetValue() - o->GetValue());)
        return Number::Subtract(other, scope);
    }

    template <typename T, typename T0>
    std::shared_ptr<Object> TNumber<T, T0>::Mod(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(mod(GetValue(),static_cast<T>(o->GetValue())));)
        return Number::Mod(other, scope);
    }

    template <typename T, typename T0>
    std::shared_ptr<Object> TNumber<T, T0>::Divide(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(GetValue() / o->GetValue());)
        return Number::Divide(other, scope);
    }

    template <typename T, typename T0>
    std::shared_ptr<Object> TNumber<T, T0>::Multiply(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(GetValue() * o->GetValue());)
        return Number::Multiply(other, scope);
    }

    template <typename T, typename T0>
    bool TNumber<T, T0>::Equal(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const
    {
        TNUMBER_SANITY_MACRO(return GetValue() == o->GetValue();)
        return Number::Equal(other, scope);
    }

    template <typename T, typename T0>
    bool TNumber<T, T0>::Less(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const
    {
        TNUMBER_SANITY_MACRO(return GetValue() < o->GetValue();)
        return Number::Less(other, scope);
    }

    template <typename T, typename T0>
    bool TNumber<T, T0>::Greater(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const
    {
        TNUMBER_SANITY_MACRO(return GetValue() > o->GetValue();)
        return Number::Greater(other, scope);
    }

    template <typename T, typename T0>
    ENumberType TNumber<T, T0>::GetNumberType() const
    {
        if(std::is_same_v<T,int>)
        {
            return ENumberType::Int;
        }

        if(std::is_same_v<T,uint64_t>)
        {
            return ENumberType::Int64;
        }

        if(std::is_same_v<T,float>)
        {
            return ENumberType::Float;
        }

        if(std::is_same_v<T,double>)
        {
            return ENumberType::Double;
        }

        return ENumberType::Int;
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
