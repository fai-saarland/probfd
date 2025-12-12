#include "language/plugins/doc_printer.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/utils/strings.h"

#include <iostream>
#include <map>
#include <print>
#include <stack>

using namespace std;

namespace language::plugins {
DocPrinter::DocPrinter(ostream& out, Registry& registry)
    : os(out)
    , registry(registry)
{
}

void DocPrinter::print_all() const
{
    const Namespace& n = registry.get_global_name_space();

    for (const auto& topic : registry.get_topics()) { print_topic(topic); }

    for (const auto type_declarations = n.get_type_declarations();
         const auto& type_declaration : type_declarations) {
        print_type_declaration(type_declaration);
    }
}

void DocPrinter::print_topic(const string& name) const
{
    const DocumentationTopic& s = registry.get_topic_by_name(name);
    print_topic(s);
}

void DocPrinter::print_type_declaration(const string& qname) const
{
    auto parts = std::views::split(qname, ".") |
                 std::ranges::to<std::vector<std::string>>();
    const auto name = std::move(parts.back());
    parts.pop_back();

    const Namespace& n = registry.get_namespace(parts);
    const auto categories = n.get_type_declarations();
    const auto it = std::ranges::lower_bound(
        categories,
        name,
        {},
        [](const auto& category_plugin) {
            return category_plugin.get_identifier();
        });

    if (it == categories.end() || it->get_identifier() != name) {
        throw MissingTypeDeclarationError(
            std::format(
                "could not find a type named '{}' in the registry",
                name));
    }

    print_type_declaration(*it);
}

void DocPrinter::print_function_declaration(const string& qname) const
{
    auto parts = std::views::split(qname, ".") |
                 std::ranges::to<std::vector<std::string>>();
    const auto name = std::move(parts.back());
    parts.pop_back();
    const auto& n = registry.get_namespace(parts);
    print_function_declaration(n.get_function_definition(name));
}

void DocPrinter::print_type_declaration(
    const InternalTypeDeclarationBase& type_declaration) const
{
    const auto& topics = registry.get_topics();

    std::vector sorted_topics(std::from_range, topics);
    std::ranges::sort(sorted_topics, {}, &DocumentationTopic::get_topic_name);

    print_type_declaration_header(type_declaration);
    print_type_declaration_synopsis(type_declaration.get_synopsis());

    vector<const InternalFunctionDefinitionBase*> function_definitions;

    std::stack<const Namespace*> s;
    s.push(&registry.get_global_name_space());

    while (!s.empty()) {
        const auto* n = s.top();
        s.pop();

        for (const auto& function_definition : n->get_function_definitions()) {
            if (const Type& type =
                    function_definition->get_type().get_return_type();
                type.is_internal_type() &&
                static_cast<const InternalType&>(type).get_type_index() ==
                    type_declaration.get_pointer_type()) {
                function_definitions.push_back(function_definition);
            }
        }

        for (const auto& [name, nested] : n->get_nested_namespaces()) {
            s.push(nested.get());
        }
    }

    std::vector sorted_function_definitions(
        std::from_range,
        function_definitions);

    std::ranges::sort(
        sorted_function_definitions,
        {},
        [](const InternalFunctionDefinitionBase* f) {
            return f->get_identifier();
        });

    print_type_declaration_related_functions(
        type_declaration,
        sorted_function_definitions);

    print_type_declaration_footer();
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

    std::vector functions(std::from_range, topic.get_functions());

    std::ranges::sort(
        functions,
        {},
        [](const InternalFunctionDefinitionBase* f) {
            return f->get_identifier();
        });

    print_topic_members(topic, functions);
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

Txt2TagsPrinter::Txt2TagsPrinter(ostream& out, Registry& registry)
    : DocPrinter(out, registry)
{
}

void Txt2TagsPrinter::print_header(
    const InternalFunctionDefinitionBase& function_def) const
{
    string title = function_def.get_title();
    if (title.empty()) { title = function_def.get_identifier(); }
    os << "== " << title << " ==" << endl;
}

void Txt2TagsPrinter::print_synopsis(
    const InternalFunctionDefinitionBase& function_def) const
{
    if (!function_def.get_synopsis().empty())
        os << function_def.get_synopsis() << endl;
}

void Txt2TagsPrinter::print_usage(
    const InternalFunctionDefinitionBase& function_def) const
{
    if (!function_def.get_identifier().empty()) {
        os << "``` " << function_def.get_identifier() << "(";
        vector<string> argument_help_strings;
        for (const ArgumentInfo& arg_info : function_def.get_arguments()) {
            string arg_help = arg_info.key;
            if (!arg_info.default_value.empty()) {
                arg_help += "=" + arg_info.default_value;
            }
            argument_help_strings.push_back(arg_help);
        }
        std::print(os, "{}", argument_help_strings);
        os << ")" << endl << endl << endl;
    }
}

void Txt2TagsPrinter::print_arguments(
    const InternalFunctionDefinitionBase& function_def) const
{
    const auto& ftype = function_def.get_type();

    for (const auto& [arg_type, arg_info, arg_help] : std::views::zip(
             ftype.get_argument_types(),
             function_def.get_arguments(),
             function_def.get_argument_docs())) {
        os << "- //" << arg_info.key << "// (" << arg_type->name()
           << "): " << arg_help << endl;
        if (arg_type->is_enum_type()) {
            for (const pair<string, string>& explanation :
                 static_cast<const EnumType&>(*arg_type)
                     .get_documented_enum_values()) {
                os << " - ``" << explanation.first
                   << "``: " << explanation.second << endl;
            }
        }
    }
}

void Txt2TagsPrinter::print_notes(
    const InternalFunctionDefinitionBase& function_def) const
{
    for (const NoteInfo& note : function_def.get_notes()) {
        if (note.long_text) {
            os << "=== " << note.name << " ===" << endl
               << note.description << endl
               << endl;
        } else {
            os << "**" << note.name << ":** " << note.description << endl
               << endl;
        }
    }
}

void Txt2TagsPrinter::print_language_features(
    const InternalFunctionDefinitionBase& function_def) const
{
    if (!function_def.get_language_support().empty()) {
        os << "Language features supported:" << endl;
        for (const LanguageSupportInfo& ls :
             function_def.get_language_support()) {
            os << "- **" << ls.feature << ":** " << ls.description << endl;
        }
    }
}

void Txt2TagsPrinter::print_properties(
    const InternalFunctionDefinitionBase& function_def) const
{
    if (!function_def.get_properties().empty()) {
        os << "Properties:" << endl;
        for (const PropertyInfo& prop : function_def.get_properties()) {
            os << "- **" << prop.property << ":** " << prop.description << endl;
        }
    }
}

void Txt2TagsPrinter::print_type_declaration_header(
    const InternalTypeDeclarationBase& type_declarations) const
{
    os << ">>>>CATEGORY: " << type_declarations.get_identifier() << "<<<<"
       << endl;
}

void Txt2TagsPrinter::print_type_declaration_synopsis(
    const string& synopsis) const
{
    if (!synopsis.empty()) { os << synopsis << endl; }
}

void Txt2TagsPrinter::print_type_declaration_footer() const
{
    os << endl << ">>>>CATEGORYEND<<<<" << endl;
}

PlainPrinter::PlainPrinter(ostream& out, Registry& registry, bool print_all)
    : DocPrinter(out, registry)
    , print_all(print_all)
{
}

void PlainPrinter::print_header(
    const InternalFunctionDefinitionBase& function_def) const
{
    string title = function_def.get_title();
    if (title.empty()) { title = function_def.get_identifier(); }
    os << "== " << title << " ==" << endl;
}

void PlainPrinter::print_synopsis(
    const InternalFunctionDefinitionBase& function_def) const
{
    if (print_all && !function_def.get_synopsis().empty()) {
        os << function_def.get_synopsis() << endl;
    }
}

void PlainPrinter::print_usage(
    const InternalFunctionDefinitionBase& function_def) const
{
    if (!function_def.get_identifier().empty()) {
        std::println(
            os,
            "{}({:n:d})",
            function_def.get_identifier(),
            function_def.get_arguments());
    }
}

void PlainPrinter::print_arguments(
    const InternalFunctionDefinitionBase& function_def) const
{
    const auto& ftype = function_def.get_type();

    for (const auto& [arg_type, arg_info, arg_help] : std::views::zip(
             ftype.get_argument_types(),
             function_def.get_arguments(),
             function_def.get_argument_docs())) {
        os << " " << arg_info.key << " (" << arg_type->name()
           << "): " << arg_help << endl;
        if (arg_type->is_enum_type()) {
            for (const pair<string, string>& explanation :
                 static_cast<const EnumType&>(*arg_type)
                     .get_documented_enum_values()) {
                os << " - " << explanation.first << ": " << explanation.second
                   << endl;
            }
        }
    }
}

void PlainPrinter::print_notes(
    const InternalFunctionDefinitionBase& function_def) const
{
    if (print_all) {
        for (const NoteInfo& note : function_def.get_notes()) {
            if (note.long_text) {
                os << "=== " << note.name << " ===" << endl
                   << note.description << endl
                   << endl;
            } else {
                os << " * " << note.name << ": " << note.description << endl
                   << endl;
            }
        }
    }
}

void PlainPrinter::print_language_features(
    const InternalFunctionDefinitionBase& function_def) const
{
    if (print_all && !function_def.get_language_support().empty()) {
        os << "Language features supported:" << endl;
        for (const LanguageSupportInfo& ls :
             function_def.get_language_support()) {
            os << " * " << ls.feature << ": " << ls.description << endl;
        }
    }
}

void PlainPrinter::print_properties(
    const InternalFunctionDefinitionBase& function_def) const
{
    if (print_all && !function_def.get_properties().empty()) {
        os << "Properties:" << endl;
        for (const PropertyInfo& prop : function_def.get_properties()) {
            os << " * " << prop.property << ": " << prop.description << endl;
        }
    }
}

void PlainPrinter::print_type_declaration_header(
    const InternalTypeDeclarationBase& type_declarations) const
{
    os << "Help for " << type_declarations.get_identifier() << endl << endl;
}

void PlainPrinter::print_type_declaration_synopsis(const string& synopsis) const
{
    if (!synopsis.empty()) { os << synopsis << endl; }
}

void PlainPrinter::print_type_declaration_footer() const
{
    os << endl;
}
} // namespace language::plugins
