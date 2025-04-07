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
#include "probfd/json/json.h"

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
using namespace downward::utils;
using namespace downward::cli::parser;
using namespace downward::cli::plugins;

using downward::utils::ExitCode;

namespace probfd {

template <std::ranges::input_range R>
    requires std::convertible_to<
        std::ranges::range_value_t<R>,
        std::pair<std::string, std::string>>
static string
insert_definitions(const std::string& old_search_argument, R&& predefinitions)
{
    auto it = std::ranges::begin(predefinitions);
    const auto end = std::ranges::end(predefinitions);

    if (it == end) return old_search_argument;

    ostringstream new_search_argument;
    new_search_argument << "let ";

    {
        const auto& [key, definition] = *it;
        new_search_argument << definition << " as " << key;
    }

    for (++it; it != end; ++it) {
        const auto& [key, definition] = *it;
        new_search_argument << ", " << definition << " as " << key;
    }

    new_search_argument << " in " << old_search_argument;

    return new_search_argument.str();
}

static int search(argparse::ArgumentParser& parser)
{
    const double max_time = parser.get<double>("--max-search-time");

    std::string search_arg = parser.get("algorithm");

    if (auto definitions_file = parser.present("--definitions-file")) {
        std::ifstream fs(*definitions_file);
        try {
            auto defs = json::read<std::map<std::string, std::string>>(fs);
            search_arg = insert_definitions(search_arg, defs);
        } catch (std::invalid_argument& e) {
            std::println(
                "Could not read definitions from {}: {}",
                *definitions_file,
                e.what());
            return static_cast<int>(ExitCode::SEARCH_INPUT_ERROR);
        }
    }

    register_event_handlers();

    shared_ptr<TaskSolverFactory> solver_factory;

    try {
        std::println(std::cout, "Using search string: '{}'", search_arg);
        TokenStream tokens = split_tokens(search_arg);
        ASTNodePtr parsed = parse(tokens);
        DecoratedASTNodePtr decorated = parsed->decorate();
        if (parser.get<bool>("--ignore-unused-definitions")) {
            std::vector<VariableDefinition> unused_defs;
            decorated->prune_unused_definitions(unused_defs);
        }
        std::any constructed = decorated->construct();
        try {
            solver_factory =
                std::any_cast<shared_ptr<TaskSolverFactory>>(constructed);
        } catch (const std::bad_any_cast&) {
            std::println(
                std::cerr,
                "Search argument \"{}\" is of type {}, not TaskSolverFactory.",
                search_arg,
                constructed.type().name());
            return static_cast<int>(ExitCode::SEARCH_INPUT_ERROR);
        }
    } catch (const ContextError& e) {
        std::cerr << e.get_message() << std::endl;
        return static_cast<int>(ExitCode::SEARCH_INPUT_ERROR);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return static_cast<int>(ExitCode::SEARCH_CRITICAL_ERROR);
    }

    std::shared_ptr<ProbabilisticTask> input_task = run_time_logged(
        std::cout,
        "Reading input task...",
        probfd::tasks::read_root_tasks_from_file,
        parser.get("sas_file"));

    std::unique_ptr<SolverInterface> solver =
        solver_factory->create(input_task);

    g_search_timer.resume();
    bool found_solution = solver->solve(max_time);
    g_search_timer.stop();
    g_timer.stop();

    solver->print_statistics();
    std::cout << "Search time: " << g_search_timer << endl;
    std::cout << "Total time: " << g_timer << endl;

    ExitCode exitcode = found_solution ? ExitCode::SUCCESS
                                       : ExitCode::SEARCH_UNSOLVED_INCOMPLETE;
    report_exit_code_reentrant(exitcode);
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

    search_parser.add_argument("--definitions-file")
        .help(
            "Path to a file in JSON format containing feature definitions. "
            "These definitions will be converted into a let expressions "
            "containing the search string as its body.")
        .filepath()
        .metavar("FILEPATH");

    search_parser.add_argument("--ignore-unused-definitions")
        .help(
            "If true, unused definitions in let expressions of the search "
            "string are ignored and the corresponding component will not be "
            "constructed.")
        .flag()
        .default_value(true);

    search_parser.add_argument("algorithm")
        .help(
            "The search algorithm factory. For available options, see "
            "--list-features TaskSolverFactory.")
        .required();

    search_parser.add_argument("sas_file")
        .help("The translated PPDDL planning problem file.")
        .required()
        .filepath();
}

} // namespace probfd