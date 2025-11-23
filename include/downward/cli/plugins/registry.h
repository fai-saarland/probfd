#ifndef PLUGINS_REGISTRY_H
#define PLUGINS_REGISTRY_H

#include "downward/cli/plugins/plugin.h"

#include <deque>
#include <map>
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

    bool operator()(const CategoryPlugin& lhs, const CategoryPlugin& rhs) const;
    bool operator()(const std::string& lhs, const CategoryPlugin& rhs) const;
    bool operator()(const CategoryPlugin& lhs, const std::string& rhs) const;
};

struct SubCategoryComparator {
    using is_transparent = void;

    bool operator()(const SubcategoryPlugin& lhs, const SubcategoryPlugin& rhs)
        const;

    bool operator()(const std::string& lhs, const SubcategoryPlugin& rhs) const;
    bool operator()(const SubcategoryPlugin& lhs, const std::string& rhs) const;
};

struct FeatureComparator {
    using is_transparent = void;

    bool operator()(
        const std::unique_ptr<Feature>& lhs,
        const std::unique_ptr<Feature>& rhs) const;

    bool operator()(const std::string& lhs, const std::unique_ptr<Feature>& rhs)
        const;

    bool operator()(const std::unique_ptr<Feature>& lhs, const std::string& rhs)
        const;
};

class Namespace;

class Registry {
    std::unique_ptr<Namespace> global_namespace;

    std::deque<SubcategoryPlugin> subcategory_plugins;
    std::map<std::string, SubcategoryPlugin*> subcategories_by_name;

public:
    Registry();

    Namespace& get_global_name_space();
    const Namespace& get_global_name_space() const;

    Namespace& get_or_create_namespace(const std::vector<std::string>& name);

    Namespace& create_namespace(const std::vector<std::string>& name);

    Namespace& get_namespace(const std::vector<std::string>& name);
    const Namespace& get_namespace(const std::vector<std::string>& name) const;

    template <typename... Args>
        requires(std::convertible_to<Args, std::string> && ...)
    Namespace& get_or_create_namespace(Args&&... name_parts)
    {
        return get_or_create_namespace(std::vector<std::string>{name_parts...});
    }

    template <typename... Args>
        requires(std::convertible_to<Args, std::string> && ...)
    Namespace& create_namespace(Args&&... name_parts)
    {
        return create_namespace(std::vector<std::string>{name_parts...});
    }

    template <typename... Args>
        requires(std::convertible_to<Args, std::string> && ...)
    Namespace& get_namespace(Args&&... name_parts)
    {
        return get_namespace(std::vector<std::string>{name_parts...});
    }

    template <typename... Args>
        requires(std::convertible_to<Args, std::string> && ...)
    const Namespace& get_namespace(Args&&... name_parts) const
    {
        return get_namespace(std::vector<std::string>{name_parts...});
    }

    template <std::derived_from<SubcategoryPlugin> T>
    SubcategoryPlugin& insert_subcategory_plugin()
    {
        auto& s = subcategory_plugins.emplace_back(T());
        auto [it, inserted] =
            subcategories_by_name.emplace(s.get_subcategory_name(), &s);

        if (!inserted) {
            throw downward::utils::CriticalError(
                "Sub-category with name {} already exists.",
                s.get_subcategory_name());
        }

        return s;
    }

    SubcategoryPlugin& get_subcategory_plugin(const std::string& subcategory);

    const SubcategoryPlugin&
    get_subcategory_plugin(const std::string& subcategory) const;

    auto get_subcategory_plugins() const
    {
        return std::views::all(subcategory_plugins);
    }
};

class Namespace {
    std::map<std::string, std::unique_ptr<Namespace>> children;

    std::set<CategoryPlugin, CategoryComparator> category_plugins;
    std::vector<EnumPlugin> enum_plugins;
    std::deque<std::unique_ptr<Feature>> features;
    std::map<std::string, const Feature*> features_by_name;

public:
    Namespace& get_or_create_nested_namespace(const std::string& name);

    Namespace& create_nested_namespace(const std::string& name);

    Namespace& get_nested_namespace(const std::string& name);
    const Namespace& get_nested_namespace(const std::string& name) const;

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
                c.get_pointer_type() == t) {
                throw downward::utils::CriticalError(
                    "CategoryPlugin for class '{}' already defined.",
                    t.name());
            }
        }

        auto [it, inserted] = category_plugins.emplace(
            TypedCategoryPlugin<T>(std::move(name), std::move(synopsis)));

        if (!inserted) {
            throw downward::utils::CriticalError(
                "Category with name {} already exists.",
                it->get_category_name());
        }

        TypeRegistry::instance()->create_feature_type(*it);

        return *it;
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
        auto& enum_plugin =
            enum_plugins.emplace_back(TypedEnumPlugin<T>(enum_values));
        TypeRegistry::instance()->create_enum_type(enum_plugin);
    }

    template <std::derived_from<Feature> T>
    T& insert_feature_plugin()
    {
        auto& f = *features.emplace_back(std::make_unique<T>());
        const auto [it, inserted] = features_by_name.emplace(f.get_key(), &f);

        if (!inserted) {
            throw downward::utils::CriticalError(
                "Feature with name {} already defined.",
                f.get_key());
        }

        return static_cast<T&>(f);
    }

    template <typename R, typename... Args>
    TypedFeature<R, Args...>&
    insert_typed_feature_plugin(std::string name, R (*func)(Args...))
    {
        auto& f = *features.emplace_back(
            std::make_unique<TypedFeature<R, Args...>>(name, func));
        const auto [it, inserted] = features_by_name.emplace(f.get_key(), &f);

        if (!inserted) {
            throw downward::utils::CriticalError(
                "Feature with name {} already defined.",
                f.get_key());
        }

        return static_cast<TypedFeature<R, Args...>&>(f);
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
    const Feature& get_feature(const std::string& name) const;

    auto get_features() const { return features_by_name | std::views::values; }

    auto get_categories() const { return std::views::all(category_plugins); }

    auto get_nested_namespaces() { return std::views::all(children); }

    auto get_nested_namespaces() const { return std::views::all(children); }
};
} // namespace downward::cli::plugins

#endif
