#include "language/documentation/topic.h"

using namespace std;

namespace language::documentation {

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

void DocumentationTopic::add_declaration(
    const parser::InternalFunctionDefinitionBase& declaration)
{
    declarations.push_back(&declaration);
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

} // namespace language::documentation
