#ifndef PLUGINS_REGISTRY_H
#define PLUGINS_REGISTRY_H

#include "downward/cli/plugins/registry_types.h"

#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_set>

namespace downward::cli::plugins {

class MissingFeatureError : public std::runtime_error {
public:
    explicit MissingFeatureError(const std::string& message)
        : runtime_error(message.c_str())
    {
    }

    explicit MissingFeatureError(const char* message)
        : runtime_error(message)
    {
    }
};

class MissingSubCategoryError : public std::runtime_error {
public:
    explicit MissingSubCategoryError(const std::string& message)
        : runtime_error(message.c_str())
    {
    }

    explicit MissingSubCategoryError(const char* message)
        : runtime_error(message)
    {
    }
};

class Feature;
class Registry {
    /*
      List of FeatureType* for types of all features in use.

      The FeatureType objects contains the documentation of those types and is
      used to generate the complete help output.
    */
    FeatureTypes feature_types;

    /*
      Maps subcategory keys to the SubcategoryPlugin with that key.

      A subcategory is a set of plugins of the same type that are grouped
      together in the user documentation. For example, all PDB heuristics
      are grouped together on the page documenting heuristics.
    */
    SubcategoryPlugins subcategory_plugins;

    /*
      Maps keys (e.g. 'astar') to features with that key.

      A Feature objects contains its documentation, arguments, and
      has the ability to construct the objects of that type.
    */
    Features features;

public:
    Registry(
        FeatureTypes&& feature_types,
        SubcategoryPlugins&& subcategory_plugins,
        Features&& features);

    bool has_feature(const std::string& name) const;
    std::shared_ptr<const Feature> get_feature(const std::string& name) const;
    const SubcategoryPlugin&
    get_subcategory_plugin(const std::string& subcategory) const;

    const FeatureTypes& get_feature_types() const;
    std::vector<const SubcategoryPlugin*> get_subcategory_plugins() const;
    std::vector<std::shared_ptr<const Feature>> get_features() const;
};
} // namespace downward::cli::plugins

#endif
