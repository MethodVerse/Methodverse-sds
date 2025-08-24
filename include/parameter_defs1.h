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
#define MP_UNITS_USE_FMTLIB 1
#include <mp-units/core.h>
#include <mp-units/systems/si.h>

using namespace mp_units;
inline constexpr double eps = std::numeric_limits<double>::epsilon();
template<class> inline constexpr bool always_false_v = false;

namespace mv
{
    // category tags for different types of parameter primitives, such as int, double, Eigen vector, Eigen matrix
    // Different primitives have different operations, or different behaviors inside the operations. We use these 
    // tags to distinguish and therefore guide the implementation of the operators, namely, + - * /, dot product, 
    // matrix multiplication, bolean operations, etc.
    struct scalar_tag {};
    struct string_tag {};
    struct bool_tag {};
    struct eigen_quat_tag {};
    struct eigen_vec_tag {};
    struct eigen_mat_tag {};

    template <class T, class = void>
    struct category {
        using type = scalar_tag;
    };

    template <class T>
    struct category<T, std::enable_if_t<std::is_convertible_v<std::remove_cvref_t<T>, std::string>>> {
         using type = string_tag;
    };

    template <>
    struct category<bool> {
        using type = bool_tag;
    };

    template <class T>       
    struct category<T, std::void_t< // Check if T is an Eigen type  
        decltype(std::remove_cvref_t<T>::RowsAtCompileTime),
        decltype(std::remove_cvref_t<T>::ColsAtCompileTime)>>{
        using U = std::remove_cvref_t<T>;
        using type = std::conditional_t<
            (U::RowsAtCompileTime == 1 || U::ColsAtCompileTime == 1), eigen_vec_tag, eigen_mat_tag>;
    };

    template<class T>
    struct category<T, std::enable_if_t<std::is_same_v<std::remove_cvref_t<T>, Eigen::Quaterniond>>>{
        using type = eigen_quat_tag;
    };

    template <class T>
    using category_t = typename category<std::remove_cvref_t<T>>::type;

    template <class T, class Derived>
    struct Parameter; // forward declaration

    template <class D>
    concept ParameterLike = requires {typename D::value_type; D::unit; D::name; } &&
                            std::is_base_of_v<Parameter<typename D::value_type, D>, D>;


 // ======== IParameter base interface ========
class IParameter {
public:
    virtual ~IParameter() = default;

    // Return the parameter name, e.g., "TE" or "TR".
    virtual std::string Name() const = 0;

    // Return the value as a string for UI, logging, or serialization.
    virtual std::string ValueAsString() const = 0;
};

template<typename T, typename Derived, mp_units::Reference auto Unit = mp_units::one>
class ParameterBase : public IParameter {
protected:
    std::vector<T> value_;
    static constexpr auto unit_ = Unit;

    // CRTP helpers
    Derived &derived() { return static_cast<Derived &>(*this); }
    const Derived &derived() const { return static_cast<const Derived &>(*this); }

public:
    using value_type = T;

    // Constructors
    ParameterBase() = default;

    ParameterBase(const ParameterBase &) = default;

    ParameterBase(ParameterBase &&) noexcept = default;

    ParameterBase &operator=(const ParameterBase &) = default;

    ParameterBase &operator=(ParameterBase &&) noexcept = default;

    // TE te(1.0);
    ParameterBase(const T& value) : value_{value} {}

    // TE te(std::vector);
    ParameterBase(const std::vector<T>& values) : value_(values) {}

    // TE te({1.0, 2.0, 3.0});
    ParameterBase(std::initializer_list<T> values) : value_(values) {}


    // assignment operators. e.g. TE te = 1.0;
    Derived& operator=(const T& rhs) {
        if (value_.empty()) value_.resize(1);
        value_[0] = rhs;
        return static_cast<Derived&>(*this);
    }

    // E.g. TE te = std::vector{1.0, 2.0, 3.0};
    Derived& operator=(const std::vector<T>& rhs) {
        value_ = rhs;
        return static_cast<Derived&>(*this);
    }

    // E.g. TE te = {1.0, 2.0, 3.0};
    Derived& operator=(std::initializer_list<T> rhs) {
        value_ = rhs;
        return static_cast<Derived&>(*this);
    }

    // Conversion to T, e.g. double x = te.Val();
    [[nodiscard]] T Val() const noexcept {
        return value_.empty() ? T{} : value_[0];
    }

    // Conversion operator to std::vector<T>, e.g. auto vec = static_cast<std::vector<double>>(te);
    [[nodiscard]] std::vector<T> Vals() const noexcept {
        return value_;
    }

    // Operator ==
    bool operator==(const ParameterBase& other) const {
        return value_ == other.value_ && Unit == other.GetUnit();
    }

    // Access operator for single value, e.g. te[0] = 1.0; auto x = te[1];
    decltype(auto) operator[](size_t i) { return value_.at(i); }
    decltype(auto) operator[](size_t i) const { return value_.at(i); }

    [[nodiscard]] std::string Name() const override { return std::string(Derived::name); }

    // serialization to string, e.g. "TE: [1.0, 2.0, 3.0]"
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

    // Getter and setter for the value.
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
};
}
