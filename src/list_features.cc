#include "list_features.h"

#include "downward/cli/parser/syntax_analyzer.h"

#include "downward/cli/plugins/doc_printer.h"
#include "downward/cli/plugins/raw_registry.h"
#include "downward/cli/plugins/registry.h"

#include "downward/utils/system.h"

#include <charconv>
#include <filesystem>
#include <iostream>
#include <print>
#include <ranges>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace downward::cli::parser;
using namespace downward::cli::plugins;

using downward::utils::ExitCode;

template <>
struct std::formatter<downward::cli::plugins::Bounds> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto
    format(const downward::cli::plugins::Bounds& bounds, FormatContext& ctx)
        const
    {
        return std::format_to(ctx.out(), "[{}, {}]", bounds.min, bounds.max);
    }
};

namespace probfd {

namespace {

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
    void print_synopsis(const Feature& feature) const override
    {
        string title = feature.get_title();
        if (title.empty()) {
            title = feature.get_key();
        }
        os << "===== " << title << " =====" << endl;
        if (print_all && !feature.get_synopsis().empty()) {
            os << feature.get_synopsis() << endl;
        }
    }

    void print_usage(const Feature& feature) const override
    {
        if (!feature.get_key().empty()) {
            os << "Feature key(s):\n  " << feature.get_key() << "\n"
               << std::endl;
        }
    }

    void print_arguments(const Feature& feature) const override
    {
        if (feature.get_arguments().empty()) {
            os << "This feature has no arguments.\n" << std::endl;
            return;
        }

        os << "Arguments:\n";

        std::size_t max_width = 0;

        std::vector<std::string> arg_strings;

        for (const ArgumentInfo& arg_info : feature.get_arguments()) {
            std::string* s;
            auto tname = arg_info.type.is_basic_type() &&
                                 arg_info.type.get_basic_type_index() ==
                                     typeid(std::string)
                             ? "string"
                             : arg_info.type.name();

            if (arg_info.bounds.has_bound()) {
                s = &arg_strings.emplace_back(
                    std::format(
                        "{} ({} {})",
                        arg_info.key,
                        tname,
                        arg_info.bounds));
            } else {
                s = &arg_strings.emplace_back(
                    std::format("{} ({})", arg_info.key, tname));
            }

            auto width = s->size();

            if (width > max_width) max_width = width;
        }

        for (const auto& [arg_info, s] :
             std::views::zip(feature.get_arguments(), arg_strings)) {

            std::print(os, "  {:{}}  {}", s, max_width, arg_info.help);

            if (arg_info.has_default()) {
                std::println(os, " (default: {})", arg_info.default_value);
            } else {
                std::println(os);
            }

            const Type& arg_type = arg_info.type;
            if (!arg_type.is_enum_type()) continue;

            for (const auto& expl : arg_type.get_documented_enum_values()) {
                std::println(
                    os,
                    "{:>{}} {}: {}",
                    '-',
                    max_width + 7,
                    expl.first,
                    expl.second);
            }
        }

        os << std::endl;
    }

    void print_notes(const Feature& feature) const override
    {
        if (print_all) {
            for (const NoteInfo& note : feature.get_notes()) {
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

    void print_language_features(const Feature& feature) const override
    {
        if (print_all && !feature.get_language_support().empty()) {
            os << "Language features supported:" << endl;
            for (const LanguageSupportInfo& ls :
                 feature.get_language_support()) {
                os << " * " << ls.feature << ": " << ls.description << endl;
            }
        }
    }

    void print_properties(const Feature& feature) const override
    {
        if (print_all && !feature.get_properties().empty()) {
            os << "Properties:" << endl;
            for (const PropertyInfo& prop : feature.get_properties()) {
                os << " * " << prop.property << ": " << prop.description
                   << endl;
            }
        }
    }

    void print_category_header(
        const FeatureType& type,
        const std::map<std::string, std::vector<const Feature*>>& subcategories)
        const override
    {
        os << "##### Category " << type.name() << " #####\n\n";

        if (subcategories.empty()) {
            os << "This category has no features.\n" << std::endl;
            return;
        }

        os << "Features:\n";

        std::size_t max_width = 0;

        for (const auto& [subcategory, features] : subcategories) {
            for (const auto& feature : features) {
                const auto& name = feature->get_key();
                if (name.size() > max_width) max_width = name.size();
            }
        }

        for (const auto& [subcategory, features] : subcategories) {
            for (const auto& feature : features) {
                const auto& name = feature->get_key();
                const auto& synopsis = feature->get_title();
                std::print(os, "  {:{}}  {}\n", name, max_width, synopsis);
            }
        }

        std::println(os);
    }

    void print_category_synopsis(
        const string& synopsis,
        bool supports_variable_binding) const override
    {
        if (print_all && !synopsis.empty()) {
            os << synopsis << endl;
        }
        if (supports_variable_binding) {
            os << endl
               << "This feature type can be bound to variables using "
               << "``let(variable_name, variable_definition, expression)"
               << "`` where ``expression`` can use ``variable_name``. "
               << "Predefinitions using ``--evaluator``, ``--heuristic``, and "
               << "``--landmarks`` are automatically transformed into ``let``-"
               << "expressions but are deprecated." << endl;
        }
    }

    void print_category_footer() const override {}
};

} // namespace

static int list_features(argparse::ArgumentParser& parser)
{
    Registry registry = RawRegistry::instance()->construct_registry();
    unique_ptr<DocPrinter> doc_printer;
    if (parser.get<bool>("--txt2tags")) {
        doc_printer = std::make_unique<Txt2TagsPrinter>(cout, registry);
    } else {
        doc_printer = std::make_unique<FeaturePrinter>(cout, registry);
    }

    if (!parser.present("features") && !parser.present("category")) {
        doc_printer->print_all();
    } else {
        if (auto categories =
                parser.present<std::vector<std::string>>("category")) {
            try {
                for (const string& name : *categories) {
                    doc_printer->print_category(name, parser.get<bool>("full"));
                }
            } catch (const MissingSubCategoryError& error) {
                std::cerr << error.what() << std::endl;
                return 1;
            }
        }

        if (auto features =
                parser.present<std::vector<std::string>>("features")) {
            for (const string& name : *features) {
                try {
                    doc_printer->print_feature(name);
                } catch (const MissingFeatureError& error) {
                    std::cerr << error.what() << std::endl;
                    return 1;
                }
            }
        }
    }

    return 0;
}

void add_list_features_subcommand(argparse::ArgumentParser& arg_parser)
{
    argparse::ArgumentParser& feature_list_parser =
        arg_parser.emplace_subparser(
            std::forward_as_tuple(
                "list-features",
                "",
                argparse::default_arguments::help),
            list_features);
    feature_list_parser.add_description(
        "Lists available search features. If no options are given, all "
        "features are listed.");

    feature_list_parser.add_argument("--txt2tags")
        .help("List features in txt2tags format.")
        .flag();
    feature_list_parser.add_argument("--category")
        .help("One or more categories which should be listed.")
        .nargs(argparse::nargs_pattern::at_least_one);
    feature_list_parser.add_argument("--full", "-f")
        .help(
            "If specified, all features of specified categories will be "
            "listed explicitly as well.")
        .flag();
    feature_list_parser.add_argument("features")
        .help("Individual features to list.")
        .remaining();
}

} // namespace probfd