#include "downward/cli/plugins/registry.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/utils/collections.h"

#include <string>

using namespace std;

namespace downward::cli::plugins {

bool CategoryComparator::operator()(
    const CategoryPlugin& lhs,
    const CategoryPlugin& rhs) const
{
    return lhs.get_category_name() < rhs.get_category_name();
}

bool CategoryComparator::operator()(
    const std::string& lhs,
    const CategoryPlugin& rhs) const
{
    return lhs < rhs.get_category_name();
}

bool CategoryComparator::operator()(
    const CategoryPlugin& lhs,
    const std::string& rhs) const
{
    return lhs.get_category_name() < rhs;
}

bool SubCategoryComparator::operator()(
    const SubcategoryPlugin& lhs,
    const SubcategoryPlugin& rhs) const
{
    return lhs.get_subcategory_name() < rhs.get_subcategory_name();
}

bool SubCategoryComparator::operator()(
    const std::string& lhs,
    const SubcategoryPlugin& rhs) const
{
    return lhs < rhs.get_subcategory_name();
}

bool SubCategoryComparator::operator()(
    const SubcategoryPlugin& lhs,
    const std::string& rhs) const
{
    return lhs.get_subcategory_name() < rhs;
}

bool FeatureComparator::operator()(
    const std::unique_ptr<Feature>& lhs,
    const std::unique_ptr<Feature>& rhs) const
{
    return lhs->get_key() < rhs->get_key();
}

bool FeatureComparator::operator()(
    const std::string& lhs,
    const std::unique_ptr<Feature>& rhs) const
{
    return lhs < rhs->get_key();
}

bool FeatureComparator::operator()(
    const std::unique_ptr<Feature>& lhs,
    const std::string& rhs) const
{
    return lhs->get_key() < rhs;
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

SubcategoryPlugin& Registry::get_subcategory_plugin(const string& subcategory)
{
    if (const auto it = subcategories_by_name.find(subcategory);
        it != subcategories_by_name.end()) {
        return *it->second;
    }

    throw MissingSubCategoryError(
        "attempt to retrieve non-existing group info from registry: " +
        subcategory);
}

const SubcategoryPlugin&
Registry::get_subcategory_plugin(const string& subcategory) const
{
    if (const auto it = subcategories_by_name.find(subcategory);
        it != subcategories_by_name.end()) {
        return *it->second;
    }

    throw MissingSubCategoryError(
        "attempt to retrieve non-existing group info from registry: " +
        subcategory);
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
    return *children.emplace(name, std::make_unique<Namespace>()).first->second;
}

Namespace& Namespace::create_nested_namespace(const std::string& name)
{
    auto [it, inserted] = children.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(name),
        std::forward_as_tuple());

    if (!inserted) {
        throw std::domain_error("Nested namespace already exists.");
    }

    return *it->second;
}

Namespace& Namespace::get_nested_namespace(const std::string& name)
{
    if (const auto it = children.find(name); it != children.end()) {
        return *it->second;
    }

    throw std::domain_error(
        std::format("Nested namespace {} does not exist.", name));
}

const Namespace& Namespace::get_nested_namespace(const std::string& name) const
{
    if (const auto it = children.find(name); it != children.end()) {
        return *it->second;
    }

    throw std::domain_error(
        std::format("Nested namespace {} does not exist.", name));
}

const Feature& Namespace::get_feature(const string& name) const
{
    if (const auto it = features.find(name); it != features.end()) {
        return **it;
    }

    throw MissingFeatureError(
        "could not find a feature named '" + name + "' in the registry");
}

bool Namespace::has_feature(const string& name) const
{
    return features.contains(name);
}

} // namespace downward::cli::plugins
