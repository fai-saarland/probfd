#ifndef LANGUAGE_PLUGINS_TYPE_REGISTRY_H
#define LANGUAGE_PLUGINS_TYPE_REGISTRY_H

#include "language/plugins/types.h"

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace language::plugins {
class CategoryPlugin;
}

namespace language::plugins {

class TypeRegistry {
    template <typename>
    struct TypeOf {
        static const Type& value(const TypeRegistry& registry);
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

private:
    template <typename T>
    void insert_basic_type();

    const Type& get_nonlist_type(std::type_index type) const;
};

template <typename T>
const Type& TypeRegistry::TypeOf<T>::value(const TypeRegistry& registry)
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

} // namespace language::plugins

#endif
