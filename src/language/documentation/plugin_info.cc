#include "language/documentation/plugin_info.h"

using namespace std;

namespace language::documentation {

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

} // namespace language::documentation
