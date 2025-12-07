#include "language/plugins/plugin_info.h"

#include <algorithm>
#include <cassert>

using namespace std;

namespace downward::cli::plugins {

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
