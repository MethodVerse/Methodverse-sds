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

double eps = std::numeric_limits<double>::epsilon();

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

    template <class D1, class D2 = D1>
    concept Addable = ParameterLike<D1> && ParameterLike<D2> &&
                      std::is_base_of_v<Parameter<typename D2::value_type, D2>, D2> &&
                      !std::is_same_v<category_t<typename D1::value_type>, bool_tag> &&
                      !std::is_same_v<category_t<typename D1::value_type>, string_tag> &&
                      !std::is_same_v<category_t<typename D1::value_type>, bool_tag> &&
                      std::is_same_v<typename D1::value_type, typename D2::value_type> &&
                      D1::unit == D2::unit;

    template <class D1, class D2 = D1>
    concept Subtractable = ParameterLike<D1> && ParameterLike<D2> &&
                           !std::is_same_v<category_t<typename D1::value_type>, bool_tag> &&
                           !std::is_same_v<category_t<typename D1::value_type>, string_tag> &&
                           std::is_same_v<typename D1::value_type, typename D2::value_type> &&
                           D1::unit == D2::unit;

    template <class D1, class D2 = D1>
    concept Multipliable = ParameterLike<D1> && ParameterLike<D2> &&
                           !std::is_same_v<category_t<typename D1::value_type>, bool_tag> &&
                           !std::is_same_v<category_t<typename D1::value_type>, string_tag> &&
                           std::is_same_v<typename D1::value_type, typename D2::value_type> &&
                           D1::unit == D2::unit;

    template <class D1, class D2 = D1>
    concept Divisible = ParameterLike<D1> && ParameterLike<D2> &&
                        !std::is_same_v<category_t<typename D1::value_type>, bool_tag> &&
                        !std::is_same_v<category_t<typename D1::value_type>, string_tag> &&
                        std::is_same_v<typename D1::value_type, typename D2::value_type> &&
                        D1::unit == D2::unit;                           

    template <class D1, class D2 = D1>
    concept AndableBool  = ParameterLike<D1> && ParameterLike<D2> &&
                           std::is_same_v<category_t<typename D1::value_type>, bool_tag> &&
                           std::is_same_v<category_t<typename D2::value_type>, bool_tag>;

    template <class D1, class D2 = D1>
    concept OrableBool   = AndableBool<D1, D2>;
    
    template <class D1, class D2 = D1>
    concept XorableBool  = AndableBool<D1, D2>;

    template <class D>
    concept NotableBool  = ParameterLike<D> && std::is_same_v<category_t<typename D::value_type>, bool_tag>;

    template <class D1, class D2 = D1>
    concept EleWiseMultipliable = ParameterLike<D1> && ParameterLike<D2> &&
                                  std::is_same_v<typename D1::value_type, typename D2::value_type> &&
                                  (std::is_same_v<category_t<typename D1::value_type>, eigen_vec_tag> ||
                                   std::is_same_v<category_t<typename D1::value_type>, eigen_mat_tag> ||
                                   std::is_same_v<category_t<typename D1::value_type>, eigen_quat_tag>);

    template <class D1, class D2 = D1>
    concept EleWiseDividable = EleWiseMultipliable<D1, D2>;

    template <class D1, class D2 = D1>
    concept MatrixMultipliable = ParameterLike<D1> && ParameterLike<D2> &&
                                  std::is_same_v<typename D1::value_type, typename D2::value_type> &&
                                  std::is_same_v<category_t<typename D1::value_type>, eigen_mat_tag>;
    
    template <class D>
    concept Transposible  = ParameterLike<D> && 
                            (std::is_same_v<category_t<typename D::value_type>, eigen_vec_tag> ||
                             std::is_same_v<category_t<typename D::value_type>, eigen_mat_tag>);                            
    
    template <class D1, class D2 = D1>
    concept CrossProductible = ParameterLike<D1> && ParameterLike<D2> &&
                               std::is_same_v<category_t<typename D1::value_type>, eigen_vec_tag> &&
                               std::is_same_v<category_t<typename D2::value_type>, eigen_vec_tag>;

    template <class D1, class D2 = D1>
    concept DotProductible = ParameterLike<D1> && ParameterLike<D2> &&
                             std::is_same_v<category_t<typename D1::value_type>, eigen_vec_tag> &&
                             std::is_same_v<category_t<typename D2::value_type>, eigen_vec_tag>;

    template <class T>
    auto coef_wise_multiple(const T& a, const T& b) {
        if constexpr (std::is_same_v<category_t<T>, eigen_vec_tag>) {
            return a.cwiseProduct(b);
        } else if constexpr (std::is_same_v<category_t<T>, eigen_mat_tag>) {
            return a.cwiseProduct(b);
        } else {
            static_assert(false, "Unsupported type for element-wise multiplication");
        }
    }

    template <class T>
    auto coef_wise_divide(const T& a, const T& b) {
        if constexpr (std::is_same_v<category_t<T>, eigen_vec_tag>) {
            return (a.array() / (b.array() + eps)).matrix().eval();
        } else if constexpr (std::is_same_v<category_t<T>, eigen_mat_tag>) {
            return (a.array() / (b.array() + eps)).matrix().eval();
        } else {
            static_assert(false, "Unsupported type for element-wise division");
        }
    }

    template <class T>
    auto matrix_divide(const T& a, const T& b) {
        if constexpr (std::is_same_v<category_t<T>, eigen_mat_tag>) {
            return a * b.inverse();
        } else {
            static_assert("Unsupported type for matrix multiplication");
        }
    }   

    template <class T>
    auto dot_product(const T& a, const T& b) {
        if constexpr (std::is_same_v<category_t<T>, eigen_vec_tag>) {
            return a.dot(b);
        } else {
            static_assert("Unsupported type for dot product");
        }
    }

    template <class T>
    auto cross_product(const T& a, const T& b) {
        if constexpr (std::is_same_v<category_t<T>, eigen_vec_tag>) {
            return a.cross(b);
        } else {
            static_assert("Unsupported type for cross product");
        }
    }

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
    Parameter() = default;
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
    [[nodiscard]] T ToScalar() const noexcept {
        return value_.empty() ? T{} : value_[0];
    }

    // Conversion operator to std::vector<T>, e.g. auto vec = static_cast<std::vector<double>>(te);
    [[nodiscard]] std::vector<T> ToVector() const noexcept {
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

    // Operator ==
    bool operator==(const Parameter& other) const {
        return value_ == other.value_;
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

    // Return the type_info for this parameter, e.g. typeid(TE) or typeid(TR).
    [[nodiscard]] const std::type_info& Type() const override {
        return typeid(*this);
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

    // Arithmetic operations with another parameter of the same primitive type.
    template <class D2 = Derived> 
    Derived operator+(const D2& other) const requires Addable<Derived, D2>{
        return elementWiseBinaryOp<T>(*this, other, std::plus<>());
    }
 
    template <class D2 = Derived>
    Derived operator-(const D2& other) const requires Subtractable<Derived, D2>{
        return elementWiseBinaryOp<T>(*this, other, std::minus<>());
    }

    template <class D2 = Derived>
    Derived operator*(const D2& other) const requires Multipliable<Derived, D2>{
        return elementWiseBinaryOp<T>(*this, other, std::multiplies<>());
    }

    template <class D2 = Derived>
    Derived operator/(const D2& other) const requires Divisible<Derived, D2>{
        if constexpr (std::is_same_v<category_t<typename Derived::value_type>, eigen_mat_tag>) {
            return elementWiseBinaryOp<T>(*this, other, matrix_divide<typename Derived::value_type>);
        } else {
            return elementWiseBinaryOp<T>(*this, other, std::divides<>());
        }        
    }

    template <class D2 = Derived>
    Derived operator&&(const D2& other) const requires AndableBool<Derived, D2> {
        return elementWiseBinaryOp<T>(*this, other, std::logical_and<>());
    }

    template <class D2 = Derived>
    Derived operator||(const D2& other) const requires OrableBool<Derived, D2> {
        return elementWiseBinaryOp<T>(*this, other, std::logical_or<>());
    }

    template <class D2 = Derived>
    Derived operator^(const D2& other) const requires XorableBool<Derived, D2> {
        return elementWiseBinaryOp<T>(*this, other, std::bit_xor<>());
    }

    Derived operator!() const requires NotableBool<Derived> {
        Derived result;
        for (const auto& v : value_) {
            result.Get().push_back(!v);
        }
        return result;
    }

    // Element-wise multiplication and division for Eigen types.
    template <class D2 = Derived>
    Derived CoefWiseMultiply(const D2& other) const requires EleWiseMultipliable<Derived, D2> {
        return Derived(elementWiseBinaryOp<T>(*this, other, coef_wise_multiple<T>));
    }

    template <class D2 = Derived>
    Derived CoefWiseDivide(const D2& other) const requires EleWiseDividable<Derived, D2> {
        return Derived(elementWiseBinaryOp<T>(*this, other, coef_wise_divide<T>));
    }

    // Dot product for Eigen vector types.
    template <class D2 = Derived>
    auto Dot(const D2& other) const requires DotProductible<Derived, D2> {
        // Can only return a std::vector, not a Parameter
        return elementWiseBinaryOp<double>(*this, other, dot_product<T>);
    }

    // Cross product for Eigen vector of scalars, not a Parameter.
    template <class D2 = Derived>
    auto Cross(const D2& other) const requires CrossProductible<Derived, D2> {
        // Can only return a std::vectorof matrices, not a Parameter 
        return elementWiseBinaryOp<T>(*this, other, cross_product<T>);
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
template <typename R, typename D1, typename D2, typename Op>
auto elementWiseBinaryOp(const Parameter<typename D1::value_type, D1>& lhs, const Parameter<typename D2::value_type, D2>& rhs, Op op)
requires (std::is_same_v<typename D1::value_type, typename D2::value_type>) && 
         (ParameterLike<D1> && ParameterLike<D2> ) && (D1::unit == D2::unit) &&
         std::is_convertible_v<std::invoke_result_t<Op, const typename D1::value_type&, const typename D2::value_type&>, R>
{
    using T = typename D1::value_type;
    // The result type of calling op(const T1&, const T2&)
    using RR = std::remove_cvref_t<R>;

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
        std::vector<RR> result(value_rhs.size());
        for (size_t i = 0; i < value_rhs.size(); ++i)
        {
            result[i] = op(value_lhs[0], value_rhs[i]);
        }
        return result;
    }
    else if (value_lhs.size() > 1 && value_rhs.size() == 1)
    {
        // when lhs is a vector and rhs is a single value
        std::vector<RR> result(value_lhs.size());
        for (size_t i = 0; i < value_lhs.size(); ++i)
        {
            result[i] = op(value_lhs[i], value_rhs[0]);
        }
        return result;
    }
    else if (value_lhs.size() == value_rhs.size())
    {
        // when both lhs and rhs are vectors of the same size
        std::vector<RR> result(value_lhs.size());
        std::transform(value_lhs.begin(), value_lhs.end(), value_rhs.begin(), result.begin(), op);
        return result;
    }
    else
    {
        throw std::runtime_error("Unexpected vector size in binary operation");
    }
}

} // namespace mv