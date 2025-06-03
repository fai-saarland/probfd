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

static constexpr int MAX_LINE_WIDTH = 180;

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
    void print_synopsis(const Feature& feature) const override
    {
        string title = feature.get_title();
        if (title.empty()) { title = feature.get_key(); }
        os << "===== " << title << " =====" << endl;
        if (print_all && !feature.get_synopsis().empty()) {
            os << feature.get_synopsis() << endl;
        }
    }

    void print_usage(const Feature& feature) const override
    {
        if (!feature.get_key().empty()) {
            os << "Feature identifier(s):\n  " << feature.get_key() << "\n"
               << std::endl;
        }

        os << "Type:\n  ";

        const auto& args = feature.get_arguments();

        auto it = args.begin();
        const auto end = args.end();

        if (it != end) {
            {
                const ArgumentInfo& arg_info = *it;
                os << arg_info.type.name();
            }

            for (++it; it != end; ++it) {
                const ArgumentInfo& arg_info = *it;
                if (arg_info.has_default()) { os << " ["; }
                os << " x " << arg_info.type.name();
                if (arg_info.has_default()) { os << " ]"; }
            }
        } else {
            os << "()";
        }

        os << " -> " << feature.get_type().name() << "\n" << std::endl;
    }

    void print_arguments(const Feature& feature) const override
    {
        if (feature.get_arguments().empty()) {
            os << "This feature has no arguments.\n" << std::endl;
            return;
        }

        os << "Keyword arguments:\n";

        int max_width = 0;

        std::vector<std::string> arg_strings;

        for (const ArgumentInfo& arg_info : feature.get_arguments()) {
            std::string* s;

            if (arg_info.bounds.has_bound()) {
                s = &arg_strings.emplace_back(
                    std::format("{} : {}", arg_info.key, arg_info.bounds));
            } else {
                s = &arg_strings.emplace_back(
                    std::format("{} : {}", arg_info.key, arg_info.type.name()));
            }

            if (const auto width = static_cast<int>(s->size());
                width > max_width)
                max_width = width;
        }

        int remaining_width = std::max(10, MAX_LINE_WIDTH - max_width + 4);

        for (const auto& [s, arg_info] :
             std::views::zip(arg_strings, feature.get_arguments())) {
            std::stringstream ss;
            ss << arg_info.help;

            if (!arg_info.help.empty() && arg_info.help.back() != '\n')
                ss << " ";

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

        std::println(os, "");
    }

    void print_category_synopsis(
        const string& synopsis) const override
    {
        if (print_all && !synopsis.empty()) { os << synopsis << endl; }
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

    if (!parser.present("features") && !parser.present("type")) {
        doc_printer->print_all();
    } else {
        if (auto categories =
                parser.present<std::vector<std::string>>("type")) {
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
        "Lists available feature variable pre-definitions that can be used "
        "in feature expressions. If no options are given, all "
        "pre-definitions are listed.");

    feature_list_parser.add_argument("--txt2tags")
        .help("List feature variable pre-definitions in txt2tags format.")
        .flag();
    feature_list_parser.add_argument("--type")
        .help(
            "One or more types for which all pre-definitions that belong "
            "to that type are to be listed.")
        .nargs(argparse::nargs_pattern::at_least_one);
    feature_list_parser.add_argument("--full", "-f")
        .help(
            "If specified, fulle detailed descriptions of all features of "
            "all specified types will be listed explicitly as well.")
        .flag();
    feature_list_parser.add_argument("features")
        .help(
            "Feature variable pre-definitions to list detailed "
            "descriptions for.")
        .remaining();
}

} // namespace probfd
