#pragma once
#include "Object.hpp"
#include <cmath>
namespace vs::frontend
{
    
#ifndef TNUMBER_SANITY_MACRO
#define TNUMBER_SANITY_MACRO(operation) \
if(const auto n = other.Cast<Number>(); n.IsValid()) \
{ \
    switch (n->GetNumberType()) \
    { \
    case NT_Int: \
        { \
            if(const auto o = n.CastStatic<TNumber<int>>(); o.IsValid()) \
            { \
                operation \
            } \
        } \
    case NT_Int64: \
        { \
            if(const auto o = n.CastStatic<TNumber<uint64_t>>(); o.IsValid()) \
            { \
                operation \
            } \
        } \
    case NT_Float: \
        { \
            if(const auto o = n.CastStatic<TNumber<float>>(); o.IsValid()) \
            { \
                operation \
            } \
        } \
    case NT_Double: \
        { \
            if(const auto o = n.CastStatic<TNumber<double>>(); o.IsValid()) \
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
    TSmartPtrType<TNumber<T>> makeNumber(const T& num);
    
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

        TSmartPtrType<Object> Add(const TSmartPtrType<Object>& other) override;
        TSmartPtrType<Object> Subtract(const TSmartPtrType<Object>& other) override;
        TSmartPtrType<Object> Divide(const TSmartPtrType<Object>& other) override;
        TSmartPtrType<Object> Multiply(const TSmartPtrType<Object>& other) override;

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
        TSmartPtrType<Object> Add(const TSmartPtrType<Object>& other) override;
        TSmartPtrType<Object> Subtract(const TSmartPtrType<Object>& other) override;
        TSmartPtrType<Object> Mod(const TSmartPtrType<Object>& other) override;
        TSmartPtrType<Object> Divide(const TSmartPtrType<Object>& other) override;
        TSmartPtrType<Object> Multiply(const TSmartPtrType<Object>& other) override;

        bool Equal(const TSmartPtrType<Object>& other) const override;
        bool Less(const TSmartPtrType<Object>& other) const override;
        bool Greater(const TSmartPtrType<Object>& other) const override;

        
        ENumberType GetNumberType() const override;

        virtual T GetValue() const;
    };

    template <typename T>
    T Number::GetValueAs()
    {
        const auto other = this->ToRef().Reserve();
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
    TSmartPtrType<Object> TNumber<T, T0>::Add(const TSmartPtrType<Object>& other)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(GetValue() + o->GetValue());)
        return Number::Add(other);
    }

    template <typename T, typename T0>
    TSmartPtrType<Object> TNumber<T, T0>::Subtract(const TSmartPtrType<Object>& other)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(GetValue() - o->GetValue());)
        return Number::Subtract(other);
    }

    template <typename T, typename T0>
    TSmartPtrType<Object> TNumber<T, T0>::Mod(const TSmartPtrType<Object>& other)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(mod(GetValue(),static_cast<T>(o->GetValue())));)
        return Number::Mod(other);
    }

    template <typename T, typename T0>
    TSmartPtrType<Object> TNumber<T, T0>::Divide(const TSmartPtrType<Object>& other)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(GetValue() / o->GetValue());)
        return Number::Divide(other);
    }

    template <typename T, typename T0>
    TSmartPtrType<Object> TNumber<T, T0>::Multiply(const TSmartPtrType<Object>& other)
    {
        TNUMBER_SANITY_MACRO(return makeNumber(GetValue() * o->GetValue());)
        return Number::Multiply(other);
    }

    template <typename T, typename T0>
    bool TNumber<T, T0>::Equal(const TSmartPtrType<Object>& other) const
    {
        TNUMBER_SANITY_MACRO(return GetValue() == o->GetValue();)
        return Number::Equal(other);
    }

    template <typename T, typename T0>
    bool TNumber<T, T0>::Less(const TSmartPtrType<Object>& other) const
    {
        TNUMBER_SANITY_MACRO(return GetValue() < o->GetValue();)
        return Number::Less(other);
    }

    template <typename T, typename T0>
    bool TNumber<T, T0>::Greater(const TSmartPtrType<Object>& other) const
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



    TSmartPtrType<Number> makeNumber(const std::string& num);

    template<typename  T,typename>
    TSmartPtrType<TNumber<T>> makeNumber(const T& num)
    {
        return manage<TNumber<T>>(num);
    }
}
