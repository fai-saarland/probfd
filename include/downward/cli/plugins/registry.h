#ifndef PLUGINS_RAW_REGISTRY_H
#define PLUGINS_RAW_REGISTRY_H

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry_types.h"

#include <set>
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

class MissingFeatureError : public std::runtime_error {
public:
    explicit MissingFeatureError(const std::string& message)
        : runtime_error(message.c_str())
    {
    }

    explicit MissingFeatureError(const char* message)
        : runtime_error(message)
    {
    }
};

class MissingCategoryError : public std::runtime_error {
public:
    explicit MissingCategoryError(const std::string& message)
        : runtime_error(message.c_str())
    {
    }

    explicit MissingCategoryError(const char* message)
        : runtime_error(message)
    {
    }
};

class MissingSubCategoryError : public std::runtime_error {
public:
    explicit MissingSubCategoryError(const std::string& message)
        : runtime_error(message.c_str())
    {
    }

    explicit MissingSubCategoryError(const char* message)
        : runtime_error(message)
    {
    }
};

struct CategoryComparator {
    using is_transparent = void;

    bool operator()(
        const std::unique_ptr<CategoryPlugin>& lhs,
        const std::unique_ptr<CategoryPlugin>& rhs) const;

    bool operator()(
        const std::string& lhs,
        const std::unique_ptr<CategoryPlugin>& rhs) const;

    bool operator()(
        const std::unique_ptr<CategoryPlugin>& lhs,
        const std::string& rhs) const;
};

struct SubCategoryComparator {
    using is_transparent = void;

    bool operator()(
        const std::unique_ptr<SubcategoryPlugin>& lhs,
        const std::unique_ptr<SubcategoryPlugin>& rhs) const;

    bool operator()(
        const std::string& lhs,
        const std::unique_ptr<SubcategoryPlugin>& rhs) const;

    bool operator()(
        const std::unique_ptr<SubcategoryPlugin>& lhs,
        const std::string& rhs) const;
};

struct FeatureComparator {
    using is_transparent = void;

    bool operator()(
        const std::shared_ptr<Feature>& lhs,
        const std::shared_ptr<Feature>& rhs) const;

    bool operator()(const std::string& lhs, const std::shared_ptr<Feature>& rhs)
        const;

    bool operator()(const std::shared_ptr<Feature>& lhs, const std::string& rhs)
        const;
};

class Registry {
    std::set<std::unique_ptr<CategoryPlugin>, CategoryComparator>
        category_plugins;
    std::vector<std::unique_ptr<EnumPlugin>> enum_plugins;
    std::set<std::unique_ptr<SubcategoryPlugin>, SubCategoryComparator>
        subcategory_plugins;
    std::set<std::shared_ptr<Feature>, FeatureComparator> features;

    void add_feature_plugin(const Plugin& plugin);

public:
    template <std::derived_from<SubcategoryPlugin> T>
    void insert_subcategory_plugin()
    {
        auto [it, inserted] = subcategory_plugins.insert(std::make_unique<T>());

        if (!inserted) {
            throw downward::utils::CriticalError(
                "Sub-category with name {} already exists.",
                (*it)->get_subcategory_name());
        }
    }

    template <typename T>
    const CategoryPlugin&
    insert_shared_category_plugin(std::string name, std::string synopsis)
    {
        return insert_category_plugin<std::shared_ptr<T>>(
            std::move(name),
            std::move(synopsis));
    }

    template <typename T>
    const CategoryPlugin&
    insert_category_plugin(std::string name, std::string synopsis)
    {
        for (const auto& c : category_plugins) {
            if (const std::type_index t = typeid(T);
                c->get_pointer_type() == t) {
                throw downward::utils::CriticalError(
                    "CategoryPlugin for class '{}' already defined.",
                    t.name());
            }
        }

        auto [it, inserted] = category_plugins.emplace(
            std::make_unique<TypedCategoryPlugin<T>>(
                std::move(name),
                std::move(synopsis)));

        if (!inserted) {
            throw downward::utils::CriticalError(
                "Category with name {} already exists.",
                (*it)->get_category_name());
        }

        TypeRegistry::instance()->create_feature_type(**it);

        return **it;
    }

    template <
        template <bool...> typename T,
        bool... b,
        typename F1,
        typename F2>
    auto insert_category_plugins(const F1& name, const F2& synopsis)
    {
        if constexpr (instantiable<T, b...>) {
            return std::tie(
                insert_category_plugin<T<b...>>(
                    name.template operator()<b...>(),
                    synopsis.template operator()<b...>()));
        } else {
            return std::tuple_cat(
                insert_category_plugins<T, b..., true>(name, synopsis),
                insert_category_plugins<T, b..., false>(name, synopsis));
        }
    }

    template <
        template <bool...> typename T,
        bool... b,
        typename F1,
        typename F2>
    auto insert_shared_category_plugins(const F1& name, const F2& synopsis)
    {
        if constexpr (instantiable<T, b...>) {
            return std::tie(
                insert_shared_category_plugin<T<b...>>(
                    name.template operator()<b...>(),
                    synopsis.template operator()<b...>()));
        } else {
            return std::tuple_cat(
                insert_shared_category_plugins<T, b..., true>(name, synopsis),
                insert_shared_category_plugins<T, b..., false>(name, synopsis));
        }
    }

    template <typename T>
    void insert_enum_plugin(
        std::initializer_list<std::pair<std::string, std::string>> enum_values)
    {
        auto& enum_plugin = enum_plugins.emplace_back(
            std::make_unique<TypedEnumPlugin<T>>(enum_values));
        TypeRegistry::instance()->create_enum_type(*enum_plugin);
    }

    template <typename T>
    void insert_feature_plugin()
    {
        auto p = std::make_unique<FeaturePlugin<T>>();
        add_feature_plugin(*p);
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

    bool has_feature(const std::string& name) const;
    std::shared_ptr<const Feature> get_feature(const std::string& name) const;
    const SubcategoryPlugin&
    get_subcategory_plugin(const std::string& subcategory) const;

    auto get_subcategory_plugins() const
    {
        return std::views::all(subcategory_plugins);
    }

    auto get_features() const { return std::views::all(features); }

    auto get_categories() const { return std::views::all(category_plugins); }
};
} // namespace downward::cli::plugins

#endif
