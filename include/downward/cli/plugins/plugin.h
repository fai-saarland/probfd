#ifndef PLUGINS_PLUGIN_H
#define PLUGINS_PLUGIN_H

#include "downward/cli/plugins/options.h"
#include "downward/cli/plugins/plugin_info.h"

#include "downward/utils/tuples.h"

#include <any>
#include <string>
#include <type_traits>
#include <typeindex>
#include <vector>

namespace downward::utils {
class Context;
}

namespace downward::cli::plugins {
class Feature {
    const Type& type;
    std::vector<ArgumentInfo> arguments;

    std::string key;
    std::string title;
    std::string synopsis;
    std::string subcategory;
    std::vector<PropertyInfo> properties;
    std::vector<LanguageSupportInfo> language_support;
    std::vector<NoteInfo> notes;

public:
    Feature(const Type& type, const std::string& key);
    virtual ~Feature() = default;
    Feature(const Feature&) = delete;

    virtual std::any
    construct(const Options& opts, const downward::utils::Context& context)
        const = 0;

    /* Add option with default value. Use def_val=ArgumentInfo::NO_DEFAULT for
       optional parameters without default values. */
    template <typename T>
    void add_option(
        const std::string& key,
        const std::string& help = "",
        const std::string& default_value = "",
        const Bounds& bounds = Bounds::unlimited());

    template <typename T>
    void add_list_option(
        const std::string& key,
        const std::string& help = "",
        const std::string& default_value = "");

    void document_subcategory(const std::string& subcategory);
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

    const Type& get_type() const;
    std::string get_key() const;
    std::string get_title() const;
    std::string get_synopsis() const;
    std::string get_subcategory() const;
    const std::vector<ArgumentInfo>& get_arguments() const;
    const std::vector<PropertyInfo>& get_properties() const;
    const std::vector<LanguageSupportInfo>& get_language_support() const;
    const std::vector<NoteInfo>& get_notes() const;
};

template <typename ReturnType>
class TypedFeature : public Feature {
    using BasePtr = std::shared_ptr<ReturnType>;

public:
    explicit TypedFeature(const std::string& key)
        : Feature(TypeRegistry::instance()->get_type<BasePtr>(), key)
    {
    }

    std::any
    construct(const Options& options, const downward::utils::Context& context)
        const override
    {
        return {this->create_component(options, context)};
    }

    virtual std::shared_ptr<ReturnType>
    create_component(const Options&, const downward::utils::Context&) const = 0;
};

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

class Plugin {
protected:
    Plugin() = default;

public:
    virtual ~Plugin() = default;

    virtual std::shared_ptr<Feature> create_feature() const = 0;
};

template <typename T>
class FeaturePlugin : public Plugin {
public:
    std::shared_ptr<Feature> create_feature() const override
    {
        return std::make_shared<T>();
    }
};

/*
  The CategoryPlugin class contains meta-information for a given
  category of feature (e.g. "SearchAlgorithm" or "MergeStrategyFactory").
*/
class CategoryPlugin {
    std::type_index pointer_type;
    std::string class_name;

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
        const std::string& class_name,
        const std::string& category_name);

public:
    virtual ~CategoryPlugin() = default;

    void document_synopsis(const std::string& synopsis);

    std::type_index get_pointer_type() const;
    std::string get_category_name() const;
    std::string get_class_name() const;
    std::string get_synopsis() const;
};

template <typename T>
class TypedCategoryPlugin : public CategoryPlugin {
public:
    explicit TypedCategoryPlugin(const std::string& category_name)
        : CategoryPlugin(
              typeid(std::shared_ptr<T>),
              typeid(std::shared_ptr<T>).name(),
              category_name)
    {
    }
};

class SubcategoryPlugin {
    std::string subcategory_name;
    std::string title;
    std::string synopsis;

public:
    explicit SubcategoryPlugin(const std::string& subcategory);

    void document_title(const std::string& title);
    void document_synopsis(const std::string& synopsis);

    std::string get_subcategory_name() const;
    std::string get_title() const;
    std::string get_synopsis() const;
};

class EnumPlugin {
    std::type_index type;
    std::string class_name;
    EnumInfo enum_info;

protected:
    EnumPlugin(
        std::type_index type,
        const std::string& class_name,
        std::initializer_list<std::pair<std::string, std::string>> enum_values);

public:
    std::type_index get_type() const;
    std::string get_class_name() const;
    const EnumInfo& get_enum_info() const;
};

template <typename T>
class TypedEnumPlugin : public EnumPlugin {
public:
    TypedEnumPlugin(
        std::initializer_list<std::pair<std::string, std::string>> enum_values)
        : EnumPlugin(typeid(T), typeid(std::shared_ptr<T>).name(), enum_values)
    {
    }
};

template <typename T>
void Feature::add_option(
    const std::string& key,
    const std::string& help,
    const std::string& default_value,
    const Bounds& bounds)
{
    using U = std::conditional_t<
        std::is_same_v<T, std::chrono::duration<double>>,
        double,
        T>;

    arguments.emplace_back(
        key,
        help,
        TypeRegistry::instance()->get_type<U>(),
        default_value,
        bounds);
}

template <typename T>
void Feature::add_list_option(
    const std::string& key,
    const std::string& help,
    const std::string& default_value)
{
    using U = std::conditional_t<
        std::is_same_v<T, std::chrono::duration<double>>,
        double,
        T>;

    add_option<std::vector<U>>(
        key,
        help,
        default_value,
        Bounds::unlimited());
}
} // namespace downward::cli::plugins

#endif
