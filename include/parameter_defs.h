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

// Updated macro: concrete parameter types derive from Parameter<T, NAME>
#define DEFINE_TYPED_PARAMETER(NAME, TEXT_NAME, TYPE, UNIT)                                 \
namespace mv {                                                                              \
    namespace { constexpr int ID_##NAME = __COUNTER__; }                                    \
    namespace { constexpr char NAME##_Literal[] = TEXT_NAME; }                              \
    struct NAME : public Parameter<TYPE, NAME> {                                            \
        using Parameter<TYPE, NAME>::Parameter;                                             \
        using value_type = TYPE;                                                            \
        static constexpr int id = ID_##NAME;                                                \
        static constexpr std::string_view name = NAME##_Literal;                            \
        static constexpr std::string_view unit = UNIT;                                      \
    };                                                                                      \
    template<> struct ObjectID<NAME> : IDTag<ID_##NAME> {                                   \
        static constexpr const char* name = NAME##_Literal;                                 \
    };                                                                                      \
    template<> struct TypeFromID<ID_##NAME> { using type = NAME; };                         \
    namespace {                                                                             \
        const bool registered_##NAME = []() {                                               \
            auto& registry = ParameterTypeRegistry();                                       \
            auto [it, inserted] = registry.emplace(TEXT_NAME,                               \
                std::type_index(typeid(NAME)));                                             \
            assert(inserted && "Duplicate TEXT_NAME detected during parameter registration!");\
            return true;                                                                    \
        }();                                                                                \
    }                                                                                       \
} /* namespace mv */

namespace mv {
    template <class T>
    concept SelfAddable = requires (const T& a, const T& b) {
        { a + b } -> std::convertible_to<T>;
    };

    template <class T>
    concept SelfSubtractable = requires (const T& a, const T& b) {
        { a - b } -> std::convertible_to<T>;
    };

    template <class T>
    concept SelfMultipliable = requires (const T& a, const T& b) {
        { a * b } -> std::convertible_to<T>;
    };

    template <class T>
    concept SelfDividable = requires (const T& a, const T& b) {
        { a / b } -> std::convertible_to<T>;
    };

// ======== IParameter base interface ========
class IParameter {
public:
    virtual ~IParameter() = default;

    // Return the parameter name, e.g., "TE" or "TR".
    virtual std::string Name() const = 0;

    // Return the value as a string for UI, logging, or serialization.
    virtual std::string ValueAsString() const = 0;

    // Return runtime type information for safe casting or introspection.
    virtual const std::type_info& Type() const = 0;
};

// ======== ParameterContainer for each sequence ========
class ParameterContainer {
public:
    // Add a parameter to the container.
    void Add(const std::shared_ptr<IParameter>& param) {
        parameters_.push_back(param);
        param_map_[param->Name()] = param;
    }

    // Look up a parameter by name.
    [[nodiscard]] std::shared_ptr<IParameter> Find(const std::string& paramName) const {
        auto it = param_map_.find(paramName);
        return (it != param_map_.end()) ? it->second : nullptr;
    }

    // Iterate over all parameters in the container.
    void ForEach(const std::function<void(const std::shared_ptr<IParameter>&)>& func) const {
        for (const auto& p : parameters_) {
            func(p);
        }
    }

    // Print all parameters in the container to stdout.
    void PrintAll() const {
        std::cout << "\n--- Parameter Container ---\n";
        ForEach([](const auto& p) {
            std::cout << std::setw(20) << std::left << p->Name() << ": " << p->ValueAsString() << '\n';
        });
    }

private:
    std::vector<std::shared_ptr<IParameter>> parameters_;
    std::unordered_map<std::string, std::shared_ptr<IParameter>> param_map_;
};

// Central runtime type registry.
// Maps parameter TEXT_NAME strings -> type_index of the C++ type.
//
// Use:
//   ParameterTypeRegistry()["TE"] = std::type_index(typeid(TE));
inline std::unordered_map<std::string, std::type_index>& ParameterTypeRegistry() {
    static std::unordered_map<std::string, std::type_index> registry;
    return registry;
}

// ======== Per-sequence container instance ========
inline ParameterContainer& SequenceParameterContainer() {
    static ParameterContainer container;
    return container;
}

// ======== Main Parameter class with CRTP and vector storage ========
template<typename T, typename Derived>
class Parameter : public IParameter {
public:
    // Default constructors that will be created by compiler, but we list them for clarity.
    Parameter(const Parameter &) = default;
    Parameter(Parameter &&) noexcept = default;
    Parameter &operator=(const Parameter &) = default;
    Parameter &operator=(Parameter &&) noexcept = default;

    // TE te(1.0);
    Parameter(const T& value) : value_{value} {}

    // TE te(std::vector);
    Parameter(const std::vector<T>& values) : value_(values) {}

    // TE te({1.0, 2.0, 3.0});
    Parameter(std::initializer_list<T> values) : value_(values) {}

    // Derived type accessors, e.g. TE te; te.Derived() returns TE&.
    const Derived& GetDerived() const { return *this; }
    Derived& GetDerived() { return *this; }

    // Conversion operator to T, e.g. double x = static_cast<double>(te);
    // We force explicit conversion to avoid implicit conversions that could lead to confusion.
    [[nodiscard]] explicit operator T() const noexcept {
        return value_.empty() ? T{} : value_[0];
    }

    // Conversion operator to std::vector<T>, e.g. auto vec = static_cast<std::vector<double>>(te);
    [[nodiscard]] explicit operator std::vector<T>() const noexcept {
        return value_;
    }

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

    // Access operator for single value or vector, e.g. te[0] = 1.0; auto x = te[1];
    T& operator[](size_t i) { return value_.at(i); }
    const T& operator[](size_t i) const { return value_.at(i); }

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

    // Return the type_info for this parameter, e.g. typeid(TE) or typeid(TR).
    [[nodiscard]] const std::type_info& Type() const override {
        return typeid(*this);
    }

    // Getter and setter for the value.
    [[nodiscard]] const std::vector<T>& Get() const noexcept { return value_; }
    void Set(const T& v) {
        if (value_.empty()) value_.resize(1);
        value_[0] = v;
    }
    void Set(const std::vector<T>& values) { value_ = values; }
    void Set(std::initializer_list<T> values) { value_ = values;}

    // Arithmetic operations with another parameter of the same type.
    Derived operator+(const Derived& other) const requires SelfAddable<T>{
        return elementWiseBinaryOp(*this, other, std::plus<>());
    }
 
    Derived operator-(const Derived& other) const requires SelfSubtractable<T>{
        return elementWiseBinaryOp(*this, other, std::minus<>());
    }

    Derived operator*(const Derived& other) const requires SelfMultipliable<T>{
        return elementWiseBinaryOp(*this, other, std::multiplies<>());
    }

    Derived operator/(const Derived& other) const requires SelfDividable<T>{
        return elementWiseBinaryOp(*this, other, std::divides<>());
    }

    size_t Size() const noexcept { return value_.size(); }
    void Resize(size_t n) { value_.resize(n); }

    const std::string& Unit() const noexcept { return unit_; }
    void SetUnit(const std::string& unit) { unit_ = unit; }

private:
    std::vector<T> value_;
    std::string unit_;
};

// ======== Utility: declare and register parameter ========
template<typename ParamType, typename... Args>
std::shared_ptr<ParamType> DeclareParameter(Args&&... args) {
    auto param = std::make_shared<ParamType>(std::forward<Args>(args)...);
    SequenceParameterContainer().Add(param);
    return param;
}

// ======== Type ID and registry helpers ========
template<int ID> struct TypeFromID;

template<typename T> struct ObjectID;
template<int N> struct IDTag : std::integral_constant<int, N> {};

template<typename D1, typename D2>
constexpr bool is_same_unit_v = (D1::unit == D2::unit);

// ======== Element-wise binary operation for parameters of same type or 
//          different types with same primitive type ========
template <typename T, typename D1, typename D2, typename Op>
auto elementWiseBinaryOp(const Parameter<T, D1>& lhs, const Parameter<T, D1>& rhs, Op op)
{
    const auto& value_lhs = lhs.Get();
    const auto& value_rhs = rhs.Get();
    if (value_lhs.empty() || value_rhs.empty())
    {
        throw std::runtime_error("Cannot perform binary operation on empty parameter values");
    }
    if (value_lhs.size() > 1 && value_rhs.size() > 1 && value_lhs.size() != value_rhs.size())
    {
        throw std::runtime_error("Parameter vector size mismatch in binary operation");
    }
    else if (value_lhs.size() == 1 && value_rhs.size() > 1)
    {
        // when lhs is a single value and rhs is a vector
        std::vector<T> result(value_rhs.size());
        for (size_t i = 0; i < value_rhs.size(); ++i)
        {
            result[i] = op(value_lhs[0], value_rhs[i]);
        }
        return D1(result);
    }
    else if (value_lhs.size() > 1 && value_rhs.size() == 1)
    {
        // when lhs is a vector and rhs is a single value
        std::vector<T> result(value_lhs.size());
        for (size_t i = 0; i < value_lhs.size(); ++i)
        {
            result[i] = op(value_lhs[i], value_rhs[0]);
        }
        return D2(result);
    }
    else if (value_lhs.size() == value_rhs.size())
    {
        // when both lhs and rhs are vectors of the same size
        std::vector<T> result(value_lhs.size());
        std::transform(value_lhs.begin(), value_lhs.end(), value_rhs.begin(), result.begin(), op);
        return D1(result);
    }
    else
    {
        throw std::runtime_error("Unexpected vector size in binary operation");
    }
}

// ======== Arithmetic operators for different Parameter types but with same primitive type ========
template<typename T, typename D1, typename D2,
         typename = std::enable_if_t<std::is_arithmetic_v<T> && is_same_unit_v<D1, D2>>>
auto operator+(const Parameter<T, D1>& lhs, const Parameter<T, D2>& rhs) {
    return Parameter<T, D1>::template elementWiseBinaryOp(lhs.GetDerived(), rhs.GetDerived(), std::plus<>{});
}

template<typename T, typename D1, typename D2,
         typename = std::enable_if_t<std::is_arithmetic_v<T> && is_same_unit_v<D1, D2>>>
auto operator-(const Parameter<T, D1>& lhs, const Parameter<T, D2>& rhs) {
    return Parameter<T, D1>::template elementWiseBinaryOp(lhs.GetDerived(), rhs.GetDerived(), std::minus<>{});
}

template<typename T, typename D1, typename D2,
         typename = std::enable_if_t<std::is_arithmetic_v<T> && is_same_unit_v<D1, D2>>>
auto operator*(const Parameter<T, D1>& lhs, const Parameter<T, D2>& rhs) {
    return Parameter<T, D1>::template elementWiseBinaryOp(lhs.GetDerived(), rhs.GetDerived(), std::plus<>{});
}

template<typename T, typename D1, typename D2,
         typename = std::enable_if_t<std::is_arithmetic_v<T> && is_same_unit_v<D1, D2>>>
auto operator/(const Parameter<T, D1>& lhs, const Parameter<T, D2>& rhs) {
    return Parameter<T, D1>::template elementWiseBinaryOp(lhs.GetDerived(), rhs.GetDerived(), std::minus<>{});
}

} // namespace mv