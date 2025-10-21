#include "downward/cli/plugins/plugin_info.h"

#include <algorithm>
#include <cassert>

using namespace std;

namespace downward::cli::plugins {
const string ArgumentInfo::NO_DEFAULT = "<none>";

ArgumentInfo::ArgumentInfo(
    const string& key,
    const Type& type,
    const string& default_value)
    : key(key)
    , type(type)
    , default_value(default_value)
{
}

bool ArgumentInfo::is_optional() const
{
    return !default_value.empty();
}

bool ArgumentInfo::has_default() const
{
    return is_optional() && default_value != NO_DEFAULT;
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
