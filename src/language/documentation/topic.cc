#include "language/documentation/topic.h"

using namespace std;

namespace language::plugins {

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

} // namespace language::plugins
