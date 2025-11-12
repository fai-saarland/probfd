#include "downward/cli/plugins/registry.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/utils/collections.h"

#include <string>

using namespace std;

namespace downward::cli::plugins {

bool CategoryComparator::operator()(
    const std::unique_ptr<CategoryPlugin>& lhs,
    const std::unique_ptr<CategoryPlugin>& rhs) const
{
    return lhs->get_category_name() < rhs->get_category_name();
}

bool CategoryComparator::operator()(
    const std::string& lhs,
    const std::unique_ptr<CategoryPlugin>& rhs) const
{
    return lhs < rhs->get_category_name();
}

bool CategoryComparator::operator()(
    const std::unique_ptr<CategoryPlugin>& lhs,
    const std::string& rhs) const
{
    return lhs->get_category_name() < rhs;
}

bool SubCategoryComparator::operator()(
    const std::unique_ptr<SubcategoryPlugin>& lhs,
    const std::unique_ptr<SubcategoryPlugin>& rhs) const
{
    return lhs->get_subcategory_name() < rhs->get_subcategory_name();
}

bool SubCategoryComparator::operator()(
    const std::string& lhs,
    const std::unique_ptr<SubcategoryPlugin>& rhs) const
{
    return lhs < rhs->get_subcategory_name();
}

bool SubCategoryComparator::operator()(
    const std::unique_ptr<SubcategoryPlugin>& lhs,
    const std::string& rhs) const
{
    return lhs->get_subcategory_name() < rhs;
}

bool FeatureComparator::operator()(
    const std::shared_ptr<Feature>& lhs,
    const std::shared_ptr<Feature>& rhs) const
{
    return lhs->get_key() < rhs->get_key();
}

bool FeatureComparator::operator()(
    const std::string& lhs,
    const std::shared_ptr<Feature>& rhs) const
{
    return lhs < rhs->get_key();
}

bool FeatureComparator::operator()(
    const std::shared_ptr<Feature>& lhs,
    const std::string& rhs) const
{
    return lhs->get_key() < rhs;
}

void Registry::add_feature_plugin(const Plugin& plugin)
{
    const auto [it, inserted] = features.insert(plugin.create_feature());

    if (!inserted) {
        throw utils::CriticalError(
            "Feature with name {} already defined.",
            (*it)->get_key());
    }

    if (const string subcategory = (*it)->get_subcategory();
        !subcategory.empty() && !subcategory_plugins.contains(subcategory)) {
        throw downward::utils::CriticalError(
            "Missing SubcategoryPlugin '{}' for Plugin '{}' of type {}",
            subcategory,
            (*it)->get_key(),
            (*it)->get_type().name());
    }
}

shared_ptr<const Feature> Registry::get_feature(const string& name) const
{
    if (const auto it = features.find(name); it != features.end()) {
        return *it;
    }

    throw MissingFeatureError(
        "could not find a feature named '" + name + "' in the registry");
}

const SubcategoryPlugin&
Registry::get_subcategory_plugin(const string& subcategory) const
{
    if (const auto it = subcategory_plugins.find(subcategory);
        it != subcategory_plugins.end()) {
        return **it;
    }

    throw MissingSubCategoryError(
        "attempt to retrieve non-existing group info from registry: " +
        string(subcategory));
}

bool Registry::has_feature(const string& name) const
{
    return features.contains(name);
}

} // namespace downward::cli::plugins
