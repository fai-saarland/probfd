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

    std::vector<std::string> argument_docs;
    std::string key;
    std::string title;
    std::string synopsis;
    std::string subcategory;
    std::vector<PropertyInfo> properties;
    std::vector<LanguageSupportInfo> language_support;
    std::vector<NoteInfo> notes;

public:
    Feature(const Type& type, std::string key);
    virtual ~Feature() = default;
    Feature(const Feature&) = delete;

    virtual std::any
    construct(const Options& opts, const downward::utils::Context& context)
        const = 0;

    template <typename T>
    void
    add_optional_argument(const std::string& key, const std::string& help = "");

    template <typename T>
    void add_optional_argument_with_default(
        const std::string& key,
        const std::string& default_value,
        const std::string& help = "");

    template <typename T>
    void
    add_required_argument(const std::string& key, const std::string& help = "");

    template <typename T>
    void add_optional_list_argument(
        const std::string& key,
        const std::string& help = "");

    template <typename T>
    void add_optional_list_argument_with_default(
        const std::string& key,
        const std::string& default_value,
        const std::string& help = "");

    template <typename T>
    void add_required_list_argument(
        const std::string& key,
        const std::string& help = "");

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
    const std::vector<std::string>& get_argument_docs() const;
    const std::vector<PropertyInfo>& get_properties() const;
    const std::vector<LanguageSupportInfo>& get_language_support() const;
    const std::vector<NoteInfo>& get_notes() const;

private:
    void add_argument(ArgumentInfo info, const std::string& help);
};

template <typename ReturnType>
class TypedFeature : public Feature {
public:
    explicit TypedFeature(std::string key)
        : Feature(
              TypeRegistry::instance()->get_type<ReturnType>(),
              std::move(key))
    {
    }

    std::any
    construct(const Options& options, const downward::utils::Context& context)
        const override
    {
        return {this->create_component(options, context)};
    }

    virtual ReturnType
    create_component(const Options&, const downward::utils::Context&) const = 0;
};

template <typename ReturnType>
using SharedTypedFeature = TypedFeature<std::shared_ptr<ReturnType>>;

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

public:
    explicit SubcategoryPlugin(const std::string& subcategory);

    void document_title(const std::string& title);
    void document_synopsis(const std::string& synopsis);

    std::string get_subcategory_name() const;
    std::string get_title() const;
    std::string get_synopsis() const;
};

inline void Feature::add_argument(ArgumentInfo info, const std::string& help)
{
    if (const auto cmp = &ArgumentInfo::key;
        std::ranges::contains(arguments, info.key, cmp)) {
        throw downward::utils::CriticalError(
            "Duplicate argument keyword argument '{}' of feature {}.",
            info.key,
            key);
    }

    arguments.emplace_back(std::move(info));
    argument_docs.emplace_back(help);
}

template <typename T>
void Feature::add_optional_argument(
    const std::string& key,
    const std::string& help)
{
    add_argument(
        ArgumentInfo::make_optional(
            key,
            TypeRegistry::instance()->get_type<T>()),
        help);
}

template <typename T>
void Feature::add_optional_argument_with_default(
    const std::string& key,
    const std::string& default_value,
    const std::string& help)
{
    add_argument(
        ArgumentInfo::make_optional(
            key,
            TypeRegistry::instance()->get_type<T>(),
            default_value),
        help);
}

template <typename T>
void Feature::add_required_argument(
    const std::string& key,
    const std::string& help)
{
    add_argument(
        ArgumentInfo::make_required(
            key,
            TypeRegistry::instance()->get_type<T>()),
        help);
}

template <typename T>
void Feature::add_optional_list_argument(
    const std::string& key,
    const std::string& help)
{
    add_optional_argument<std::vector<T>>(key, help);
}

template <typename T>
void Feature::add_optional_list_argument_with_default(
    const std::string& key,
    const std::string& default_value,
    const std::string& help)
{
    add_optional_argument_with_default<std::vector<T>>(
        key,
        default_value,
        help);
}

template <typename T>
void Feature::add_required_list_argument(
    const std::string& key,
    const std::string& help)
{
    add_required_argument<std::vector<T>>(key, help);
}

} // namespace downward::cli::plugins

#endif
