#ifndef PLUGINS_RAW_REGISTRY_H
#define PLUGINS_RAW_REGISTRY_H

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry_types.h"

#include <vector>

namespace downward::cli::plugins {

template <template <bool...> typename T, bool... b>
concept instantiable = requires { typename T<b...>; };

template <template <bool...> typename T, bool... b>
struct suffix_instantiable_h : std::false_type {};

template <template <bool...> typename T, bool b, bool... tail>
struct suffix_instantiable_h<T, b, tail...>
    : std::bool_constant<
          instantiable<T, b, tail...> ||
          suffix_instantiable_h<T, tail...>::value> {};

template <template <bool...> typename T, bool... b>
concept suffix_instantiable = suffix_instantiable_h<T, b...>::value;

template <template <bool...> typename T, bool... b>
concept strict_suffix_instantiable =
    suffix_instantiable<T, b...> && !instantiable<T, b...>;

template <template <bool...> typename T, bool... b>
concept partial_specialization = !strict_suffix_instantiable<T, b...>;

class RawRegistry {
    std::vector<std::unique_ptr<CategoryPlugin>> category_plugins;
    std::vector<std::unique_ptr<SubcategoryPlugin>> subcategory_plugins;
    std::vector<std::unique_ptr<EnumPlugin>> enum_plugins;
    std::vector<std::unique_ptr<Plugin>> plugins;

    FeatureTypes collect_types(std::vector<std::string>& errors) const;
    void validate_category_names(std::vector<std::string>& errors) const;
    SubcategoryPlugins
    collect_subcategory_plugins(std::vector<std::string>& errors) const;
    Features collect_features(
        const SubcategoryPlugins& subcategory_plugins,
        std::vector<std::string>& errors) const;

public:
    template <std::derived_from<SubcategoryPlugin> T>
    void insert_subcategory_plugin()
    {
        subcategory_plugins.push_back(std::make_unique<T>());
    }

    template <typename T>
    CategoryPlugin& insert_category_plugin(std::string name)
    {
        return *category_plugins.emplace_back(
            std::make_unique<SharedTypedCategoryPlugin<T>>(std::move(name)));
    }

    template <template <bool...> typename T, bool... b>
    void insert_category_plugins()
    {
        if constexpr (instantiable<T, b...>) {
            category_plugins.push_back(std::make_unique<T<b...>>());
        } else {
            insert_category_plugins<T, b..., true>();
            insert_category_plugins<T, b..., false>();
        }
    }

    template <typename T>
    void insert_enum_plugin(
        std::initializer_list<std::pair<std::string, std::string>> enum_values)
    {
        enum_plugins.emplace_back(
            std::make_unique<TypedEnumPlugin<T>>(enum_values));
    }

    template <typename T>
    void insert_feature_plugin()
    {
        plugins.emplace_back(std::make_unique<FeaturePlugin<T>>());
    }

    template <template <bool...> typename T, bool... b>
        requires partial_specialization<T, b...>
    void insert_feature_plugins()
    {
        if constexpr (instantiable<T, b...>) {
            insert_feature_plugin<T<b...>>();
        } else {
            insert_feature_plugins<T, b..., true>();
            insert_feature_plugins<T, b..., false>();
        }
    }

    Registry construct_registry() const;
};
} // namespace downward::cli::plugins

#endif
