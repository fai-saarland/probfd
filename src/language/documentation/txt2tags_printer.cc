#include "language/documentation/txt2tags_printer.h"

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

Txt2TagsPrinter::Txt2TagsPrinter(ostream& out)
    : DocPrinter(out)
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
        for (const ArgumentInfo& arg_info : function_def.get_argument_infos()) {
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
    for (const auto& [arg_type, arg_info, arg_help] : std::views::zip(
             function_def.get_arg_types(),
             function_def.get_argument_infos(),
             function_def.get_argument_docs())) {
        os << "- //" << arg_info.key << "// ("
           << utils::demangle(arg_type.name()) << "): " << arg_help << endl;
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

void Txt2TagsPrinter::print_header(
    const InternalTypeDeclaration& type_declarations) const
{
    os << ">>>>CATEGORY: " << type_declarations.get_identifier() << "<<<<"
       << endl;
}

void Txt2TagsPrinter::print_synopsis(
    const InternalTypeDeclaration& type_declaration) const
{
    if (const auto& synopsis = type_declaration.get_synopsis();
        !synopsis.empty()) {
        os << synopsis << endl;
    }
}

void Txt2TagsPrinter::print_footer(const InternalTypeDeclaration&) const
{
    os << endl << ">>>>CATEGORYEND<<<<" << endl;
}

void Txt2TagsPrinter::print_header(
    const InternalEnumDeclarationBase& enum_declaration) const
{
    os << ">>>>CATEGORY: " << enum_declaration.get_identifier() << "<<<<"
       << endl;
}

void Txt2TagsPrinter::print_synopsis(
    const InternalEnumDeclarationBase& enum_declaration) const
{
    if (const auto& synopsis = enum_declaration.get_synopsis();
        !synopsis.empty()) {
        os << synopsis << endl;
    }
}

void Txt2TagsPrinter::print_enumerators(
    const InternalEnumDeclarationBase& enum_declaration) const
{
    for (const auto& [arg_name, arg_help] : std::views::zip(
             enum_declaration.get_enumerator_names(),
             enum_declaration.get_enumerator_docs())) {
        os << "- //" << arg_name << "// : " << arg_help << endl;
    }
}

void Txt2TagsPrinter::print_footer(const InternalEnumDeclarationBase&) const
{
    os << endl << ">>>>CATEGORYEND<<<<" << endl;
}

} // namespace language::documentation
