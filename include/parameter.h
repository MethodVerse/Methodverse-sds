#pragma once

#include <variant>
#include <functional>
#include <string>
#include <vector>
#include <typeindex>
#include <Eigen/Dense>

// EnumHolder is used to store an enum value in an integer and its type in std::type_index.
// This allows for type-safe operations to prevent assigning an enum value of one enum type to another enum type.
struct EnumHolder {
    int value;
    std::type_index type;       // for type-safe comparisons
    bool operator==(const EnumHolder& other) const {
        return (value == other.value) && (type == other.type);
    }
};

// Combine primitive types into a single variant type
// Ther are pros and cons doing this.
using ParameterValue = std::variant<
    int,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>,
    Eigen::Vector3d,
    Eigen::Matrix3d,
    Eigen::Quaterniond,
    std::vector<Eigen::Vector3d>,
    std::vector<Eigen::Matrix3d>,
    std::vector<Eigen::Quaterniond>,
    EnumHolder
    >;

// Compile time helper to check if T is in ParameterValue
template <typename T, typename Variant>
struct is_in_variant;

template <typename T, typename... Types>
struct is_in_variant<T, std::variant<Types...>>
    : std::disjunction<std::is_same<T, Types>...>
{
};

template <typename T>
constexpr bool is_parameter_value_type_v = is_in_variant<T, ParameterValue>::value;

// Compile time helper to get the index of a type in a variant
template <typename T, typename Variant, std::size_t Index = 0>
constexpr std::size_t variant_index_of()
{
    using First = std::variant_alternative_t<Index, Variant>;
    if constexpr (std::is_same_v<T, First>)
    {
        return Index;
    }
    else if constexpr (Index + 1 < std::variant_size_v<Variant>)
    {
        return variant_index_of<T, Variant, Index + 1>();
    }
    else
    {
        static_assert(!sizeof(T), "Type not found in variant");
    }
}

// The base Parameter class
class Parameter
{
public:
    using ObserverPtr = Parameter *;
    using NotificationHandler = std::function<void(const Parameter &)>;
    using ValueType = ParameterValue;

    // Default constructor
    Parameter();

    // Copy constructor
    Parameter(const Parameter &other);

    // Move constructor
    Parameter(Parameter &&other) noexcept;

    // Copy assignment operator
    Parameter &operator=(const Parameter &other);

    // Move assignment operator
    Parameter &operator=(Parameter &&other) noexcept;

    // Constructor from a ParameterValue
    explicit Parameter(const ParameterValue &v);

    // Constructor from a concrete type and value
    template <typename T>
    Parameter(T a);

    // Assignment Operator from value of a primitive type
    template <typename T, typename = std::enable_if_t<is_parameter_value_type_v<T>>>
    Parameter &operator=(T a);

    // Equality operator
    bool operator==(const Parameter& other) const;

    // Inequality operator
    bool operator!=(const Parameter& other) const;
    
    // Destructor
    virtual ~Parameter();

    // Overloaded conversion operators. 
    template <typename T, typename = std::enable_if_t<is_parameter_value_type_v<T>>>
    operator T() const; // implicit conversion, e.g., T x = param;

    // Overloaded function call operator for explicit extraction
    template <typename T, typename = std::enable_if_t<is_parameter_value_type_v<T>>>
    T operator()() const; // explicit extraction, e.g. int x = param<int>();

    // Enum values are stored as integers in the variant.
    template <typename EnumType>
    void setEnum(EnumType val);

    template <typename EnumType>
    EnumType getEnum() const;

    // Getters and setters for the value
    const ParameterValue& getValue() const;
    ParameterValue& getValue(); 
    void setValue(const ParameterValue& v);

    // Notifier - Observer pattern embdedded in Parameter. May need to be moved to a separate class and logic.
    void addObserver(ObserverPtr observer);
    void removeObserver(ObserverPtr observer);
    void notifyObservers();
    void onNotified(const Parameter &notifier);
    template <typename NotifierType>
    void setNotificationHandler(NotificationHandler handler);

protected:
    ParameterValue value_;
    std::vector<ObserverPtr> observers_;
    std::unordered_map<std::type_index, NotificationHandler> handlers_;
    std::size_t type_index_ = 0; // index of the type in the variant
};

template <typename NotifierType>
void Parameter::setNotificationHandler(NotificationHandler handler)
{
    auto x = std::type_index(typeid(NotifierType));
    handlers_[std::type_index(typeid(NotifierType))] = std::move(handler);
}

template <typename T>
Parameter::Parameter(T a)
{
    static_assert(is_parameter_value_type_v<T>, "Invalid type assigned to Parameter");

    value_ = a;

    // get index of T in variant's list of types
    constexpr std::size_t idx = variant_index_of<T, ParameterValue>();
    type_index_ = idx;
}

template <typename T, typename>
Parameter::operator T() const
{
    static_assert(is_parameter_value_type_v<T>, "Invalid type conversion from Parameter");

    return std::get<T>(value_);
}

template <typename T, typename>
T Parameter::operator()() const {

    static_assert(is_parameter_value_type_v<T>, "Invalid type extraction from Parameter");

    return std::get<T>(value_);
}

template <typename EnumType>
void Parameter::setEnum(EnumType val){
    static_assert(std::is_enum_v<EnumType>, "EnumType must be an enum type");
    // Store the enum's integer value and its type info into EnumHolder
    value_ = EnumHolder{static_cast<int>(val), std::type_index(typeid(EnumType))};
}

template <typename EnumType>
EnumType Parameter::getEnum() const
{
    static_assert(std::is_enum_v<EnumType>, "EnumType must be an enum type");
    
    // Make sure the stored value is an EnumHolder
    const auto& holder = std::get<EnumHolder>(value_);

    // Check if the stored type matches the requested EnumType
    if (holder.type != std::type_index(typeid(EnumType))) {
        throw std::runtime_error("Enum type mismatch in getEnum()");
    }

    // Safe to cast and return
    return static_cast<EnumType>(holder.value);
}

template <typename T, typename>
Parameter &Parameter::operator=(T a)
{
    static_assert(is_parameter_value_type_v<T>, "Invalid type assigned to Parameter");
    value_ = a;

    // get index of T in variant's list of types
    constexpr std::size_t idx = variant_index_of<T, ParameterValue>();
    type_index_ = idx;

    notifyObservers();
    return *this;
}