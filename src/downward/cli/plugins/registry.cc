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

const Feature& Registry::get_feature(const string& name) const
{
    if (const auto it = features.find(name); it != features.end()) {
        return **it;
    }

    throw MissingFeatureError(
        "could not find a feature named '" + name + "' in the registry");
}

const SubcategoryPlugin&
Registry::get_subcategory_plugin(const string& subcategory) const
{
    if (const auto it = subcategory_plugins.find(subcategory);
        it != subcategory_plugins.end()) {
        return *it;
    }

    throw MissingSubCategoryError(
        "attempt to retrieve non-existing group info from registry: " +
        subcategory);
}

bool Registry::has_feature(const string& name) const
{
    return features.contains(name);
}

} // namespace downward::cli::plugins
