#ifndef LANGUAGE_PLUGINS_TOPIC_H
#define LANGUAGE_PLUGINS_TOPIC_H

#include "language/plugins/registry_types.h"

#include <ranges>
#include <string>
#include <vector>

namespace language::plugins {

class DocumentationTopic {
    std::string topic_name;
    std::string title;
    std::string synopsis;

    std::vector<const InternalTypeDeclarationBase*> member_types;
    std::vector<const InternalEnumDeclarationBase*> enum_types;
    std::vector<const InternalFunctionDefinitionBase*> features;

public:
    explicit DocumentationTopic(const std::string& subcategory);

    void document_title(const std::string& title);
    void document_synopsis(const std::string& synopsis);

    void
    add_type_declaration(const InternalTypeDeclarationBase& type_declaration);
    void add_enum(const InternalEnumDeclarationBase& enum_declaration);
    void add_function(const InternalFunctionDefinitionBase& function);

    auto get_types() { return std::views::all(member_types); }

    auto get_types() const { return std::views::all(member_types); }

    auto get_enums() { return std::views::all(enum_types); }

    auto get_enums() const { return std::views::all(enum_types); }

    auto get_functions() { return std::views::all(features); }

    auto get_functions() const { return std::views::all(features); }

    std::string get_topic_name() const;
    std::string get_title() const;
    std::string get_synopsis() const;
};

}

#endif
