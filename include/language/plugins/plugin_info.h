#ifndef LANGUAGE_PLUGINS_PLUGIN_INFO_H
#define LANGUAGE_PLUGINS_PLUGIN_INFO_H

#include "language/plugins/types.h"

#include <string>

namespace language::plugins {

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
