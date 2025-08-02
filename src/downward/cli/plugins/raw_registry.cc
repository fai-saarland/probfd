#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/utils/collections.h"
#include "downward/utils/strings.h"

#include <string>

using namespace std;

namespace downward::cli::plugins {

FeatureTypes RawRegistry::collect_types(vector<string>& errors) const
{
    FeatureTypes feature_types;
    unordered_map<type_index, vector<string>> type_to_names;

    for (const auto& enum_plugin : enum_plugins) {
        vector<string>& names = type_to_names[enum_plugin->get_type()];
        if (names.empty()) {
            TypeRegistry::instance()->create_enum_type(*enum_plugin);
        }
        names.push_back("EnumPlugin(" + enum_plugin->get_class_name() + ")");
    }

    for (const auto& category_plugin : category_plugins) {
        vector<string>& names =
            type_to_names[category_plugin->get_pointer_type()];
        if (names.empty()) {
            const FeatureType& type =
                TypeRegistry::instance()->create_feature_type(*category_plugin);
            feature_types.push_back(&type);
        }
        names.push_back(
            "CategoryPlugin(" + category_plugin->get_class_name() + ", " +
            category_plugin->get_category_name() + ")");
    }

    // Check that each type index is only used once for either an enum or a
    // category.
    for (const auto& names : type_to_names | views::values) {
        if (names.size() > 1) {
            errors.push_back(
                "Multiple plugins are defined for the same type: " +
                utils::join(names, ", ") + "'.");
        }
    }
    return feature_types;
}

void RawRegistry::validate_category_names(vector<string>& errors) const
{
    unordered_map<string, vector<string>> category_name_to_class_names;
    unordered_map<string, vector<string>> class_name_to_category_names;
    for (const auto& category_plugin : category_plugins) {
        string class_name = category_plugin->get_class_name();
        string category_name = category_plugin->get_category_name();
        category_name_to_class_names[category_name].push_back(class_name);
        class_name_to_category_names[class_name].push_back(category_name);
    }

    for (const auto& [category_name, class_names] :
         category_name_to_class_names) {
        if (class_names.size() > 1) {
            errors.push_back(
                "Multiple CategoryPlugins have the name '" + category_name +
                "': " + utils::join(class_names, ", ") + ".");
        }
    }
    for (const auto& [class_name, category_names] :
         class_name_to_category_names) {
        if (category_names.size() > 1) {
            errors.push_back(
                "Multiple CategoryPlugins are defined for the class '" +
                class_name + "': " + utils::join(category_names, ", ") + ".");
        }
    }
}

SubcategoryPlugins
RawRegistry::collect_subcategory_plugins(vector<string>& errors) const
{
    SubcategoryPlugins subcategory_plugin_map;
    unordered_map<string, int> occurrences;

    for (const auto& subcategory_plugin : subcategory_plugins) {
        ++occurrences[subcategory_plugin->get_subcategory_name()];
        subcategory_plugin_map.emplace(
            subcategory_plugin->get_subcategory_name(),
            subcategory_plugin.get());
    }

    for (const auto& [subcategory, occurrence] : occurrences) {
        if (occurrence > 1) {
            errors.push_back(
                "The SubcategoryPlugin '" + subcategory + "' is defined " +
                to_string(occurrence) + " times.");
        }
    }
    return subcategory_plugin_map;
}

Features RawRegistry::collect_features(
    const SubcategoryPlugins& subcategory_plugins,
    vector<string>& errors) const
{
    Features features;
    unordered_map<string, int> feature_key_occurrences;
    for (const auto& plugin : plugins) {
        shared_ptr<Feature> feature = plugin->create_feature();
        string feature_key = feature->get_key();
        feature_key_occurrences[feature_key]++;
        features[feature_key] = move(feature);
    }

    // Check that feature_keys  are unique
    for (const auto& [feature_key, occurrences] : feature_key_occurrences) {
        if (occurrences > 1) {
            errors.push_back(
                to_string(occurrences) + " Features are defined for the key '" +
                feature_key + "'.");
        }
    }

    // Check that all subcategories used in features are defined
    for (const auto& [feature_key, feature] : features) {
        string subcategory = feature->get_subcategory();

        if (!subcategory.empty() &&
            !subcategory_plugins.contains(subcategory)) {
            const Type& type = feature->get_type();
            errors.push_back(
                "Missing SubcategoryPlugin '" + subcategory + "' for Plugin '" +
                feature_key + "' of type " + type.name());
        }
    }

    // Check that all types used in features are defined
    unordered_set<type_index> missing_types;
    for (const auto& [feature_key, feature] : features) {
        const Type& type = feature->get_type();
        if (type == TypeRegistry::NO_TYPE) {
            errors.push_back(
                "Missing Plugin for type of feature '" + feature_key + "'.");
        }

        unordered_map<string, int> parameter_occurrences;
        for (const ArgumentInfo& arg_info : feature->get_arguments()) {
            if (arg_info.type == TypeRegistry::NO_TYPE) {
                errors.push_back(
                    "Missing Plugin for type of parameter '" + arg_info.key +
                    "' of feature '" + feature_key + "'.");
            }
            ++parameter_occurrences[arg_info.key];
        }
        // Check that parameters are unique
        for (const auto& [parameter, parameter_occurrence] :
             parameter_occurrences) {
            if (parameter_occurrence > 1) {
                errors.push_back(
                    "The parameter '" + parameter + "' in '" + feature_key +
                    "' is defined " + to_string(parameter_occurrence) +
                    " times.");
            }
        }
    }

    return features;
}

Registry RawRegistry::construct_registry() const
{
    vector<string> errors;
    FeatureTypes feature_types = collect_types(errors);
    validate_category_names(errors);
    SubcategoryPlugins subcategory_plugins =
        collect_subcategory_plugins(errors);
    Features features = collect_features(subcategory_plugins, errors);

    if (!errors.empty()) {
        ranges::sort(errors);
        cerr << "Internal registry error(s):" << endl;
        for (const string& error : errors) { cerr << error << endl; }
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    return Registry(
        move(feature_types),
        move(subcategory_plugins),
        move(features));
}
} // namespace downward::cli::plugins
