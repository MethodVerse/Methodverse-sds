#include "parameter.h"
#include <stdexcept>

Parameter::Parameter() = default;

Parameter::Parameter(const Parameter& other)
    : value_(other.value_),
      observers_(other.observers_),
      handlers_(other.handlers_),
      type_index_(other.type_index_) {}

Parameter::Parameter(Parameter&& other) noexcept
    : value_(std::move(other.value_)),
      observers_(std::move(other.observers_)),
      handlers_(std::move(other.handlers_)),
      type_index_(other.type_index_) {}

Parameter& Parameter::operator=(const Parameter& other) {
    if (this != &other) {
        value_ = other.value_;
        observers_ = other.observers_;
        handlers_ = other.handlers_;
        type_index_ = other.type_index_;
    }
    return *this;
}   

Parameter& Parameter::operator=(Parameter&& other) noexcept {
    if (this != &other) {
        value_ = std::move(other.value_);
        observers_ = std::move(other.observers_);
        handlers_ = std::move(other.handlers_);
        type_index_ = other.type_index_;
    }
    return *this;
}

Parameter::Parameter(const ParameterValue& v) : value_(v) {}

Parameter::~Parameter() = default;

// Equality operator
bool Parameter::operator==(const Parameter& other) const {
    // Quick check: if the variants hold different alternatives, they can't be equal.
    if (value_.index() != other.value_.index()) return false;

    // Use std::visit to compare the active alternatives
    return std::visit([](const auto& va, const auto& vb) -> bool {
        using A = std::decay_t<decltype(va)>;
        using B = std::decay_t<decltype(vb)>;
        
        if constexpr (std::is_same_v<A, B>) {
            if constexpr (std::is_same_v<A, Eigen::Matrix3d> ||
                          std::is_same_v<A, Eigen::Vector3d> ||
                          std::is_same_v<A, Eigen::Quaterniond>) {
                // Compare Eigen::Matrix3d with isApprox()
                return va.isApprox(vb);
            } else if constexpr (std::is_same_v<A, std::vector<Eigen::Vector3d>> ||
                                 std::is_same_v<A, std::vector<Eigen::Matrix3d>> ||
                                 std::is_same_v<A, std::vector<Eigen::Quaterniond>>) {
                // Compare vectors of Eigen types with isApprox()
                if (va.size() != vb.size()) return false;
                for (size_t i = 0; i < va.size(); ++i) {
                    if (!va[i].isApprox(vb[i])) return false;
                }
                return true;
            } else {
                // Default comparison with operator==
                return va == vb;
            }
        } else {
            // Different types => not equal
            return false;
        }
    }, value_, other.value_);
}

bool Parameter::operator!=(const Parameter &other) const
{
    return !(*this == other);
}

void Parameter::addObserver(ObserverPtr observer) {
    observers_.push_back(observer);
}

void Parameter::removeObserver(ObserverPtr observer) {
    observers_.erase(std::remove(observers_.begin(), observers_.end(), observer),
                                     observers_.end());
}

void Parameter::notifyObservers() {
    for (auto observer : observers_) {
        if (observer) {
            observer->onNotified(*this);
        }
    }
}

void Parameter::onNotified(const Parameter& notifier) {
    auto it = handlers_.find(std::type_index(typeid(notifier)));
    if (it != handlers_.end()) {
        it->second(notifier);
    }
}

const ParameterValue &Parameter::getValue() const{
    return value_;
}

ParameterValue &Parameter::getValue(){
    return value_;
}

void Parameter::setValue(const ParameterValue &v){
    value_ = v;
    notifyObservers();
}

