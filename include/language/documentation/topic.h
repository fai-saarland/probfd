#ifndef LANGUAGE_DOCUMENTATION_TOPIC_H
#define LANGUAGE_DOCUMENTATION_TOPIC_H

#include <ranges>
#include <string>
#include <vector>

namespace language::parser {
class InternalFunctionDefinitionBase;
}

namespace language::documentation {

class DocumentationTopic {
    std::string topic_name;
    std::string title;
    std::string synopsis;

    std::vector<const parser::InternalFunctionDefinitionBase*> declarations;

public:
    explicit DocumentationTopic(const std::string& subcategory);

    void document_title(const std::string& title);
    void document_synopsis(const std::string& synopsis);

    void
    add_declaration(const parser::InternalFunctionDefinitionBase& declaration);

    auto get_declarations() const { return std::views::all(declarations); }

    std::string get_topic_name() const;
    std::string get_title() const;
    std::string get_synopsis() const;
};

} // namespace language::documentation

#endif
