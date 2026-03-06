#ifndef LANGUAGE_AST_FUNCTION_DEFINITION_H
#define LANGUAGE_AST_FUNCTION_DEFINITION_H

#include "language/ast/compilation_context.h"
#include "language/ast/function_declaration.h"

#include "language/documentation/plugin_info.h"

#include "language/typed_ast/type_aliases.h"
#include "language/typed_ast/type_registry.h"

#include "language/tuples.h"

#include "language/utils/concepts.h"
#include "language/utils/function_traits.h"

#include <any>
#include <functional>
#include <string>
#include <type_traits>
#include <typeindex>
#include <vector>

namespace language::parser {
class CompilationContext;
class NamespaceScope;
} // namespace language::parser

namespace language::parser {

template <typename T, typename... Args>
    requires std::constructible_from<T, Args...>
T constructor(Args... args)
{
    return T(std::forward<std::remove_reference_t<Args>>(args)...);
}

template <typename Base, typename T, typename... Args>
std::shared_ptr<Base> construct_shared(Args... args)
    requires std::constructible_from<T, Args...> &&
             std::derived_from<T, Base> && (!std::is_reference_v<T>) &&
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

class InternalFunctionDefinitionBase : public NamespaceLevelDeclaration {
protected:
    typed_ast::AnyFunctionType func;
    std::type_index return_type;
    std::vector<std::type_index> arg_types;

    std::vector<ArgumentInfo> arguments;

    // Documentation
    std::string title;
    std::string synopsis;
    std::vector<documentation::PropertyInfo> properties;
    std::vector<documentation::LanguageSupportInfo> language_support;
    std::vector<documentation::NoteInfo> notes;
    std::vector<std::string> argument_docs;

public:
    InternalFunctionDefinitionBase(
        std::string identifier,
        typed_ast::AnyFunctionType func,
        std::type_index return_type,
        std::vector<std::type_index> arg_types);

    InternalFunctionDefinitionBase(const InternalFunctionDefinitionBase&) =
        delete;

    std::type_index get_return_type() const;

    const std::vector<std::type_index>& get_arg_types() const;

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

    auto get_argument_infos() const { return arguments | std::views::all; }

    std::string get_title() const;
    std::string get_synopsis() const;

    const std::vector<std::string>& get_argument_docs() const;

    const std::vector<documentation::PropertyInfo>& get_properties() const;

    const std::vector<documentation::LanguageSupportInfo>&
    get_language_support() const;

    const std::vector<documentation::NoteInfo>& get_notes() const;

    void register_declarations(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::NamespaceScope& scope,
        typed_ast::TypeRegistry& type_registry) const override;

    void visit(NamespaceLevelDeclarationVisitor& visitor) override;
};

template <auto f>
auto& insert_function_definition(
    NamespaceLevelDeclarationList& list,
    std::string name);

template <template <bool...> typename T, bool... b>
    requires utils::partial_specialization<T, b...>
void insert_function_definitions(NamespaceLevelDeclarationList& list);

} // namespace language::parser

#include "language/context.h"

#include "language/typed_ast/construct_context.h"
#include "language/typed_ast/decorated_feature_literal_node.h"
#include "language/typed_ast/variable_environment.h"

namespace language::parser {

template <typename F>
struct ContextExtendedFunction;

template <typename R, typename... Args>
struct ContextExtendedFunction<R (*)(Args...)> {
    template <R (*f)(Args...)>
    static R func(const Context&, Args... args)
    {
        return f(args...);
    }
};

template <typename R, typename... Args>
struct ContextExtendedFunction<R (*)(const Context&, Args...)> {
    template <R (*f)(const Context&, Args...)>
    static constexpr R (*func)(const Context&, Args...) = f;
};

template <auto ext_f, std::size_t... indices>
static std::any as_any_function(typed_ast::ConstructContext& stack)
{
    static constexpr auto nargs = utils::num_args<decltype(ext_f)>;

    try {
        auto return_value = std::invoke(
            ext_f,
            stack,
            stack.get_variable<utils::ArgType<decltype(ext_f), indices>>(
                indices)...);
        stack.pop_variables(nargs);
        return return_value;
    } catch (std::exception& e) {
        stack.error("Exception thrown during construction:\n{}", e.what());
    }
}

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

class InternalFunctionValue : public typed_ast::FunctionValue {
    typed_ast::AnyFunctionType function;
    std::vector<ArgumentInfo> arguments;

public:
    explicit InternalFunctionValue(typed_ast::AnyFunctionType function)
        : function(function)
    {
    }

    const std::vector<ArgumentInfo>& get_arguments() override
    {
        return arguments;
    }

    std::unique_ptr<typed_ast::DecoratedExpressionNode>
    create_load_node() override
    {
        return std::make_unique<typed_ast::FeatureLiteralNode>(function);
    }
};

template <auto ext_f, std::size_t index, std::size_t... indices>
auto& insert_function_definition(
    NamespaceLevelDeclarationList& list,
    std::string name,
    std::index_sequence<index, indices...>)
{
    using FType = decltype(ext_f);

    return list.emplace_declaration<InternalFunctionDefinitionBase>(
        name,
        as_any_function<ext_f, indices...>,
        typeid(utils::ReturnType<FType>),
        std::vector<std::type_index>{
            typeid(utils::ArgType<FType, indices>)...});
}

template <auto f>
auto& insert_function_definition(
    NamespaceLevelDeclarationList& list,
    std::string name)
{
    constexpr auto* ext_f =
        ContextExtendedFunction<decltype(f)>::template func<f>;

    using FType = decltype(ext_f);

    return insert_function_definition<ext_f>(
        list,
        name,
        std::make_index_sequence<utils::num_args<FType>>{});
}

template <template <bool...> typename T, bool... b>
    requires utils::partial_specialization<T, b...>
void insert_function_definitions(NamespaceLevelDeclarationList& list)
{
    if constexpr (utils::instantiable<T, b...>) {
        T<b...>::operator()(list);
    } else {
        insert_function_definitions<T, b..., true>(list);
        insert_function_definitions<T, b..., false>(list);
    }
}

} // namespace language::parser

#endif
