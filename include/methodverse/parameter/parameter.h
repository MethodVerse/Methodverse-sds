#pragma once

#include <iostream>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <typeinfo>
#include <iomanip>
#include <typeindex>
#include <initializer_list>
#include <type_traits>
#include <concepts>
#include <Eigen/Dense>
#include <limits>
// #define MP_UNITS_USE_FMTLIB 1
#include <mp-units/core.h>
#include <mp-units/systems/si.h>
#include <methodverse/parameter/operation_policy.h>



using namespace mp_units;
inline constexpr double eps = std::numeric_limits<double>::epsilon();
template<class> inline constexpr bool always_false_v = false;

namespace mv
{

 // ======== IParameter base interface ========
class IParameter {
public:
    virtual ~IParameter() = default;

    // Return the parameter name, e.g., "TE" or "TR".
    virtual std::string Name() const = 0;

    // Return the value as a string for UI, logging, or serialization.
    virtual std::string ValueAsString() const = 0;
};

template<typename T, mp_units::Reference auto Unit = mp_units::one>
class ParameterBase : public IParameter {
protected:
    std::vector<T> value_;
    static constexpr auto unit_ = Unit;

public:
    using value_type = T;

    // Constructors
    ParameterBase() = default;

    ParameterBase(const ParameterBase &) = default;

    ParameterBase(ParameterBase &&) noexcept = default;

    ParameterBase &operator=(const ParameterBase &) = default;

    ParameterBase &operator=(ParameterBase &&) noexcept = default;

    ParameterBase(const T& value) : value_{value} {}

    ParameterBase(const std::vector<T>& values) : value_(values) {}

    ParameterBase(std::initializer_list<T> values) : value_(values) {}

    // Conversion to T
    [[nodiscard]] T Val() const noexcept {
        return value_.empty() ? T{} : value_[0];
    }

    // Conversion to vector
    [[nodiscard]] std::vector<T> Vals() const noexcept {
        return value_;
    }

    // Operator ==
    template <class T2, auto Unit2>
    bool operator==(const ParameterBase<T2, Unit2> &other) const {
        if constexpr (Unit == Unit2) {
            return value_ == other.Get();
        }
        else {
            return false;
        }
    }

    // Access operator
    decltype(auto) operator[](size_t i) { return value_.at(i); }
    decltype(auto) operator[](size_t i) const { return value_.at(i); }

    std::string Name() const override { return "ParameterBase"; }

    // serialization to string
    [[nodiscard]] std::string ValueAsString() const override {
        std::ostringstream oss;
        oss << std::boolalpha;
        if (value_.size() == 1) {
            oss << value_[0];
        } else {
            oss << "[";
            for (size_t i = 0; i < value_.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << value_[i];
            }
            oss << "]";
        }
        return oss.str();
    }

    // Getter/setter
    [[nodiscard]] std::vector<T>& Get() noexcept { return value_; }
    [[nodiscard]] const std::vector<T>& Get() const noexcept { return value_; }
    void Set(const T& v) {
        if (value_.empty()) value_.resize(1);
        value_[0] = v;
    }
    void Set(const std::vector<T>& values) { value_ = values; }
    void Set(std::initializer_list<T> values) { value_ = values;}
    static constexpr auto  GetUnit() noexcept { return unit_;}
    std::size_t Size() const noexcept { return value_.size();}

    // ----------------
    // Binary operators, we can probably use MACRO to reduce duplication of code
    // ----------------
    // ---- Operator +
    template<class T2, mp_units::Reference auto Unit2>
    requires (op_allowed<op_policy<category_t<T>, category_t<T2>, add_op>, T, T2>)
    auto operator+(const ParameterBase<T2, Unit2>& rhs) const {
        using policy = op_policy<category_t<T>, category_t<T2>, add_op>;
        using T3 = op_return_t<policy, T, T2>;
        constexpr auto Unit3 = policy::template unit_of<Unit, Unit2>();

        auto r = policy::template impl<T, T2>(Val(), rhs.Val());
        return ParameterBase<T3, Unit3>(r);
    }

    // ---- Operator -
    template<class T2, mp_units::Reference auto Unit2>
    requires (op_allowed<op_policy<category_t<T>, category_t<T2>, sub_op>, T, T2>)
    auto operator-(const ParameterBase<T2, Unit2>& rhs) const {
        using policy = op_policy<category_t<T>, category_t<T2>, sub_op>;
        using T3 = op_return_t<policy, T, T2>;
        constexpr auto Unit3 = policy::template unit_of<Unit, Unit2>();

        auto r = policy::template impl<T, T2>(Val(), rhs.Val());
        return ParameterBase<T3, Unit3>(r);
    }    

    // ---- Operator *
    template<class T2, mp_units::Reference auto Unit2>
    requires (op_allowed<op_policy<category_t<T>, category_t<T2>, mul_op>, T, T2>)
    auto operator*(const ParameterBase<T2, Unit2>& rhs) const {
        using policy = op_policy<category_t<T>, category_t<T2>, mul_op>;
        using T3 = op_return_t<policy, T, T2>;
        constexpr auto Unit3 = policy::template unit_of<Unit, Unit2>();

        auto r = policy::template impl<T, T2>(Val(), rhs.Val());
        return ParameterBase<T3, Unit3>(r);
    }

    // ---- Operator /
    template<class T2, mp_units::Reference auto Unit2>
    requires (op_allowed<op_policy<category_t<T>, category_t<T2>, div_op>, T, T2>)
    auto operator/(const ParameterBase<T2, Unit2>& rhs) const {
        using policy = op_policy<category_t<T>, category_t<T2>, div_op>;
        using T3 = op_return_t<policy, T, T2>;
        constexpr auto Unit3 = policy::template unit_of<Unit, Unit2>();

        auto r = policy::template impl<T, T2>(Val(), rhs.Val());
        return ParameterBase<T3, Unit3>(r);
    }

    // Similarly implement -, *, / with the right op tags
};


template<class T, class Derived, mp_units::Reference auto Unit>
class Parameter : public ParameterBase<T, Unit> {
public:
    using Base = ParameterBase<T, Unit>;
    using Base::Base;

    static constexpr const char* name = Derived::name;

    // CRTP assignment operators
    Derived& operator=(const T& rhs) {
        if (this->value_.empty()) this->value_.resize(1);
        this->value_[0] = rhs;
        return static_cast<Derived&>(*this);
    }

    Derived& operator=(const std::vector<T>& rhs) {
        this->value_ = rhs;
        return static_cast<Derived&>(*this);
    }

    Derived& operator=(std::initializer_list<T> rhs) {
        this->value_ = rhs;
        return static_cast<Derived&>(*this);
    }

    // CRTP-specific functions
    [[nodiscard]] std::string Name() const override {
        return std::string(Derived::name);
    }

    // Rebind constructor: only works if type/unit match
    template<class T2, auto Unit2>
    Parameter(const ParameterBase<T2, Unit2>& other)
        requires (std::is_same_v<T2, T> && (Unit2 == Unit))
        : Base(other) {}
};



}
