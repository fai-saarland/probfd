#ifndef PLUGINS_PLUGIN_H
#define PLUGINS_PLUGIN_H

#include "downward/cli/plugins/options.h"
#include "downward/cli/plugins/plugin_info.h"

#include "downward/utils/tuples.h"

#include <any>
#include <functional>
#include <string>
#include <type_traits>
#include <typeindex>
#include <vector>

namespace downward::utils {
class Context;
}

namespace downward::cli::plugins {

class Feature {
protected:
    std::vector<ArgumentInfo> arguments;

    std::vector<std::string> argument_docs;
    std::string key;
    std::string title;
    std::string synopsis;
    std::vector<PropertyInfo> properties;
    std::vector<LanguageSupportInfo> language_support;
    std::vector<NoteInfo> notes;

public:
    Feature(std::string key, std::size_t num_args);

    virtual ~Feature() = default;

    Feature(const Feature&) = delete;

    void make_optional_argument_with_default(
        std::size_t i,
        const std::string& key,
        const std::string& default_value,
        const std::string& help = "");

    void make_required_argument(
        std::size_t i,
        const std::string& key,
        const std::string& help = "");

    void document_title(const std::string& title);
    void document_synopsis(const std::string& note);
    void
    document_property(const std::string& property, const std::string& note);
    void document_language_support(
        const std::string& feature,
        const std::string& note);
    void document_note(
        const std::string& title,
        const std::string& note,
        bool long_text = false);

    std::string get_key() const;
    std::string get_title() const;
    std::string get_synopsis() const;
    const std::vector<ArgumentInfo>& get_arguments() const;
    const std::vector<std::string>& get_argument_docs() const;
    const std::vector<PropertyInfo>& get_properties() const;
    const std::vector<LanguageSupportInfo>& get_language_support() const;
    const std::vector<NoteInfo>& get_notes() const;

    virtual std::any
    construct(const Options& opts, const downward::utils::Context& context)
        const = 0;

    virtual const FunctionType& get_type() const = 0;
};

template <typename ReturnType, typename... Args>
class TypedFeature : public Feature {
    std::function<ReturnType(Args...)> f;

public:
    explicit TypedFeature(std::string key, std::function<ReturnType(Args...)> f)
        : Feature(std::move(key), sizeof...(Args))
        , f(std::move(f))
    {
    }

    std::any
    construct(const Options& options, const downward::utils::Context& context)
        const override
    {
        return construct(
            options,
            context,
            std::make_index_sequence<sizeof...(Args)>{});
    }

    const FunctionType& get_type() const override
    {
        return TypeRegistry::instance()
            ->get_function_type<ReturnType, Args...>();
    }

private:
    template <std::size_t... indices>
    std::any construct(
        const Options& options,
        const downward::utils::Context& context,
        std::index_sequence<indices...>) const
    {
        try {
            return std::invoke(f, options.get<Args>(indices)...);
        } catch (std::exception& e) {
            context.error("Exception thrown during contruction:\n{}", e.what());
        }
    }
};

template <typename ReturnType, typename... Args>
class TypedFeatureWithContext : public Feature {
    std::function<ReturnType(const downward::utils::Context&, Args...)> f;

public:
    explicit TypedFeatureWithContext(
        std::string key,
        std::function<ReturnType(const downward::utils::Context&, Args...)> f)
        : Feature(std::move(key), sizeof...(Args))
        , f(std::move(f))
    {
    }

    std::any
    construct(const Options& options, const downward::utils::Context& context)
        const override
    {
        return construct(
            options,
            context,
            std::make_index_sequence<sizeof...(Args)>{});
    }

    const FunctionType& get_type() const override
    {
        return TypeRegistry::instance()
            ->get_function_type<ReturnType, Args...>();
    }

private:
    template <std::size_t... indices>
    std::any construct(
        const Options& options,
        const downward::utils::Context& context,
        std::index_sequence<indices...>) const
    {
        try {
            return std::invoke(f, context, options.get<Args>(indices)...);
        } catch (std::exception& e) {
            context.error("Exception thrown during contruction:\n{}", e.what());
        }
    }
};

template <typename ReturnType, typename... Args>
using SharedTypedFeature = TypedFeature<std::shared_ptr<ReturnType>, Args...>;

template <typename ReturnType, typename... Args>
using SharedTypedFeatureWithContext =
    TypedFeatureWithContext<std::shared_ptr<ReturnType>, Args...>;

/*
  Expects constructor arguments of T. Consecutive arguments may be
  grouped in a tuple. All tuples in the arguments will be flattened
  before calling the constructor. The resulting arguments will be used
  as arguments to make_shared.
*/
template <typename T, typename... Arguments>
std::shared_ptr<T> make_shared_from_arg_tuples(Arguments... arguments)
{
    return std::apply(
        []<typename... A>(A&&... flattened_args)
            requires requires { T{flattened_args...}; }
        { return std::make_shared<T>(std::forward<A>(flattened_args)...); },
        downward::utils::flatten_tuple(
            std::tuple<Arguments...>(std::forward<Arguments>(arguments)...)));
}

/*
  The CategoryPlugin class contains meta-information for a given
  category of feature (e.g. "SearchAlgorithm" or "MergeStrategyFactory").
*/
class CategoryPlugin {
    std::type_index pointer_type;

    /*
      The category name should be "user-friendly". It is for example used
      as the name of the wiki page that documents this feature type.
      It follows wiki conventions (e.g. "Heuristic", "SearchAlgorithm",
      "ShrinkStrategy").
    */
    std::string category_name;

    /*
      General documentation for the feature type. This is included at
      the top of the wiki page for this feature type.
    */
    std::string synopsis;

protected:
    CategoryPlugin(
        std::type_index pointer_type,
        std::string category_name,
        std::string synopsis);

public:
    virtual ~CategoryPlugin() = default;

    std::type_index get_pointer_type() const;
    std::string get_category_name() const;
    std::string get_class_name() const;
    std::string get_synopsis() const;
};

template <typename T>
    requires std::is_class_v<T>
class TypedCategoryPlugin : public CategoryPlugin {
public:
    explicit TypedCategoryPlugin(
        std::string category_name,
        std::string synopsis)
        : CategoryPlugin(
              typeid(T),
              std::move(category_name),
              std::move(synopsis))
    {
    }
};

template <typename T>
using SharedTypedCategoryPlugin = TypedCategoryPlugin<std::shared_ptr<T>>;

class EnumPlugin {
    std::type_index type;
    EnumInfo enum_info;

protected:
    EnumPlugin(
        std::type_index type,
        std::initializer_list<std::pair<std::string, std::string>> enum_values);

public:
    std::type_index get_type() const;
    std::string get_class_name() const;
    const EnumInfo& get_enum_info() const;
};

template <typename T>
    requires std::is_enum_v<T>
class TypedEnumPlugin : public EnumPlugin {
public:
    TypedEnumPlugin(
        std::initializer_list<std::pair<std::string, std::string>> enum_values)
        : EnumPlugin(typeid(T), enum_values)
    {
    }
};

class SubcategoryPlugin {
    std::string subcategory_name;
    std::string title;
    std::string synopsis;

    std::vector<const CategoryPlugin*> member_types;
    std::vector<const EnumPlugin*> enum_types;
    std::vector<const Feature*> features;

public:
    explicit SubcategoryPlugin(const std::string& subcategory);

    void document_title(const std::string& title);
    void document_synopsis(const std::string& synopsis);

    void add_category(const CategoryPlugin& category);
    void add_enum(const EnumPlugin& enum_plugin);
    void add_feature(const Feature& feature);

    auto get_types() { return std::views::all(member_types); }

    auto get_types() const { return std::views::all(member_types); }

    auto get_enums() { return std::views::all(enum_types); }

    auto get_enums() const { return std::views::all(enum_types); }

    auto get_features() { return std::views::all(features); }

    auto get_features() const { return std::views::all(features); }

    std::string get_subcategory_name() const;
    std::string get_title() const;
    std::string get_synopsis() const;
};

inline void Feature::make_optional_argument_with_default(
    std::size_t i,
    const std::string& arg_key,
    const std::string& default_value,
    const std::string& help)
{
    if (std::ranges::contains(arguments, arg_key, &ArgumentInfo::key)) {
        throw downward::utils::CriticalError(
            "Duplicate argument keyword argument '{}' of feature {}.",
            arg_key,
            this->key);
    }

    auto& info = arguments[i];
    info.set_key(arg_key);
    info.set_default(default_value);

    argument_docs[i] = help;
}

inline void Feature::make_required_argument(
    std::size_t i,
    const std::string& arg_key,
    const std::string& help)
{
    if (std::ranges::contains(arguments, arg_key, &ArgumentInfo::key)) {
        throw downward::utils::CriticalError(
            "Duplicate argument keyword argument '{}' of feature {}.",
            arg_key,
            this->key);
    }

    auto& info = arguments[i];
    info.set_key(arg_key);

    argument_docs[i] = help;
}

} // namespace downward::cli::plugins

#endif
