#include "list_features.h"

#include "register_definitions.h"

#include "language/syntax_analyzer.h"

#include "language/plugins/doc_printer.h"
#include "language/plugins/registry.h"

#include "downward/utils/system.h"

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
using namespace language::plugins;

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

class FeaturePrinter : public DocPrinter {
    // If this is false, notes, properties and language_features are omitted.
    bool print_all;

public:
    FeaturePrinter(ostream& out, Registry& registry, bool print_all = false)
        : DocPrinter(out, registry)
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

        const auto& args_infos = function_def.get_arguments();
        const auto& arg_types = function_def.get_type(*TypeRegistry::instance())
                                    .get_argument_types();

        const auto args = std::views::zip(args_infos, arg_types);

        auto it = args.begin();
        const auto end = args.end();

        os << "(";

        bool has_optional = false;
        // bool has_kw = false;

        if (it != end) {
            {
                const auto& [arg_info, arg_type] = *it;
                os << arg_type->name();
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
                os << ", " << arg_type->name();
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
           << function_def.get_type(*TypeRegistry::instance())
                  .get_return_type()
                  .name()
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
        if (function_def.get_arguments().empty()) {
            os << "This function_def has no arguments.\n" << std::endl;
            return;
        }

        os << "Arguments in positional order:\n";

        int max_width = 0;

        std::vector<std::string> arg_strings;

        for (const auto& [arg_info, arg_type] : std::views::zip(
                 function_def.get_arguments(),
                 function_def.get_type(*TypeRegistry::instance())
                     .get_argument_types())) {
            std::string& s = arg_strings.emplace_back(
                std::format("{} : {}", arg_info, arg_type->name()));

            if (const auto width = static_cast<int>(s.size());
                width > max_width)
                max_width = width;
        }

        int remaining_width = std::max(10, MAX_LINE_WIDTH - max_width + 4);

        for (const auto& [s, arg_info, arg_type, arg_help] : std::views::zip(
                 arg_strings,
                 function_def.get_arguments(),
                 function_def.get_type(*TypeRegistry::instance())
                     .get_argument_types(),
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

    void print_language_features(
        const InternalFunctionDefinitionBase& function_def) const override
    {
        if (print_all && !function_def.get_language_support().empty()) {
            os << "Language features supported:" << endl;
            for (const LanguageSupportInfo& ls :
                 function_def.get_language_support()) {
                os << " * " << ls.feature << ": " << ls.description << endl;
            }
        }
    }

    void print_properties(
        const InternalFunctionDefinitionBase& function_def) const override
    {
        if (print_all && !function_def.get_properties().empty()) {
            os << "Properties:" << endl;
            for (const PropertyInfo& prop : function_def.get_properties()) {
                os << " * " << prop.property << ": " << prop.description
                   << endl;
            }
        }
    }

    void print_type_declaration_header(
        const InternalTypeDeclarationBase& type_declaration) const override
    {
        os << "##### Type " << type_declaration.get_identifier()
           << " #####\n\n";
    }

    void print_type_declaration_synopsis(const string& synopsis) const override
    {
        if (!synopsis.empty()) {
            std::println(os, "Description:\n{}\n", synopsis);
        } else {
            std::println(os, "No description.\n");
        }
    }

    void print_type_declaration_related_functions(
        const InternalTypeDeclarationBase&,
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
        const DocumentationTopic&,
        const std::vector<const InternalFunctionDefinitionBase*> features)
        const override
    {
        if (features.empty()) {
            os << "This topic has no members.\n" << std::endl;
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

    void print_type_declaration_footer() const override {}
};

} // namespace

static int list_features(argparse::ArgumentParser& parser)
{
    Registry registry;
    register_definitions(registry);

    unique_ptr<DocPrinter> doc_printer;
    if (parser.get<bool>("--txt2tags")) {
        doc_printer = std::make_unique<Txt2TagsPrinter>(cout, registry);
    } else {
        doc_printer = std::make_unique<FeaturePrinter>(cout, registry);
    }

    int exitcode = 0;

    if (parser.present("--types")) {
        const auto types = parser.get<std::vector<std::string>>("--types");
        for (const string& name : types) {
            try {
                doc_printer->print_type_declaration(name);
            } catch (const MissingTypeDeclarationError&) {
                std::println(std::cerr, "Type '{}' does not exist.", name);
                exitcode = 1;
            }
        }
    }

    if (parser.present("--features")) {
        const auto features =
            parser.get<std::vector<std::string>>("--functions");
        for (const string& name : features) {
            try {
                doc_printer->print_function_declaration(name);
            } catch (const MissingFunctionError&) {
                std::println(std::cerr, "Function '{}' does not exist.", name);
                exitcode = 1;
            }
        }
    }

    if (parser.present("--topics")) {
        const auto topics = parser.get<std::vector<std::string>>("--topics");
        for (const string& name : topics) {
            try {
                doc_printer->print_topic(name);
            } catch (const MissingTopicError&) {
                std::println(std::cerr, "Topic '{}' does not exist.", name);
                exitcode = 1;
            }
        }
    }

    return exitcode;
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
    feature_list_parser.add_argument("--features")
        .help(
            "One or more identifiers of pre-defined variables that should be "
            "listed.")
        .nargs(argparse::nargs_pattern::at_least_one);
    feature_list_parser.add_argument("--types")
        .help(
            "One or more identifiers of pre-defined types that should be "
            "listed.")
        .nargs(argparse::nargs_pattern::at_least_one);
    feature_list_parser.add_argument("--topics")
        .help(
            "One or more identifiers of documentation groups that should be "
            "listed.")
        .nargs(argparse::nargs_pattern::at_least_one);
}

} // namespace probfd