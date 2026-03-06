#include "language/documentation/doc_printer.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_function_definition.h"
#include "language/ast/internal_type_declaration.h"
#include "language/ast/namespace_declaration.h"
#include "language/ast/namespace_level_declaration_visitor.h"

#include <iostream>
#include <map>
#include <print>
#include <regex>
#include <stack>

using namespace std;

using namespace language::parser;

namespace language::documentation {

namespace {

class DocumentedEntities : public NamespaceLevelDeclarationVisitor {
    struct TopicComparator {
        bool operator()(
            const DocumentationTopic* left,
            const DocumentationTopic* right) const
        {
            return left->get_topic_name() < right->get_topic_name();
        }
    };

    struct DeclarationComparator {
        bool operator()(const Declaration* left, const Declaration* right) const
        {
            return left->get_identifier() < right->get_identifier();
        }
    };

    std::set<const DocumentationTopic*, TopicComparator> topics;

    std::set<const InternalTypeDeclaration*, DeclarationComparator> types;
    std::set<const InternalEnumDeclarationBase*, DeclarationComparator> enums;
    std::set<const InternalFunctionDefinitionBase*, DeclarationComparator>
        functions;

    std::regex topic_regex;
    std::regex type_regex;
    std::regex enum_regex;
    std::regex function_regex;

public:
    explicit DocumentedEntities(
        std::string topic_regex,
        std::string type_regex,
        std::string enum_regex,
        std::string function_regex)
        : topic_regex(std::move(topic_regex))
        , type_regex(std::move(type_regex))
        , enum_regex(std::move(enum_regex))
        , function_regex(std::move(function_regex))
    {
    }

    void accept(const DocumentationTopic& topic)
    {
        if (std::regex_match(topic.get_topic_name(), topic_regex)) {
            topics.insert(&topic);
        }
    }

    void accept(const InternalEnumDeclarationBase& declaration) override
    {
        if (std::regex_match(declaration.get_identifier(), enum_regex)) {
            enums.insert(&declaration);
        }
    }

    void accept(const InternalFunctionDefinitionBase& declaration) override
    {
        if (std::regex_match(declaration.get_identifier(), function_regex)) {
            functions.insert(&declaration);
        }
    }

    void accept(const InternalTypeDeclaration& declaration) override
    {
        if (std::regex_match(declaration.get_identifier(), type_regex)) {
            types.insert(&declaration);
        }
    }

    void accept(const NamespaceDeclaration& declaration) override
    {
        for (const auto& decl : declaration.get_declarations()) {
            decl->visit(*this);
        }
    }

    auto get_sorted_topics() const { return topics | std::views::all; }

    auto get_sorted_types() const { return types | std::views::all; }

    auto get_sorted_enums() const { return enums | std::views::all; }

    auto get_sorted_functions() const { return functions | std::views::all; }
};

} // namespace

DocPrinter::DocPrinter(ostream& out)
    : os(out)
{
}

void DocPrinter::print_documentation(
    const CompilationContext& ccontext,
    std::string topic_regex,
    std::string type_regex,
    std::string enum_regex,
    std::string function_regex) const
{
    DocumentedEntities entities(
        topic_regex,
        type_regex,
        enum_regex,
        function_regex);

    for (auto& topic : ccontext.get_topics()) { entities.accept(topic); }

    for (auto& decl : ccontext.get_declarations()) { decl->visit(entities); }

    for (auto& topic : entities.get_sorted_topics()) { print_topic(*topic); }

    for (auto& type : entities.get_sorted_types()) {
        print_type_declaration(*type);
    }

    for (auto& enum_decl : entities.get_sorted_enums()) {
        print_enum_declaration(*enum_decl);
    }

    for (auto& func : entities.get_sorted_functions()) {
        print_function_declaration(*func);
    }
}

void DocPrinter::print_type_declaration(
    const InternalTypeDeclaration& type_declaration) const
{
    print_header(type_declaration);
    print_synopsis(type_declaration);

    // print_type_declaration_related_functions(
    //     type_declaration,
    //     sorted_function_definitions);

    print_footer(type_declaration);
}

void DocPrinter::print_enum_declaration(
    const InternalEnumDeclarationBase& enum_declaration) const
{
    print_header(enum_declaration);
    print_synopsis(enum_declaration);
    print_enumerators(enum_declaration);
    print_footer(enum_declaration);
}

void DocPrinter::print_topic(const DocumentationTopic& topic) const
{
    if (const auto& topic_name = topic.get_topic_name(); !topic_name.empty()) {
        os << endl << "===== Topic " << topic.get_title() << " =====" << endl;
        if (!topic.get_synopsis().empty()) {
            os << topic.get_synopsis() << endl;
        }
        os << endl;
    }

    std::vector decls(std::from_range, topic.get_declarations());

    std::ranges::sort(decls, {}, [](const NamespaceLevelDeclaration* d) {
        return d->get_identifier();
    });

    print_topic_members(topic, decls);
}

void DocPrinter::print_function_declaration(
    const InternalFunctionDefinitionBase& function_def) const
{
    print_header(function_def);
    print_synopsis(function_def);
    print_usage(function_def);
    print_arguments(function_def);
    print_notes(function_def);
    print_language_features(function_def);
    print_properties(function_def);
}

} // namespace language::documentation
