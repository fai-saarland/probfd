#ifndef LANGUAGE_PLUGINS_PLUGIN_H
#define LANGUAGE_PLUGINS_PLUGIN_H

#include "language/documentation/plugin_info.h"

#include "language/plugins/options.h"
#include "language/plugins/type_registry.h"

#include "language/context.h"
#include "language/tuples.h"

#include <any>
#include <functional>
#include <string>
#include <type_traits>
#include <typeindex>
#include <vector>

namespace language::plugins {
class FunctionType;
}

namespace language::plugins {

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

template <typename T>
struct strip_context;

template <typename R, typename... Args>
struct strip_context<R(const Context&, Args...)> {
    using type = R(Args...);
};

template <typename R, typename... Args>
struct strip_context<R(Args...)> {
    using type = R(Args...);
};

} // namespace detail

template <typename T, typename... Args>
    requires std::constructible_from<T, Args...>
T constructor(Args... args)
{
    return T(std::forward<std::remove_reference_t<Args>>(args)...);
}

template <typename Base, typename T, typename... Args>
std::shared_ptr<Base> construct_shared(Args... args)
    requires std::constructible_from<T, Args...> &&
             std::derived_from<T, Base> && !std::is_reference_v<T> &&
             (!std::is_reference_v<Args> && ...)
{
    return std::make_shared<T>(
        std::forward<std::remove_reference_t<Args>>(args)...);
}

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
        flatten_tuple(
            std::tuple<Arguments...>(std::forward<Arguments>(arguments)...)));
}

class DuplicateKeywordError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    template <class... Args>
    explicit DuplicateKeywordError(
        std::format_string<Args...> fmt,
        Args&&... args)
        : std::runtime_error(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};

struct ArgumentInfo {
    std::string key;
    std::string default_value;

private:
    ArgumentInfo(const std::string& key, const std::string& default_value);

public:
    static ArgumentInfo make_optional(const std::string& default_value);
    static ArgumentInfo make_required();

    static ArgumentInfo make_named_optional(
        const std::string& key,
        const std::string& default_value);
    static ArgumentInfo make_named_required(const std::string& key);

    bool has_default() const;

    void set_key(const std::string& key);
    void set_default(const std::string& default_value);

    friend bool operator==(const ArgumentInfo&, const ArgumentInfo&) = default;
};

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

    virtual std::any
    construct(const Options& opts, const Context& context) const = 0;

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
        const std::string& name,
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
    construct(const Options& options, const Context& context) const override
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
        return TypeRegistry::instance()
            ->get_function_type<typename detail::strip_context<FType>::type>();
    }

private:
    template <std::size_t findex, std::size_t... indices>
    std::any construct(
        const Options& options,
        const Context& context,
        std::index_sequence<findex, indices...>) const
        requires(nargs > 0)
    {
        if constexpr (std::same_as<detail::ArgType<FType, 0>, const Context&>) {
            return std::invoke(
                f,
                context,
                options.get<detail::ArgType<FType, indices>>(
                    indices - 1,
                    context)...);
        } else {
            return std::invoke(
                f,
                options.get<detail::ArgType<FType, findex>>(findex, context),
                options.get<detail::ArgType<FType, indices>>(
                    indices,
                    context)...);
        }
    }

    template <std::size_t... indices>
    std::any
    construct(const Options&, const Context&, std::index_sequence<indices...>)
        const
        requires(nargs == 0)
    {
        return std::invoke(f);
    }
};

template <typename F>
InternalFunctionDefinition(std::string key, F f)
    -> InternalFunctionDefinition<detail::FSignature<F>>;

inline void InternalFunctionDefinitionBase::make_optional_argument_with_default(
    std::size_t i,
    const std::string& key,
    const std::string& default_value,
    const std::string& help)
{
    if (std::ranges::contains(arguments, key, &ArgumentInfo::key)) {
        throw DuplicateKeywordError(
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
        throw DuplicateKeywordError(
            "Duplicate argument keyword argument '{}' of function '{}'.",
            key,
            this->identifier);
    }

    auto& info = arguments[i];
    info.set_key(key);

    argument_docs[i] = help;
}

} // namespace language::plugins

template <>
struct std::formatter<language::plugins::ArgumentInfo> {
    bool with_default = false;

    // bool with_type = false;

    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        auto pos = ctx.begin();

        for (; pos != ctx.end() && *pos != '}'; ++pos) {
            switch (*pos) {
            case '}': return pos;
            case 'd': {
                if (with_default) {
                    throw std::format_error(
                        std::format("Repeated format specifier: '{}'", *pos));
                }
                with_default = true;
            } break;
                /*case 't': {
                    if (with_type) {
                        throw std::format_error(
                            std::format("Repeated format specifier: '{}'",
                *pos));
                    }
                    with_type = true;
                } break;*/
            default:
                throw std::format_error(
                    std::format("Illegal format specifier: '{}'", *pos));
            }
        }

        return pos;
    }

    template <typename FormatContext>
    auto format(const language::plugins::ArgumentInfo& info, FormatContext& ctx)
        const
    {
        auto it = std::format_to(ctx.out(), "{}", info.key);

        // if (with_type) { it = std::format_to(it, " : {}", info.type.name());
        // }

        if (with_default) {
            it = std::format_to(it, " = {}", info.default_value);
        }

        return it;
    }
};

#endif
