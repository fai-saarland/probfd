#include "language/plugins/registry.h"

#include "language/plugins/plugin.h"

#include "downward/utils/collections.h"

#include <string>

using namespace std;

namespace downward::cli::plugins {

bool TypeDeclarationComparator::operator()(
    const InternalTypeDeclarationBase& lhs,
    const InternalTypeDeclarationBase& rhs) const
{
    return lhs.get_identifier() < rhs.get_identifier();
}

bool TypeDeclarationComparator::operator()(
    const std::string& lhs,
    const InternalTypeDeclarationBase& rhs) const
{
    return lhs < rhs.get_identifier();
}

bool TypeDeclarationComparator::operator()(
    const InternalTypeDeclarationBase& lhs,
    const std::string& rhs) const
{
    return lhs.get_identifier() < rhs;
}

bool TopicComparator::operator()(
    const DocumentationTopic& lhs,
    const DocumentationTopic& rhs) const
{
    return lhs.get_topic_name() < rhs.get_topic_name();
}

bool TopicComparator::operator()(
    const std::string& lhs,
    const DocumentationTopic& rhs) const
{
    return lhs < rhs.get_topic_name();
}

bool TopicComparator::operator()(
    const DocumentationTopic& lhs,
    const std::string& rhs) const
{
    return lhs.get_topic_name() < rhs;
}

bool FeatureComparator::operator()(
    const std::unique_ptr<InternalFunctionDefinitionBase>& lhs,
    const std::unique_ptr<InternalFunctionDefinitionBase>& rhs) const
{
    return lhs->get_identifier() < rhs->get_identifier();
}

bool FeatureComparator::operator()(
    const std::string& lhs,
    const std::unique_ptr<InternalFunctionDefinitionBase>& rhs) const
{
    return lhs < rhs->get_identifier();
}

bool FeatureComparator::operator()(
    const std::unique_ptr<InternalFunctionDefinitionBase>& lhs,
    const std::string& rhs) const
{
    return lhs->get_identifier() < rhs;
}

Registry::Registry()
    : global_namespace(std::make_unique<Namespace>())
{
}

Namespace& Registry::get_global_name_space()
{
    return *global_namespace;
}

const Namespace& Registry::get_global_name_space() const
{
    return *global_namespace;
}

Namespace&
Registry::get_or_create_namespace(const std::vector<std::string>& name)
{
    Namespace* n = global_namespace.get();

    for (const std::string& part : name) {
        n = &n->get_or_create_nested_namespace(part);
    }

    return *n;
}

Namespace& Registry::create_namespace(const std::vector<std::string>& name)
{
    if (name.empty()) {
        throw std::domain_error(
            "Global namespace always exists and cannot be created.");
    }

    Namespace* n = global_namespace.get();

    for (const std::string& part : name) {
        n = &n->create_nested_namespace(part);
    }

    return *n;
}

Namespace& Registry::get_namespace(const std::vector<std::string>& name)
{
    Namespace* n = global_namespace.get();

    for (const std::string& part : name) { n = &n->get_nested_namespace(part); }

    return *n;
}

DocumentationTopic& Registry::get_topic_by_name(const string& subcategory)
{
    if (const auto it = topics_by_name.find(subcategory);
        it != topics_by_name.end()) {
        return *it->second;
    }

    throw MissingTopicError(
        std::format(
            "attempt to retrieve non-existing topic from registry: {}",
            subcategory));
}

const DocumentationTopic&
Registry::get_topic_by_name(const string& subcategory) const
{
    if (const auto it = topics_by_name.find(subcategory);
        it != topics_by_name.end()) {
        return *it->second;
    }

    throw MissingTopicError(
        std::format(
            "Attempt to retrieve non-existing topic from registry: {}",
            subcategory));
}

const Namespace&
Registry::get_namespace(const std::vector<std::string>& name) const
{
    Namespace* n = global_namespace.get();

    for (const std::string& part : name) { n = &n->get_nested_namespace(part); }

    return *n;
}

Namespace& Namespace::get_or_create_nested_namespace(const std::string& name)
{
    return *nested_namespaces.emplace(name, std::make_unique<Namespace>())
                .first->second;
}

Namespace& Namespace::create_nested_namespace(const std::string& name)
{
    auto [it, inserted] = nested_namespaces.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(name),
        std::forward_as_tuple());

    if (!inserted) {
        throw std::domain_error(
            std::format("Nested namespace {} already exists.", name));
    }

    return *it->second;
}

Namespace& Namespace::get_nested_namespace(const std::string& name)
{
    if (const auto it = nested_namespaces.find(name);
        it != nested_namespaces.end()) {
        return *it->second;
    }

    throw std::domain_error(
        std::format("Nested namespace {} does not exist.", name));
}

const Namespace& Namespace::get_nested_namespace(const std::string& name) const
{
    if (const auto it = nested_namespaces.find(name);
        it != nested_namespaces.end()) {
        return *it->second;
    }

    throw std::domain_error(
        std::format("Nested namespace {} does not exist.", name));
}

const InternalFunctionDefinitionBase&
Namespace::get_function_definition(const string& name) const
{
    if (const auto it = functions_by_name.find(name);
        it != functions_by_name.end()) {
        return *it->second;
    }

    throw MissingFunctionError(
        std::format(
            "Could not find a function named '{}' in the registry.",
            name));
}

bool Namespace::has_function(const string& name) const
{
    return functions_by_name.contains(name);
}

} // namespace downward::cli::plugins
