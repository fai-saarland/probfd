#ifndef PLUGINS_REGISTRY_TYPES_H
#define PLUGINS_REGISTRY_TYPES_H

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace downward::cli::plugins {
class CategoryPlugin;
class EnumPlugin;
class Feature;
class FeatureType;
class Plugin;
class Registry;
class SubcategoryPlugin;

using EnumInfo = std::vector<std::pair<std::string, std::string>>;
} // namespace downward::cli::plugins
#endif
