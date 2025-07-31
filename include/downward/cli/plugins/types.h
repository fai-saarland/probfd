#ifndef PLUGINS_TYPES_H
#define PLUGINS_TYPES_H

#include "downward/cli/plugins/registry_types.h"

#include "downward/utils/strings.h"

#include <algorithm>
#include <any>
#include <cassert>
#include <limits>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace downward::utils {
class Context;
}

namespace downward::cli::plugins {
class CategoryPlugin;

class Type {
public:
    virtual ~Type() = default;

    virtual bool operator==(const Type& other) const = 0;

    virtual bool is_basic_type() const;
    virtual bool is_feature_type() const;
    virtual bool is_list_type() const;
    virtual bool is_enum_type() const;
    virtual bool is_symbol_type() const;

    virtual bool can_convert_into(const Type& other) const;

    virtual std::string name() const = 0;
    virtual size_t get_hash() const = 0;
};

class BasicType : public Type {
    std::type_index type;
    std::string class_name;

public:
    explicit BasicType(std::type_index type, const std::string& class_name);

    bool operator==(const Type& other) const override;
    bool is_basic_type() const override;
    bool can_convert_into(const Type& other) const override;
    std::string name() const override;
    size_t get_hash() const override;

    const std::type_index& get_basic_type_index() const;
};

class FeatureType : public Type {
    std::type_index pointer_type;
    std::string type_name;
    std::string synopsis;

public:
    FeatureType(
        std::type_index pointer_type,
        const std::string& type_name,
        const std::string& synopsis);

    bool operator==(const Type& other) const override;
    bool is_feature_type() const override;
    std::string name() const override;
    size_t get_hash() const override;

    std::string get_synopsis() const;
};

class ListType : public Type {
    const Type& nested_type;

public:
    ListType(const Type& nested_type);

    bool operator==(const Type& other) const override;
    bool is_list_type() const override;
    bool can_convert_into(const Type& other) const override;
    std::string name() const override;
    size_t get_hash() const override;

    const Type& get_nested_type() const;
};

class EmptyListType : public Type {
public:
    bool operator==(const Type& other) const override;
    bool is_list_type() const override;
    bool can_convert_into(const Type& other) const override;
    std::string name() const override;
    size_t get_hash() const override;
};

class EnumType : public Type {
    std::type_index type;
    std::vector<std::string> values;
    EnumInfo documented_values;

public:
    EnumType(std::type_index type, const EnumInfo& documented_values);

    bool operator==(const Type& other) const override;
    bool is_enum_type() const override;

    std::string name() const override;
    size_t get_hash() const override;

    const EnumInfo& get_documented_enum_values() const;

    int
    get_enum_index(const std::string& value, downward::utils::Context& context)
        const;
};

class SymbolType : public Type {
public:
    bool operator==(const Type& other) const override;
    bool is_symbol_type() const override;
    bool can_convert_into(const Type& other) const override;
    std::string name() const override;
    size_t get_hash() const override;
};

class TypeRegistry {
    template <typename T>
    struct TypeOf {
        static const Type& value(TypeRegistry& registry);
    };

    template <typename T>
    struct TypeOf<std::vector<T>> {
        static const Type& value(TypeRegistry& registry);
    };

    struct SemanticHash {
        size_t operator()(const Type* t) const
        {
            if (!t) { return 0; }
            return t->get_hash();
        }
    };

    struct SemanticEqual {
        size_t operator()(const Type* t1, const Type* t2) const
        {
            if (!t1 || !t2) { return t1 == t2; }
            return *t1 == *t2;
        }
    };

    std::unordered_map<std::type_index, std::unique_ptr<Type>> registered_types;
    std::unordered_map<
        const Type*,
        std::unique_ptr<ListType>,
        SemanticHash,
        SemanticEqual>
        registered_list_types;
    template <typename T>
    void insert_basic_type();
    const Type& get_nonlist_type(std::type_index type) const;

public:
    static BasicType NO_TYPE;
    static SymbolType SYMBOL_TYPE;
    static EmptyListType EMPTY_LIST_TYPE;

    TypeRegistry();

    const FeatureType& create_feature_type(const CategoryPlugin& plugin);
    const EnumType& create_enum_type(const EnumPlugin& plugin);
    const ListType& create_list_type(const Type& element_type);

    template <typename T>
    const Type& get_type();

    static TypeRegistry* instance()
    {
        static TypeRegistry instance_;
        return &instance_;
    }
};

template <typename T>
const Type& TypeRegistry::TypeOf<T>::value(TypeRegistry& registry)
{
    return registry.get_nonlist_type(typeid(T));
}

template <typename T>
const Type& TypeRegistry::TypeOf<std::vector<T>>::value(TypeRegistry& registry)
{
    return registry.create_list_type(registry.get_type<T>());
}

template <typename T>
const Type& TypeRegistry::get_type()
{
    return TypeOf<T>::value(*this);
}

extern std::any convert(
    const std::any& value,
    const Type& from_type,
    const Type& to_type,
    downward::utils::Context& context);
} // namespace downward::cli::plugins

#endif
