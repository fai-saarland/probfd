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

class MissingFunctionError : public std::runtime_error {
public:
    explicit MissingFunctionError(const std::string& message)
        : runtime_error(message.c_str())
    {
    }

    explicit MissingFunctionError(const char* message)
        : runtime_error(message)
    {
    }
};

class MissingTypeDeclarationError : public std::runtime_error {
public:
    explicit MissingTypeDeclarationError(const std::string& message)
        : runtime_error(message.c_str())
    {
    }

    explicit MissingTypeDeclarationError(const char* message)
        : runtime_error(message)
    {
    }
};

class MissingTopicError : public std::runtime_error {
public:
    explicit MissingTopicError(const std::string& message)
        : runtime_error(message.c_str())
    {
    }

    explicit MissingTopicError(const char* message)
        : runtime_error(message)
    {
    }
};

struct TypeDeclarationComparator {
    using is_transparent = void;

    bool operator()(
        const InternalTypeDeclarationBase& lhs,
        const InternalTypeDeclarationBase& rhs) const;
    bool
    operator()(const std::string& lhs, const InternalTypeDeclarationBase& rhs)
        const;
    bool
    operator()(const InternalTypeDeclarationBase& lhs, const std::string& rhs)
        const;
};

struct TopicComparator {
    using is_transparent = void;

    bool
    operator()(const DocumentationTopic& lhs, const DocumentationTopic& rhs)
        const;

    bool
    operator()(const std::string& lhs, const DocumentationTopic& rhs) const;
    bool
    operator()(const DocumentationTopic& lhs, const std::string& rhs) const;
};

struct FeatureComparator {
    using is_transparent = void;

    bool operator()(
        const std::unique_ptr<InternalFunctionDefinitionBase>& lhs,
        const std::unique_ptr<InternalFunctionDefinitionBase>& rhs) const;

    bool operator()(
        const std::string& lhs,
        const std::unique_ptr<InternalFunctionDefinitionBase>& rhs) const;

    bool operator()(
        const std::unique_ptr<InternalFunctionDefinitionBase>& lhs,
        const std::string& rhs) const;
};

class Namespace;

class Registry {
    std::unique_ptr<Namespace> global_namespace;

    std::deque<DocumentationTopic> topics;
    std::map<std::string, DocumentationTopic*> topics_by_name;

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

    template <std::derived_from<DocumentationTopic> T>
    DocumentationTopic& insert_documentation_topic()
    {
        auto& s = topics.emplace_back(T());
        auto [it, inserted] = topics_by_name.emplace(s.get_topic_name(), &s);

        if (!inserted) {
            throw downward::utils::CriticalError(
                "Documentation topic with name {} already exists.",
                s.get_topic_name());
        }

        return s;
    }

    DocumentationTopic& get_topic_by_name(const std::string& subcategory);

    const DocumentationTopic&
    get_topic_by_name(const std::string& subcategory) const;

    auto get_topics() const { return std::views::all(topics); }
};

class Namespace {
    std::map<std::string, std::unique_ptr<Namespace>> nested_namespaces;

    std::set<InternalTypeDeclarationBase, TypeDeclarationComparator>
        types_declarations;
    std::vector<InternalEnumDeclarationBase> enum_declarations;
    std::deque<std::unique_ptr<InternalFunctionDefinitionBase>> functions;
    std::map<std::string, const InternalFunctionDefinitionBase*>
        functions_by_name;

public:
    Namespace& get_or_create_nested_namespace(const std::string& name);

    Namespace& create_nested_namespace(const std::string& name);

    Namespace& get_nested_namespace(const std::string& name);
    const Namespace& get_nested_namespace(const std::string& name) const;

    template <typename T>
    const InternalTypeDeclarationBase&
    insert_shared_type_declaration(std::string name, std::string synopsis)
    {
        return insert_type_declaration<std::shared_ptr<T>>(
            std::move(name),
            std::move(synopsis));
    }

    template <typename T>
    const InternalTypeDeclarationBase&
    insert_type_declaration(std::string name, std::string synopsis)
    {
        for (const auto& c : types_declarations) {
            if (const std::type_index t = typeid(T);
                c.get_pointer_type() == t) {
                throw downward::utils::CriticalError(
                    "Type for class '{}' already defined.",
                    t.name());
            }
        }

        for (const auto& namespace_name :
             nested_namespaces | std::views::keys) {
            if (name == namespace_name) {
                throw downward::utils::CriticalError(
                    "Type '{}' has already been defined as a namespace.",
                    name);
            }
        }

        auto [it, inserted] = types_declarations.emplace(
            InternalTypeDeclaration<T>(std::move(name), std::move(synopsis)));

        if (!inserted) {
            throw downward::utils::CriticalError(
                "Type with name {} already exists.",
                it->get_identifier());
        }

        TypeRegistry::instance()->create_feature_type(*it);

        return *it;
    }

    template <
        template <bool...> typename T,
        bool... b,
        typename F1,
        typename F2>
    auto insert_type_declarations(const F1& name, const F2& synopsis)
    {
        if constexpr (instantiable<T, b...>) {
            return std::tie(
                insert_type_declaration<T<b...>>(
                    name.template operator()<b...>(),
                    synopsis.template operator()<b...>()));
        } else {
            return std::tuple_cat(
                insert_type_declarations<T, b..., true>(name, synopsis),
                insert_type_declarations<T, b..., false>(name, synopsis));
        }
    }

    template <
        template <bool...> typename T,
        bool... b,
        typename F1,
        typename F2>
    auto insert_shared_type_declarations(const F1& name, const F2& synopsis)
    {
        if constexpr (instantiable<T, b...>) {
            return std::tie(
                insert_shared_type_declaration<T<b...>>(
                    name.template operator()<b...>(),
                    synopsis.template operator()<b...>()));
        } else {
            return std::tuple_cat(
                insert_shared_type_declarations<T, b..., true>(name, synopsis),
                insert_shared_type_declarations<T, b..., false>(
                    name,
                    synopsis));
        }
    }

    template <typename T>
    void insert_enum_declaration(
        std::initializer_list<std::pair<std::string, std::string>> enum_values)
    {
        auto& enum_plugin = enum_declarations.emplace_back(
            InternalEnumDeclaration<T>(enum_values));
        TypeRegistry::instance()->create_enum_type(enum_plugin);
    }

    template <std::derived_from<InternalFunctionDefinitionBase> T>
    T& insert_function_definition()
    {
        auto& f = *functions.emplace_back(std::make_unique<T>());
        const auto [it, inserted] =
            functions_by_name.emplace(f.get_identifier(), &f);

        if (!inserted) {
            throw downward::utils::CriticalError(
                "Function with name '{}' already defined.",
                f.get_identifier());
        }

        for (const auto& namespace_name :
             nested_namespaces | std::views::keys) {
            if (f.get_identifier() == namespace_name) {
                throw downward::utils::CriticalError(
                    "Function with name '{}' has already been defined as a "
                    "namespace.",
                    f.get_identifier());
            }
        }

        return static_cast<T&>(f);
    }

    template <typename F>
    auto& insert_function_definition(std::string name, F&& func)
    {
        auto* f = new InternalFunctionDefinition(name, std::forward<F>(func));

        functions.emplace_back(f);
        const auto [it, inserted] =
            functions_by_name.emplace(f->get_identifier(), f);

        if (!inserted) {
            throw downward::utils::CriticalError(
                "Function with name {} already defined.",
                f->get_identifier());
        }

        return *f;
    }

    template <template <bool...> typename T, bool... b>
        requires partial_specialization<T, b...>
    void insert_function_definitions()
    {
        if constexpr (instantiable<T, b...>) {
            insert_function_definition<T<b...>>();
        } else {
            insert_function_definitions<T, b..., true>();
            insert_function_definitions<T, b..., false>();
        }
    }

    bool has_function(const std::string& name) const;

    const InternalFunctionDefinitionBase&
    get_function_definition(const std::string& name) const;

    auto get_function_definitions() const
    {
        return functions_by_name | std::views::values;
    }

    auto get_type_declarations() const
    {
        return std::views::all(types_declarations);
    }

    auto get_nested_namespaces() { return std::views::all(nested_namespaces); }

    auto get_nested_namespaces() const
    {
        return std::views::all(nested_namespaces);
    }
};
} // namespace downward::cli::plugins

#endif
