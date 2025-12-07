#include "language/plugins/plugin.h"

#include "downward/utils/strings.h"

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

InternalTypeDeclarationBase::InternalTypeDeclarationBase(
    type_index pointer_type,
    std::string type_identifier,
    std::string synopsis)
    : pointer_type(pointer_type)
    , identifier(std::move(type_identifier))
    , synopsis(std::move(synopsis))
{
}

type_index InternalTypeDeclarationBase::get_pointer_type() const
{
    return pointer_type;
}

string InternalTypeDeclarationBase::get_identifier() const
{
    return identifier;
}

string InternalTypeDeclarationBase::get_class_name() const
{
    return pointer_type.name();
}

string InternalTypeDeclarationBase::get_synopsis() const
{
    return synopsis;
}

DocumentationTopic::DocumentationTopic(const string& subcategory)
    : topic_name(subcategory)
{
}

void DocumentationTopic::document_title(const string& title)
{
    this->title = title;
}

void DocumentationTopic::document_synopsis(const string& synopsis)
{
    this->synopsis = synopsis;
}

void DocumentationTopic::add_type_declaration(
    const InternalTypeDeclarationBase& type_declaration)
{
    member_types.push_back(&type_declaration);
}

void DocumentationTopic::add_enum(
    const InternalEnumDeclarationBase& enum_declaration)
{
    enum_types.push_back(&enum_declaration);
}

void DocumentationTopic::add_function(
    const InternalFunctionDefinitionBase& function)
{
    features.push_back(&function);
}

string DocumentationTopic::get_topic_name() const
{
    return topic_name;
}

string DocumentationTopic::get_title() const
{
    return title;
}

string DocumentationTopic::get_synopsis() const
{
    return synopsis;
}

InternalEnumDeclarationBase::InternalEnumDeclarationBase(
    type_index type,
    initializer_list<pair<string, string>> enum_values)
    : type(type)
    , enum_info(enum_values)
{
}

type_index InternalEnumDeclarationBase::get_type() const
{
    return type;
}

string InternalEnumDeclarationBase::get_class_name() const
{
    return type.name();
}

const EnumInfo& InternalEnumDeclarationBase::get_enum_info() const
{
    return enum_info;
}
} // namespace downward::cli::plugins
