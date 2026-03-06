#include "language/ast/function_declaration.h"

using namespace std;

namespace language::parser {

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

} // namespace language::parser