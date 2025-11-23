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

namespace detail {
template <typename F>
struct FuncTraits;

template <typename R, typename... Args>
struct FuncTraits<R(Args...)> {
    template <std::size_t I>
    using ArgType = std::tuple_element_t<I, std::tuple<Args...>>;

    static constexpr std::size_t num_args = sizeof...(Args);
};

template <typename F>
struct FSignatureS;

template <typename R, typename... Args>
struct FSignatureS<std::function<R(Args...)>> {
    using type = R(Args...);
};

template <typename F>
struct FSignatureS {
    using type =
        typename FSignatureS<decltype(std::function(std::declval<F>()))>::type;
};

template <typename F>
using FSignature = typename FSignatureS<F>::type;

template <typename T>
constexpr auto num_args = FuncTraits<T>::num_args;

template <typename T, std::size_t I>
using ArgType = typename FuncTraits<T>::template ArgType<I>;
} // namespace detail

template <typename T, typename... Args>
    requires std::constructible_from<T, Args...>
T constructor(Args... args)
{
    return T(std::forward<std::remove_reference_t<Args>>(args)...);
}

template <typename Base, typename T, typename... Args>
std::shared_ptr<Base> construct_shared(Args... args)
    requires std::constructible_from<T, Args...> && std::derived_from<T, Base>
{
    return std::make_shared<T>(
        std::forward<std::remove_reference_t<Args>>(args)...);
}

class InternalFunctionDefinitionBase {
protected:
    std::string identifier;
    std::vector<ArgumentInfo> arguments;

    // Documentation
    std::string title;
    std::string synopsis;
    std::vector<PropertyInfo> properties;
    std::vector<LanguageSupportInfo> language_support;
    std::vector<NoteInfo> notes;
    std::vector<std::string> argument_docs;

    InternalFunctionDefinitionBase(
        std::string identifier,
        std::size_t num_args);

public:
    virtual ~InternalFunctionDefinitionBase() = default;

    InternalFunctionDefinitionBase(const InternalFunctionDefinitionBase&) =
        delete;

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

    std::string get_identifier() const;
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

template <typename FType>
    requires std::is_function_v<FType>
class InternalFunctionDefinition : public InternalFunctionDefinitionBase {
    std::function<FType> f;

    static constexpr auto nargs = detail::num_args<FType>;

public:
    template <typename F>
    explicit InternalFunctionDefinition(std::string key, F f)
        : InternalFunctionDefinitionBase(std::move(key), nargs)
        , f(std::move(f))
    {
    }

    std::any
    construct(const Options& options, const downward::utils::Context& context)
        const override
    {
        try {
            return construct(
                options,
                context,
                std::make_index_sequence<nargs>{});
        } catch (std::exception& e) {
            context.error(
                "Exception thrown during construction:\n{}",
                e.what());
        }
    }

    const FunctionType& get_type() const override
    {
        return TypeRegistry::instance()->get_function_type<FType>();
    }

private:
    template <std::size_t findex, std::size_t... indices>
    std::any construct(
        const Options& options,
        const downward::utils::Context& context,
        std::index_sequence<findex, indices...>) const
        requires(nargs > 0)
    {
        if constexpr (std::same_as<
                          detail::ArgType<FType, 0>,
                          const downward::utils::Context&>) {
            return std::invoke(
                f,
                context,
                options.get<detail::ArgType<FType, indices>>(indices - 1)...);
        } else {
            return std::invoke(
                f,
                options.get<detail::ArgType<FType, findex>>(findex),
                options.get<detail::ArgType<FType, indices>>(indices)...);
        }
    }

    template <std::size_t... indices>
    std::any construct(
        const Options&,
        const downward::utils::Context&,
        std::index_sequence<indices...>) const
        requires(nargs == 0)
    {
        return std::invoke(f);
    }
};

template <typename F>
InternalFunctionDefinition(std::string key, F f)
    -> InternalFunctionDefinition<detail::FSignature<F>>;

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
  The InternalTypeDeclarationBase class contains meta-information for a C++
  type.
*/
class InternalTypeDeclarationBase {
    std::type_index pointer_type;

    std::string identifier;

    /*
      General documentation for the type.
      This is included at the top of the wiki page for this feature type.
    */
    std::string synopsis;

protected:
    InternalTypeDeclarationBase(
        std::type_index pointer_type,
        std::string type_identifier,
        std::string synopsis);

public:
    virtual ~InternalTypeDeclarationBase() = default;

    std::type_index get_pointer_type() const;
    std::string get_identifier() const;
    std::string get_class_name() const;
    std::string get_synopsis() const;
};

template <typename T>
    requires std::is_class_v<T>
class InternalTypeDeclaration : public InternalTypeDeclarationBase {
public:
    explicit InternalTypeDeclaration(
        std::string type_identifier,
        std::string synopsis)
        : InternalTypeDeclarationBase(
              typeid(T),
              std::move(type_identifier),
              std::move(synopsis))
    {
    }
};

template <typename T>
using InternalSharedTypeDeclaration =
    InternalTypeDeclaration<std::shared_ptr<T>>;

class InternalEnumDeclarationBase {
    std::type_index type;
    EnumInfo enum_info;

protected:
    InternalEnumDeclarationBase(
        std::type_index type,
        std::initializer_list<std::pair<std::string, std::string>> enum_values);

public:
    std::type_index get_type() const;
    std::string get_class_name() const;
    const EnumInfo& get_enum_info() const;
};

template <typename T>
    requires std::is_enum_v<T>
class InternalEnumDeclaration : public InternalEnumDeclarationBase {
public:
    InternalEnumDeclaration(
        std::initializer_list<std::pair<std::string, std::string>> enum_values)
        : InternalEnumDeclarationBase(typeid(T), enum_values)
    {
    }
};

class DocumentationTopic {
    std::string topic_name;
    std::string title;
    std::string synopsis;

    std::vector<const InternalTypeDeclarationBase*> member_types;
    std::vector<const InternalEnumDeclarationBase*> enum_types;
    std::vector<const InternalFunctionDefinitionBase*> features;

public:
    explicit DocumentationTopic(const std::string& subcategory);

    void document_title(const std::string& title);
    void document_synopsis(const std::string& synopsis);

    void
    add_type_declaration(const InternalTypeDeclarationBase& type_declaration);
    void add_enum(const InternalEnumDeclarationBase& enum_declaration);
    void add_function(const InternalFunctionDefinitionBase& function);

    auto get_types() { return std::views::all(member_types); }

    auto get_types() const { return std::views::all(member_types); }

    auto get_enums() { return std::views::all(enum_types); }

    auto get_enums() const { return std::views::all(enum_types); }

    auto get_functions() { return std::views::all(features); }

    auto get_functions() const { return std::views::all(features); }

    std::string get_topic_name() const;
    std::string get_title() const;
    std::string get_synopsis() const;
};

inline void InternalFunctionDefinitionBase::make_optional_argument_with_default(
    std::size_t i,
    const std::string& key,
    const std::string& default_value,
    const std::string& help)
{
    if (std::ranges::contains(arguments, key, &ArgumentInfo::key)) {
        throw downward::utils::CriticalError(
            "Duplicate argument keyword argument '{}' of function '{}'.",
            key,
            this->identifier);
    }

    auto& info = arguments[i];
    info.set_key(key);
    info.set_default(default_value);

    argument_docs[i] = help;
}

inline void InternalFunctionDefinitionBase::make_required_argument(
    std::size_t i,
    const std::string& key,
    const std::string& help)
{
    if (std::ranges::contains(arguments, key, &ArgumentInfo::key)) {
        throw downward::utils::CriticalError(
            "Duplicate argument keyword argument '{}' of function '{}'.",
            key,
            this->identifier);
    }

    auto& info = arguments[i];
    info.set_key(key);

    argument_docs[i] = help;
}

} // namespace downward::cli::plugins

#endif
