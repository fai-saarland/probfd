#include "downward/cli/plugins/registry.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/utils/system.h"

#include <algorithm>

using namespace std;

namespace downward::cli::plugins {
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
            "could not find a feature named '" + name + "' in the registry");
    }
    return features.at(name);
}

const SubcategoryPlugin&
Registry::get_subcategory_plugin(const string& subcategory) const
{
    if (!subcategory_plugins.count(subcategory)) {
        throw MissingSubCategoryError(
            "attempt to retrieve non-existing group info from registry: " +
            string(subcategory));
    }
    return *subcategory_plugins.at(subcategory);
}

bool Registry::has_feature(const string& name) const
{
    return features.count(name);
}

const FeatureTypes& Registry::get_feature_types() const
{
    return feature_types;
}

vector<const SubcategoryPlugin*> Registry::get_subcategory_plugins() const
{
    vector<const SubcategoryPlugin*> result;
    for (const auto& pair : subcategory_plugins) {
        result.push_back(pair.second);
    }
    return result;
}

vector<shared_ptr<const Feature>> Registry::get_features() const
{
    vector<shared_ptr<const Feature>> result;
    for (const auto& pair : features) {
        result.push_back(pair.second);
    }
    return result;
}
} // namespace downward::cli::plugins
