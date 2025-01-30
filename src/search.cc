#include "search.h"

#include "downward/cli/parser/lexical_analyzer.h"
#include "downward/cli/parser/syntax_analyzer.h"
#include "downward/cli/parser/token_stream.h"

#include "downward/cli/plugins/raw_registry.h"

#include "probfd/tasks/root_task.h"

#include "probfd/utils/timed.h"

#include "probfd/solver_interface.h"

#include "downward/utils/logging.h"
#include "downward/utils/strings.h"
#include "downward/utils/system.h"

#include <any>
#include <charconv>
#include <filesystem>
#include <iostream>
#include <limits>
#include <print>
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

    template <typename FormatContext>
    auto
    format(const downward::cli::plugins::Bounds& bounds, FormatContext& ctx)
        const
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

static int search(argparse::ArgumentParser& parser)
{
    const double max_time = parser.get<double>("--max-search-time");

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
        try {
            solver_factory =
                std::any_cast<shared_ptr<TaskSolverFactory>>(constructed);
        } catch (const std::bad_any_cast&) {
            std::println(
                std::cerr,
                "Search argument {:?} is of type {}, not TaskSolverFactory.",
                search_arg,
                constructed.type().name());
            return static_cast<int>(utils::ExitCode::SEARCH_INPUT_ERROR);
        }
    } catch (const utils::ContextError& e) {
        std::cerr << e.get_message() << std::endl;
        return static_cast<int>(utils::ExitCode::SEARCH_INPUT_ERROR);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return static_cast<int>(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }

    std::shared_ptr<ProbabilisticTask> input_task = timed(
        std::cout,
        "Reading input task...",
        probfd::tasks::read_root_tasks_from_file,
        parser.get("sas_file"));

    std::unique_ptr<SolverInterface> solver =
        solver_factory->create(input_task);

    utils::g_search_timer.resume();
    bool found_solution = solver->solve(max_time);
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

void add_search_subcommand(argparse::ArgumentParser& arg_parser)
{
    argparse::ArgumentParser& search_parser = arg_parser.emplace_subparser(
        std::forward_as_tuple("search", "", argparse::default_arguments::help),
        search);

    search_parser.add_description("Runs the search component.");

    search_parser.add_argument("--max-search-time")
        .help("The maximum search time for the algorithm.")
        .metavar("DOUBLE")
        .default_value(std::numeric_limits<double>::infinity())
        .scan<'g', double>(0.0);

    search_parser.add_argument("--predefinition")
        .help("[Deprecated] Feature predefinition. The options --landmarks, "
              "--evaluator and --heuristic are aliases for this option.")
        .append()
        .metavar("FEATURE_STRING");

    search_parser.add_argument("algorithm")
        .help("The search algorithm factory. For available options, see "
              "--list-features TaskSolverFactory.")
        .required();

    search_parser.add_argument("sas_file")
        .help("The translated PPDDL planning problem file.")
        .required()
        .filepath();
}

} // namespace probfd