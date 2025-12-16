#include "language/plugins/internal_function_definition.h"

using namespace std;

namespace language::plugins {

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

InternalFunctionDefinitionBase::InternalFunctionDefinitionBase(
    string identifier,
    std::size_t num_args)
    : identifier(std::move(identifier))
    , arguments(num_args, ArgumentInfo::make_required())
    , argument_docs(num_args)
{
}

void InternalFunctionDefinitionBase::document_title(const string& title)
{
    this->title = title;
}

void InternalFunctionDefinitionBase::document_synopsis(const string& note)
{
    synopsis = note;
}

void InternalFunctionDefinitionBase::document_property(
    const string& property,
    const string& note)
{
    properties.emplace_back(property, note);
}

void InternalFunctionDefinitionBase::document_language_support(
    const string& feature,
    const string& note)
{
    language_support.emplace_back(feature, note);
}

void InternalFunctionDefinitionBase::document_note(
    const string& name,
    const string& note,
    bool long_text)
{
    notes.emplace_back(name, note, long_text);
}

string InternalFunctionDefinitionBase::get_identifier() const
{
    return identifier;
}

string InternalFunctionDefinitionBase::get_title() const
{
    return title;
}

string InternalFunctionDefinitionBase::get_synopsis() const
{
    return synopsis;
}

const vector<ArgumentInfo>&
InternalFunctionDefinitionBase::get_arguments() const
{
    return arguments;
}

const vector<std::string>&
InternalFunctionDefinitionBase::get_argument_docs() const
{
    return argument_docs;
}

const vector<PropertyInfo>&
InternalFunctionDefinitionBase::get_properties() const
{
    return properties;
}

const vector<LanguageSupportInfo>&
InternalFunctionDefinitionBase::get_language_support() const
{
    return language_support;
}

const vector<NoteInfo>& InternalFunctionDefinitionBase::get_notes() const
{
    return notes;
}

} // namespace language::plugins
