#ifndef LANGUAGE_PLUGINS_TYPES_H
#define LANGUAGE_PLUGINS_TYPES_H

#include "language/plugins/registry_types.h"

#include <any>
#include <string>
#include <typeindex>
#include <vector>

namespace language {
class Context;
}

namespace language::plugins {

class Type {
public:
    virtual ~Type() = default;

    virtual bool operator==(const Type& other) const = 0;

    virtual bool is_basic_type() const;

    virtual bool is_feature_type() const;

    virtual bool is_list_type() const;

    virtual bool is_empty_list_type() const;

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
    explicit ListType(const Type& nested_type);

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

    bool is_empty_list_type() const override;

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

    int get_enum_index(const std::string& value, const Context& context) const;
};

class SymbolType : public Type {
public:
    bool operator==(const Type& other) const override;

    bool is_symbol_type() const override;

    bool can_convert_into(const Type& other) const override;

    std::string name() const override;

    size_t get_hash() const override;
};

extern std::any convert(
    const std::any& value,
    const Type& from_type,
    const Type& to_type,
    Context& context);

} // namespace language::plugins

#endif
