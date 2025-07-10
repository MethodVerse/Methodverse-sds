#pragma once
#include "parameter.h"

template<int ID> 
struct TypeFromID;

template<unsigned Hash> 
struct TypeFromName;

template<typename T> 
struct ObjectID;

// The constexpr_hash() function is a compile-time hash function that takes a string
// (usually a literal like "TE" or "TR") and converts it into an unsigned integer value (a hash).
// This lets you use strings as identifiers at compile time, even though C++ doesn’t directly allow
// string literals as template arguments.
//
// The function:
//   - Iterates over each character in the input string.
//   - Multiplies the current hash by 101 (a prime number, common in hash functions)
//     and adds the character’s numeric value.
//   - Returns an unsigned integer hash.
//
// This allows you to uniquely (with high probability) represent a string as a constexpr
// integral value usable in templates, e.g.:
//   constexpr unsigned te_hash = constexpr_hash("TE");  // e.g., 8469
//   constexpr unsigned tr_hash = constexpr_hash("TR");  // e.g., 8570
constexpr unsigned constexpr_hash(const char* str) {
    unsigned hash = 0;
    while (*str) hash = hash * 101 + static_cast<unsigned>(*str++);
    return hash;
}

template<int N> 
struct IDTag : std::integral_constant<int, N> {};

// REGISTER_PARAMETER(NAME, TEXT_NAME)
// -----------------------------------
// Registers a new parameter type with a unique integer ID, a string name, and a compile-time hash.
// This macro defines a struct named NAME derived from Parameter, and sets up type traits for
// mapping between the parameter type, its ID, and its name/hash.
//
// Usage example:
//   REGISTER_PARAMETER(TE, "TE");
//   REGISTER_PARAMETER(TR, "TR");
//
// After registration, you can use NAME::id, NAME::name, NAME::hash, and perform type lookups
// via ObjectID<NAME> and TypeFromID<NAME::id>.
//
// Args:
//   NAME:      The C++ identifier for the parameter type (will become a struct).
//   TEXT_NAME: The string literal name for the parameter (e.g., "TE").
//
#define REGISTER_PARAMETER(NAME, TEXT_NAME)                                  \
    namespace { constexpr int ID_##NAME = __COUNTER__; }                     \
    constexpr char NAME##_Literal[] = TEXT_NAME;                             \
    struct NAME : public Parameter {                                         \
        using Parameter::Parameter;                                          \
        static constexpr int id = ID_##NAME;                                 \
        static constexpr const char* name = NAME##_Literal;                  \
        static constexpr unsigned hash = constexpr_hash(TEXT_NAME);          \
    };                                                                       \
    template<> struct ObjectID<NAME> : IDTag<ID_##NAME> {                    \
        static constexpr const char* name = NAME##_Literal;                  \
        static constexpr unsigned hash = constexpr_hash(TEXT_NAME);          \
    };                                                                       \
    template<> struct TypeFromID<ID_##NAME> { using type = NAME; };
