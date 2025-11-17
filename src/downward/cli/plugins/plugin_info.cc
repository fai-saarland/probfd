#include "downward/cli/plugins/plugin_info.h"

#include <algorithm>
#include <cassert>

using namespace std;

namespace downward::cli::plugins {

ArgumentInfo::ArgumentInfo(const string& key, const string& default_value)
    : key(key)
    , default_value(default_value)
{
}

ArgumentInfo ArgumentInfo::make_optional(const std::string& default_value)
{
    return ArgumentInfo("", default_value);
}

ArgumentInfo ArgumentInfo::make_required()
{
    return ArgumentInfo("", "");
}

ArgumentInfo ArgumentInfo::make_named_optional(
    const std::string& key,
    const std::string& default_value)
{
    return ArgumentInfo(key, default_value);
}

ArgumentInfo ArgumentInfo::make_named_required(const std::string& key)
{
    return ArgumentInfo(key, "");
}

bool ArgumentInfo::has_default() const
{
    return !default_value.empty();
}

void ArgumentInfo::set_key(const std::string& key)
{
    this->key = key;
}

void ArgumentInfo::set_default(const std::string& default_value)
{
    this->default_value = default_value;
}

PropertyInfo::PropertyInfo(const string& property, const string& description)
    : property(property)
    , description(description)
{
}

NoteInfo::NoteInfo(
    const string& name,
    const string& description,
    bool long_text)
    : name(name)
    , description(description)
    , long_text(long_text)
{
}

LanguageSupportInfo::LanguageSupportInfo(
    const string& feature,
    const string& description)
    : feature(feature)
    , description(description)
{
}
} // namespace downward::cli::plugins
