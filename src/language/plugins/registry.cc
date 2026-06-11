#include "language/plugins/registry.h"

#include "language/plugins/plugin.h"

#include <algorithm>
#include <ranges>

using namespace std;

namespace language::plugins {
Registry::Registry(
    FeatureTypes&& feature_types,
    SubcategoryPlugins&& subcategory_plugins,
    Features&& features)
    : feature_types(move(feature_types))
    , subcategory_plugins(move(subcategory_plugins))
    , features(move(features))
{
}

shared_ptr<const Feature> Registry::get_feature(const string& name) const
{
    if (!has_feature(name)) {
        throw MissingFeatureError(
            "could not find a feature named '{}' in the registry",
            name);
    }
    return features.at(name);
}

const SubcategoryPlugin&
Registry::get_subcategory_plugin(const string& subcategory) const
{
    if (!subcategory_plugins.contains(subcategory)) {
        throw MissingSubCategoryError(
            "attempt to retrieve non-existing group info from registry: {}",
            subcategory);
    }
    return *subcategory_plugins.at(subcategory);
}

bool Registry::has_feature(const string& name) const
{
    return features.contains(name);
}

const FeatureTypes& Registry::get_feature_types() const
{
    return feature_types;
}

vector<const SubcategoryPlugin*> Registry::get_subcategory_plugins() const
{
    vector<const SubcategoryPlugin*> result;
    for (const auto& val : subcategory_plugins | views::values) {
        result.push_back(val);
    }
    return result;
}

vector<shared_ptr<const Feature>> Registry::get_features() const
{
    vector<shared_ptr<const Feature>> result;
    for (const auto& val : features | views::values) { result.push_back(val); }
    return result;
}
} // namespace language::plugins
