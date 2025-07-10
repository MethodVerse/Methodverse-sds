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

    [[nodiscard]] std::string Name() const override { return Derived::name; }

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
    template<typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    Derived operator+(const Derived& other) const {
        return elementWiseBinaryOp(other, std::plus<>());
    }
    template<typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    Derived operator-(const Derived& other) const {
        return elementWiseBinaryOp(other, std::minus<>());
    }
    template<typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    Derived operator*(const Derived& other) const {
        return elementWiseBinaryOp(other, std::multiplies<>());
    }
    template<typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    Derived operator/(const Derived& other) const {
        return elementWiseBinaryOp(other, std::divides<>());
    }

    template<typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    Derived operator+(const T& rhs) const { return Derived(value_[0] + rhs); }
    template<typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    Derived operator-(const T& rhs) const { return Derived(value_[0] - rhs); }
    template<typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    Derived operator*(const T& rhs) const { return Derived(value_[0] * rhs); }
    template<typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    Derived operator/(const T& rhs) const { return Derived(value_[0] / rhs); }

    template<typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend Derived operator+(const T& lhs, const Derived& rhs) { return Derived(lhs + rhs[0]); }
    template<typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend Derived operator-(const T& lhs, const Derived& rhs) { return Derived(lhs - rhs[0]); }
    template<typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend Derived operator*(const T& lhs, const Derived& rhs) { return Derived(lhs * rhs[0]); }
    template<typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend Derived operator/(const T& lhs, const Derived& rhs) { return Derived(lhs / rhs[0]); }

    size_t Size() const noexcept { return value_.size(); }
    void Resize(size_t n) { value_.resize(n); }

    const std::string& Unit() const noexcept { return unit_; }
    void SetUnit(const std::string& unit) { unit_ = unit; }

private:
    std::vector<T> value_;
    std::string unit_;
    template <typename Op>
    Derived elementWiseBinaryOp(const Derived& other, Op op) const {
        if (value_.size() > 1 && other.Size() > 1 && value_.size() != other.Size()) {
            throw std::runtime_error("Parameter vector size mismatch in binary operation");
        } else if (value_.size() == 1 && other.Size() > 1) {
            std::vector<T> result(other.Size());
            for (size_t i = 0; i < other.Size(); ++i) {
                result[i] = op(value_[0], other[i]);
            }
            return Derived(result);
        } else if (value_.size() > 1 && other.Size() == 1) {
            std::vector<T> result(value_.size());
            for (size_t i = 0; i < value_.size(); ++i) {
                result[i] = op(value_[i], other[0]);
            }
            return Derived(result);
        } else if (value_.size() == other.Size()) {
            std::vector<T> result(value_.size());
            std::transform(value_.begin(), value_.end(), other.Get().begin(), result.begin(), op);
            return Derived(result);
        } else {
            throw std::runtime_error("Unexpected vector size in binary operation");
        }
    }
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

// Updated macro: concrete parameter types derive from Parameter<T, NAME>
#define DEFINE_TYPED_PARAMETER(NAME, TEXT_NAME, TYPE)                                 \
    namespace { constexpr int ID_##NAME = __COUNTER__; }                             \
    namespace { constexpr char NAME##_Literal[] = TEXT_NAME; }                       \
    struct NAME : public Parameter<TYPE, NAME> {                                     \
        using Parameter<TYPE, NAME>::Parameter;                                      \
        using value_type = TYPE;                                                     \
        static constexpr int id = ID_##NAME;                                         \
        static constexpr const char* name = NAME##_Literal;                          \
    };                                                                               \
    template<> struct ObjectID<NAME> : IDTag<ID_##NAME> {                            \
        static constexpr const char* name = NAME##_Literal;                         \
    };                                                                               \
    template<> struct TypeFromID<ID_##NAME> { using type = NAME; };                  \
    namespace {                                                                      \
        const bool registered_##NAME = []() {                                        \
            auto& registry = ParameterTypeRegistry();                                \
            auto [it, inserted] = registry.emplace(TEXT_NAME, std::type_index(typeid(NAME))); \
            assert(inserted && "Duplicate TEXT_NAME detected during parameter registration!"); \
            return true;                                                             \
        }();                                                                         \
    }

