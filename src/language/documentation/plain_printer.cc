#include "language/documentation/plain_printer.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_function_definition.h"
#include "language/ast/internal_type_declaration.h"
#include "language/utils/demangle.h"

#include <iostream>
#include <print>

using namespace std;

using namespace language::parser;

namespace language::documentation {

PlainPrinter::PlainPrinter(ostream& out, bool print_all)
    : DocPrinter(out)
    , print_all(print_all)
{
}

void PlainPrinter::print_header(
    const InternalFunctionDefinitionBase& function_def) const
{
    string title = function_def.get_title();
    if (title.empty()) { title = function_def.get_identifier(); }
    std::println(os, "== {} ==", title);
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
            function_def.get_argument_infos());
    }
}

void PlainPrinter::print_arguments(
    const InternalFunctionDefinitionBase& function_def) const
{
    const auto& arg_types = function_def.get_arg_types();

    for (const auto& [arg_type, arg_info, arg_help] : std::views::zip(
             arg_types,
             function_def.get_argument_infos(),
             function_def.get_argument_docs())) {
        os << " " << arg_info.key << " (" << utils::demangle(arg_type.name())
           << "): " << arg_help << endl;
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

void PlainPrinter::print_header(
    const InternalTypeDeclaration& type_declaration) const
{
    os << "Help for " << type_declaration.get_identifier() << endl << endl;
}

void PlainPrinter::print_synopsis(
    const InternalTypeDeclaration& type_declaration) const
{
    if (const auto& synopsis = type_declaration.get_synopsis();
        !synopsis.empty()) {
        os << synopsis << endl;
    }
}

void PlainPrinter::print_footer(const InternalTypeDeclaration&) const
{
    os << endl;
}

void PlainPrinter::print_header(
    const InternalEnumDeclarationBase& enum_declaration) const
{
    os << "Help for " << enum_declaration.get_identifier() << endl << endl;
}

void PlainPrinter::print_synopsis(
    const InternalEnumDeclarationBase& enum_declaration) const
{
    if (const auto& synopsis = enum_declaration.get_synopsis();
        !synopsis.empty()) {
        os << synopsis << endl;
    }
}

void PlainPrinter::print_enumerators(
    const InternalEnumDeclarationBase& enum_def) const
{
    for (const auto& [name, doc] : std::views::zip(
             enum_def.get_enumerator_names(),
             enum_def.get_enumerator_docs())) {
        os << " " << name << " : " << doc << endl;
    }
}

void PlainPrinter::print_footer(const InternalEnumDeclarationBase&) const
{
    os << endl;
}

} // namespace language::documentation
