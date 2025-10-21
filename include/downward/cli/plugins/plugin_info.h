#ifndef PLUGINS_PLUGIN_INFO_H
#define PLUGINS_PLUGIN_INFO_H

#include "downward/cli/plugins/bounds.h"
#include "downward/cli/plugins/types.h"

#include <string>
#include <typeindex>
#include <vector>

namespace downward::cli::plugins {

struct PropertyInfo {
    std::string property;
    std::string description;

    PropertyInfo(const std::string& property, const std::string& description);
};

struct NoteInfo {
    std::string name;
    std::string description;
    bool long_text;

    NoteInfo(
        const std::string& name,
        const std::string& description,
        bool long_text);
};

struct LanguageSupportInfo {
    std::string feature;
    std::string description;

    LanguageSupportInfo(
        const std::string& feature,
        const std::string& description);
};
} // namespace downward::cli::plugins

#endif
