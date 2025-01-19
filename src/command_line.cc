#include "command_line.h"

#include "downward/cli/parser/lexical_analyzer.h"
#include "downward/cli/parser/syntax_analyzer.h"
#include "downward/cli/parser/token_stream.h"

#include "downward/cli/plugins/doc_printer.h"
#include "downward/cli/plugins/raw_registry.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/tasks/root_task.h"

#include "probfd/solver_interface.h"
#include "probfd/value_type.h"

#include "downward/utils/logging.h"
#include "downward/utils/strings.h"
#include "downward/utils/system.h"

#include <any>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <stdexcept>
#include <vector>

#include <argparse/argparse.hpp>

using namespace std;
using namespace downward::cli::parser;
using namespace downward::cli::plugins;

using utils::ExitCode;

template <>
struct std::formatter<downward::cli::plugins::Bounds> {
    std::formatter<std::pair<std::string, std::string>> inheritted;

    constexpr formatter() { inheritted.set_brackets("[", "]"); }

    constexpr auto parse(std::format_parse_context& ctx)
    {
        return inheritted.parse(ctx);
    }

    auto format(
        const downward::cli::plugins::Bounds& bounds,
        std::format_context& ctx) const
    {
        return inheritted.format(std::make_pair(bounds.min, bounds.max), ctx);
    }
};

namespace probfd {

static string replace_old_style_predefinitions(
    const std::string& old_search_argument,
    const std::vector<std::string>& predefinitions)
{
    ostringstream new_search_argument;

    for (const auto& predefinition_kv : predefinitions) {
        vector<string> predefinition = utils::split(predefinition_kv, "=", 1);
        if (predefinition.size() < 2)
            throw std::invalid_argument(
                "predefinition expects format 'key=definition'");
        string key = predefinition[0];
        string definition = predefinition[1];
        if (!utils::is_alpha_numeric(key))
            throw std::invalid_argument(
                "predefinition key has to be alphanumeric: '" + key + "'");
        new_search_argument << "let(" << key << "," << definition << ",";
    }

    new_search_argument << old_search_argument
                        << string(predefinitions.size(), ')');

    return new_search_argument.str();
}

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
                s = &arg_strings.emplace_back(std::format(
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

            auto fmt = std::format("  {{:{}}}  {{}}", max_width);
            std::vprint_unicode(
                os,
                fmt,
                std::make_format_args(s, arg_info.help));

            if (arg_info.has_default()) {
                std::println(os, " (default: {})", arg_info.default_value);
            } else {
                std::println(os);
            }

            const Type& arg_type = arg_info.type;
            if (arg_type.is_enum_type()) {
                for (const pair<string, string>& explanation :
                     arg_type.get_documented_enum_values()) {
                    os << std::string(max_width + 2, ' ') << "    - "
                       << explanation.first << ": " << explanation.second
                       << endl;
                }
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

        auto fmt = std::format("  {{:<{}}}  {{}}\n", max_width);

        for (const auto& [subcategory, features] : subcategories) {
            for (const auto& feature : features) {
                const auto& name = feature->get_key();
                const auto& synopsis = feature->get_title();
                std::vprint_unicode(
                    os,
                    fmt,
                    std::make_format_args(name, synopsis));
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
            for (const string& name : *categories) {
                doc_printer->print_category(
                    name,
                    parser.get<bool>("full"));
            }
        }

        if (auto features =
                parser.present<std::vector<std::string>>("features")) {
            for (const string& name : *features) {
                doc_printer->print_feature(name);
            }
        }
    }

    return 0;
}

static int search(argparse::ArgumentParser& parser)
{
    if (auto eps = parser.present<float>("--epsilon")) {
        probfd::g_epsilon = *eps;
    }

    std::string search_arg = parser.get("algorithm");

    auto predefinitions = parser.get<std::vector<std::string>>("predefinition");

    if (!predefinitions.empty()) {
        try {
            search_arg =
                replace_old_style_predefinitions(search_arg, predefinitions);
        } catch (const std::invalid_argument& err) {
            std::cerr << err.what() << std::endl;
            return static_cast<int>(utils::ExitCode::SEARCH_INPUT_ERROR);
        }

        std::cout << "Using translated search string: " << search_arg
                  << std::endl;
    }

    utils::register_event_handlers();

    shared_ptr<TaskSolverFactory> solver_factory;

    try {
        TokenStream tokens = split_tokens(search_arg);
        ASTNodePtr parsed = parse(tokens);
        DecoratedASTNodePtr decorated = parsed->decorate();
        std::any constructed = decorated->construct();
        solver_factory =
            std::any_cast<shared_ptr<TaskSolverFactory>>(constructed);
    } catch (const utils::ContextError& e) {
        std::cerr << e.get_message() << std::endl;
        return static_cast<int>(utils::ExitCode::SEARCH_INPUT_ERROR);
    }

    std::shared_ptr<ProbabilisticTask> input_task;

    {
        utils::g_log << "reading input..." << endl;
        std::fstream input_file(parser.get("sas_file"));
        input_task = probfd::tasks::read_root_tasks(input_file);
        utils::g_log << "done reading input!" << endl;
    }

    std::unique_ptr<SolverInterface> solver =
        solver_factory->create(tasks::g_root_task);

    utils::g_search_timer.resume();
    bool found_solution = solver->solve();
    utils::g_search_timer.stop();
    utils::g_timer.stop();

    solver->print_statistics();
    std::cout << "Search time: " << utils::g_search_timer << endl;
    std::cout << "Total time: " << utils::g_timer << endl;

    ExitCode exitcode = found_solution ? ExitCode::SUCCESS
                                       : ExitCode::SEARCH_UNSOLVED_INCOMPLETE;
    utils::report_exit_code_reentrant(exitcode);
    return static_cast<int>(exitcode);
}

void setup_argparser(argparse::ArgumentParser& arg_parser)
{
    argparse::ArgumentParser& search_parser = arg_parser.emplace_subparser(
        "search",
        "",
        argparse::default_arguments::help);

    search_parser.add_description("Runs the search component.");
    search_parser.add_argument("--epsilon")
        .help("The floating-point precision used for convergence checks.")
        .metavar("FLOAT")
        .scan<'g', float>();

    search_parser.add_argument("--predefinition")
        .help("[Deprecated] Feature predefinition. The options --landmarks, "
              "--evaluator and --heuristic are aliases for this option.")
        .append()
        .metavar("FEATURE_STRING");

    search_parser.add_argument("algorithm")
        .help("The search algorithm factory string. For more information, see "
              "the list-features subcommand.")
        .required();

    search_parser.add_argument("sas_file")
        .help("The translated PPDDL planning problem file.")
        .required()
        .action([](const auto& path) {
            if (!std::filesystem::exists(path)) {
                std::cerr << "Input file does not exist: " << path << std::endl;
                exit(static_cast<int>(utils::ExitCode::SEARCH_INPUT_ERROR));
            }

            return path;
        });

    search_parser.add_argument("fn").hidden().default_value<SubCommandFn>(
        search);

    argparse::ArgumentParser& feature_list_parser =
        arg_parser.emplace_subparser(
            "list-features",
            "",
            argparse::default_arguments::help);
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
        .help("If specified, all features of specified categories will be "
              "listed explicitly as well.")
        .flag();
    feature_list_parser.add_argument("features")
        .help("Individual features to list.")
        .remaining();

    feature_list_parser.add_argument("fn").hidden().default_value<SubCommandFn>(
        list_features);
}

} // namespace probfd