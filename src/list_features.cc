#include "list_features.h"

#include "register_definitions.h"

#include "language/syntax_analyzer.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"
#include "language/ast/internal_type_declaration.h"

#include "language/documentation/doc_printer.h"
#include "language/documentation/txt2tags_printer.h"

#include "language/utils/demangle.h"

#include "downward/utils/system.h"
#include "language/ast/internal_enum_declaration.h"

#include <cassert>
#include <charconv>
#include <filesystem>
#include <iostream>
#include <print>
#include <ranges>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace language::parser;

using downward::utils::ExitCode;

static constexpr int MAX_LINE_WIDTH = 180;

namespace probfd {

namespace {

std::vector<std::string_view> split_lines(const std::string& s, int line_wrap)
{
    std::vector<std::string_view> lines;

    // split at newline delimiter and wrap to not exceed max line width
    auto prev = std::ranges::find(s, '\n');

    auto it = s.begin();

    while (std::distance(it, prev) > line_wrap) {
        auto n = std::next(it, line_wrap);
        auto [l, end] = std::ranges::find_last(it, n, ' ');

        if (l == end) {
            auto ll = std::ranges::find(n, prev, ' ');

            if (ll == end) { break; }

            lines.emplace_back(it, ll);
            it = ll;
            continue;
        }

        lines.emplace_back(it, l);
        it = std::next(l);
    }

    lines.emplace_back(it, prev);

    for (decltype(prev) pos; prev != s.end(); prev = pos) {
        pos = std::find(++prev, s.end(), '\n');

        while (std::distance(prev, pos) > line_wrap) {
            auto n = std::next(it, line_wrap);
            auto [l, end] = std::ranges::find_last(prev, n, ' ');

            if (l == end) {
                auto ll = std::ranges::find(n, pos, ' ');

                if (ll == end) { break; }

                lines.emplace_back(prev, ll);
                prev = ll;
                continue;
            }

            lines.emplace_back(prev, l);
            prev = std::next(l);
        }

        lines.emplace_back(prev, pos);
    }

    return lines;
}

class FeaturePrinter : public language::documentation::DocPrinter {
    // If this is false, notes, properties and language_features are omitted.
    bool print_all;

public:
    explicit FeaturePrinter(ostream& out, bool print_all = false)
        : DocPrinter(out)
        , print_all(print_all)
    {
    }

protected:
    void print_header(
        const InternalFunctionDefinitionBase& function_def) const override
    {
        assert(!function_def.get_identifier().empty());
        std::println(
            os,
            "===== Variable {} =====",
            function_def.get_identifier());
    }

    void print_synopsis(
        const InternalFunctionDefinitionBase& function_def) const override
    {
        if (const auto& syn = function_def.get_synopsis(); !syn.empty()) {
            std::println(os, "Description:\n{}\n", syn);
        } else if (const auto& title = function_def.get_title();
                   !title.empty()) {
            std::println(os, "Description:\n{}\n", title);
        }
    }

    void print_usage(
        const InternalFunctionDefinitionBase& function_def) const override
    {
        os << "Type:\n  ";

        const auto& args_infos = function_def.get_argument_infos();
        const auto& arg_types = function_def.get_arg_types();

        const auto args = std::views::zip(args_infos, arg_types);

        auto it = args.begin();
        const auto end = args.end();

        os << "(";

        bool has_optional = false;
        // bool has_kw = false;

        if (it != end) {
            {
                const auto& [arg_info, arg_type] = *it;
                os << language::utils::demangle(arg_type.name());
                if (arg_info.has_default()) {
                    os << "*";
                    has_optional = true;
                }

                /*if (const auto& key = arg_info.key; !key.empty()) {
                    os << " [\"" << key << "\"]";
                    has_kw = true;
                }*/
            }

            for (++it; it != end; ++it) {
                const auto& [arg_info, arg_type] = *it;
                os << ", " << language::utils::demangle(arg_type.name());
                if (arg_info.has_default()) {
                    os << "*";
                    has_optional = true;
                }

                /*if (const auto& key = arg_info.key; !key.empty()) {
                    os << " [\"" << key << "\"]";
                    has_kw = true;
                }*/
            }
        }

        os << ") -> "
           << language::utils::demangle(function_def.get_return_type().name())
           << std::endl;

        if (has_optional) {
            os << "  The symbol '*' indicates an optional argument."
               << std::endl;
        }

        /*if (has_kw) {
            os << "  Argument names of keyword arguments are specified in "
                  "brackets following the argument."
               << std::endl;
        }*/

        os << endl;
    }

    void print_arguments(
        const InternalFunctionDefinitionBase& function_def) const override
    {
        if (function_def.get_argument_infos().empty()) {
            os << "This function_def has no arguments.\n" << std::endl;
            return;
        }

        os << "Arguments in positional order:\n";

        int max_width = 0;

        std::vector<std::string> arg_strings;

        for (const auto& [arg_info, arg_type] : std::views::zip(
                 function_def.get_argument_infos(),
                 function_def.get_arg_types())) {
            std::string& s = arg_strings.emplace_back(
                std::format(
                    "{} : {}",
                    arg_info,
                    language::utils::demangle(arg_type.name())));

            if (const auto width = static_cast<int>(s.size());
                width > max_width)
                max_width = width;
        }

        int remaining_width = std::max(10, MAX_LINE_WIDTH - max_width + 4);

        for (const auto& [s, arg_info, arg_help] : std::views::zip(
                 arg_strings,
                 function_def.get_argument_infos(),
                 function_def.get_argument_docs())) {
            std::stringstream ss;
            ss << arg_help;

            if (!arg_help.empty() && arg_help.back() != '\n') ss << " ";

            if (arg_info.has_default()) {
                std::print(ss, "[default: {}]", arg_info.default_value);
            } else {
                std::print(ss, "[required]");
            }

            std::string temps = ss.str();
            std::vector<std::string_view> lines =
                split_lines(temps, remaining_width);

            // Print help line by line
            std::println(os, "  {:{}}  {}", s, max_width, lines.front());

            for (std::string_view line : lines | std::views::drop(1)) {
                std::println(os, "{:>{}}{}", "", max_width + 4, line);
            }
        }

        os << std::endl;
    }

    void print_notes(
        const InternalFunctionDefinitionBase& function_def) const override
    {
        if (print_all) {
            for (const auto& note : function_def.get_notes()) {
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

    void print_language_features(
        const InternalFunctionDefinitionBase& function_def) const override
    {
        if (print_all && !function_def.get_language_support().empty()) {
            os << "Language features supported:" << endl;
            for (const auto& ls : function_def.get_language_support()) {
                os << " * " << ls.feature << ": " << ls.description << endl;
            }
        }
    }

    void print_properties(
        const InternalFunctionDefinitionBase& function_def) const override
    {
        if (print_all && !function_def.get_properties().empty()) {
            os << "Properties:" << endl;
            for (const auto& prop : function_def.get_properties()) {
                os << " * " << prop.property << ": " << prop.description
                   << endl;
            }
        }
    }

    void
    print_header(const InternalTypeDeclaration& type_declaration) const override
    {
        os << "##### Type " << type_declaration.get_identifier()
           << " #####\n\n";
    }

    void print_synopsis(
        const InternalTypeDeclaration& type_declaration) const override
    {
        if (const auto& synopsis = type_declaration.get_synopsis();
            !synopsis.empty()) {
            std::println(os, "Description:\n{}\n", synopsis);
        } else {
            std::println(os, "No description.\n");
        }
    }

    void print_type_declaration_related_functions(
        const InternalTypeDeclaration&,
        const std::vector<const InternalFunctionDefinitionBase*> features)
        const override
    {
        if (features.empty()) {
            os << "This type has no members.\n" << std::endl;
            return;
        }

        os << "Members:\n";

        std::size_t max_width = 0;

        for (const auto& function_def : features) {
            if (const auto& name = function_def->get_identifier();
                name.size() > max_width)
                max_width = name.size();
        }

        for (const auto& function_def : features) {
            const auto& name = function_def->get_identifier();
            const auto& synopsis = function_def->get_title();
            std::print(os, "  {:{}}  {}\n", name, max_width, synopsis);
        }

        std::println(os);
    }

    void print_topic_members(
        const language::documentation::DocumentationTopic&,
        const std::vector<const InternalFunctionDefinitionBase*> decls)
        const override
    {
        if (decls.empty()) {
            os << "This topic has no members.\n" << std::endl;
            return;
        }

        os << "Members:\n";

        std::size_t max_width = 0;

        for (const auto& function_def : decls) {
            if (const auto& name = function_def->get_identifier();
                name.size() > max_width)
                max_width = name.size();
        }

        for (const auto& function_def : decls) {
            const auto& name = function_def->get_identifier();
            const auto& synopsis = function_def->get_title();
            std::print(os, "  {:{}}  {}\n", name, max_width, synopsis);
        }

        std::println(os);
    }

    void print_footer(const InternalTypeDeclaration&) const override {}

    void print_header(
        const InternalEnumDeclarationBase& enum_declaration) const override
    {
        os << "##### Enumeration " << enum_declaration.get_identifier()
           << " #####\n\n";
    }

    void print_synopsis(
        const InternalEnumDeclarationBase& enum_declaration) const override
    {
        if (const auto& synopsis = enum_declaration.get_synopsis();
            !synopsis.empty()) {
            std::println(os, "Description:\n{}\n", synopsis);
        } else {
            std::println(os, "No description.\n");
        }
    }

    void print_enumerators(
        const InternalEnumDeclarationBase& enum_def) const override
    {
        if (enum_def.get_enumerator_names().empty()) {
            os << "This enumeration has no enumerators.\n" << std::endl;
            return;
        }

        os << "Enumerators:\n";

        int max_width = 0;

        std::vector<std::string> arg_strings;

        for (const auto& name : enum_def.get_enumerator_names()) {
            if (const auto width = static_cast<int>(name.size());
                width > max_width)
                max_width = width;
        }

        const int remaining_width =
            std::max(10, MAX_LINE_WIDTH - max_width + 4);

        for (const auto& [name, help] : std::views::zip(
                 enum_def.get_enumerator_names(),
                 enum_def.get_enumerator_docs())) {
            std::stringstream ss;
            ss << help;

            if (!help.empty() && help.back() != '\n') ss << " ";

            std::string temps = ss.str();
            std::vector<std::string_view> lines =
                split_lines(temps, remaining_width);

            // Print help line by line
            std::println(os, "  {:{}}  {}", name, max_width, lines.front());

            for (std::string_view line : lines | std::views::drop(1)) {
                std::println(os, "{:>{}}{}", "", max_width + 4, line);
            }
        }

        os << std::endl;
    }

    void print_footer(const InternalEnumDeclarationBase&) const override {}
};

} // namespace

static int list_features(argparse::ArgumentParser& parser)
{
    CompilationContext registry;
    register_definitions(registry);

    unique_ptr<language::documentation::DocPrinter> doc_printer;
    if (parser.get<bool>("--txt2tags")) {
        doc_printer =
            std::make_unique<language::documentation::Txt2TagsPrinter>(cout);
    } else {
        doc_printer = std::make_unique<FeaturePrinter>(cout);
    }

    std::string topic_regex = "$^";
    std::string type_regex = "$^";
    std::string enum_regex = "$^";
    std::string function_regex = "$^";

    if (parser.present("--types")) {
        type_regex = parser.get<std::string>("--types");
    }

    if (parser.present("--functions")) {
        function_regex = parser.get<std::string>("--functions");
    }

    if (parser.present("--topics")) {
        topic_regex = parser.get<std::string>("--topics");
    }

    if (parser.present("--enums")) {
        enum_regex = parser.get<std::string>("--enums");
    }

    doc_printer->print_documentation(
        registry,
        topic_regex,
        type_regex,
        enum_regex,
        function_regex);

    return 0;
}

void add_list_features_subcommand(argparse::ArgumentParser& arg_parser)
{
    argparse::ArgumentParser& feature_list_parser =
        arg_parser.emplace_subparser(
            std::forward_as_tuple(
                "list-features",
                "0.0",
                argparse::default_arguments::help),
            list_features);

    feature_list_parser.add_description(
        "Lists pre-defined variables and types of the expression language used "
        "to specify the planning task solver."
        "If no options are given, information for the root expression type "
        "expected by the search command will be listed.");

    feature_list_parser.add_argument("--txt2tags")
        .help("Emit output in txt2tags format.")
        .flag();
    feature_list_parser.add_argument("--functions")
        .help(
            "One or more identifiers of pre-defined functions that should be "
            "listed.")
        .nargs(argparse::nargs_pattern::at_least_one);
    feature_list_parser.add_argument("--types")
        .help(
            "One or more identifiers of pre-defined types that should be "
            "listed.")
        .nargs(argparse::nargs_pattern::at_least_one);
    feature_list_parser.add_argument("--enums")
        .help(
            "One or more identifiers of pre-defined enums that should be "
            "listed.")
        .nargs(argparse::nargs_pattern::at_least_one);
    feature_list_parser.add_argument("--topics")
        .help(
            "One or more identifiers of documentation groups that should be "
            "listed.")
        .nargs(argparse::nargs_pattern::at_least_one);
}

} // namespace probfd