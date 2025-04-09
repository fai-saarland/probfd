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
        TokenStream tokens = split_tokens(search_arg);
        ASTNodePtr parsed = parse(tokens);
        DecoratedASTNodePtr decorated = parsed->decorate();

        if (parser.get<bool>("--ignore-unused-definitions")) {
            std::vector<VariableDefinition> unused_defs;
            decorated->prune_unused_definitions(unused_defs);

            if (!unused_defs.empty()) {
                std::cout
                    << "Removed unused variables from feature expression: ";

                {
                    const auto var_name = unused_defs.front().variable_name;
                    std::print(std::cout, "{}", var_name);
                }

                for (const auto& def : unused_defs | std::views::drop(1)) {
                    std::print(std::cout, ", {}", def.variable_name);
                }

                std::println(std::cout);
            }
        }

        std::cout << "Constructing solver from feature expression:\n";
        decorated->print(std::cout, 4, false);
        std::println(std::cout);

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
            "These definitions will be converted into a let expression "
            "containing the search string as its body.")
        .filepath()
        .metavar("FILEPATH");

    search_parser.add_argument("--ignore-unused-definitions")
        .help(
            "If specified, unused definitions in let expressions of the search "
            "string are ignored and the corresponding component will not be "
            "constructed. By default, all expressions are evaluated, so "
            "even features ultimately not used by the solver factory are "
            "constructed.")
        .flag()
        .default_value(true);

    search_parser.add_argument("algorithm")
        .help(
            "A feature expression evaluating to a search algorithm factory "
            "(TaskSolverFactory). The factory will be invoked to construct a "
            "solver for the input planning task which will be used to compute "
            "a policy. "
            "The expression grammar is according to the BNF:\n"
            "\n"
            "EXPRESSION := LET_EXPRESSION | LIST_EXPRESSION | FUNCTION_CALL | "
            "IDENTIFIER | BOOLEAN | STRING | INTEGER | FLOAT\n"
            "LET_EXPRESSION := 'let' VARIABLE_DEFINITION [ , "
            "VARIABLE_DEFINITION ]* 'in' EXPRESSION\n"
            "VARIABLE_DEFINITION := EXPRESSION 'as' IDENTIFIER\n"
            "IDENTIFIER := LETTER [ LETTER | DIGIT ]* (except 'infinity', "
            "'in', 'as', 'let', 'true', 'false')\n"
            "LIST_EXPRESSION := '[' [ EXPRESSION_LIST ] ']'\n"
            "EXPRESSION_LIST := EXPRESSION [ , EXPRESSION ]*\n"
            "FUNCTION_CALL := IDENTIFIER '(' [ POSITIONAL_ARGUMENTS ] [ "
            "KEYWORD_ARGUMENTS ] ')'\n"
            "POSITIONAL_ARGUMENTS := EXPRESSION_LIST\n"
            "KEYWORD_ARGUMENTS := IDENTIFIER '=' EXPRESSION [ , IDENTIFIER '=' "
            "EXPRESSION ]*\n"
            "BOOLEAN := 'true' | 'false'\n"
            "STRING := '\"' <any non-control character except '\"'> '\"'\n"
            "INTEGER := [ '+' | '-' ] POSITIVE_INTEGER | 'infinity' \n"
            "POSITIVE_INTEGER := '0' | [ 1-9 ] [ 0-9 ]*\n"
            "FLOAT := INTEGER [ '.' POSITIVE_INTEGER ] [ 'e' INTEGER ] [ 'k' | "
            "'m' | 'g' ]\n"
            "\n"
            //"The BNF corresponds to the following syntactic constructs:"
            //"Let[x_1, e_1, ..., x_n, e_n, b], where x_1, ... x_n are the "
            //"declared identifiers, e_1, ..., e_n are the variable definition "
            //"expressions, and b is the body expression\n"
            //"Call[f, a_1, ... a_m, x_1, ..., x_n, e_1, ..., e_n], where f is "
            //"the function identifier, a_1, ..., a_m are the positional "
            //"argument expressions, x_1, ..., x_n are the used keyword "
            //"identifiers and e_1, ..., e_n are the keyword argument "
            //"expressions\n"
            //"List[e_1, ...., e_n], where e_1, ..., e_n are the list element "
            //"expressions\n"
            //"Id[x], where x is a string\n"
            //"Bool[b], where b is true or false\n"
            //"String[s], where s is a string\n"
            //"Int[i], where i a a natural number\n"
            //"Float[f], where f is a real number\n"
            "Identifiers can either be declared in let-expressions to be used "
            "within their body, pre-defined in a definitions file "
            "(see option --definitions-file) or are pre-defined by the planner "
            "and designate a specific feature like a heuristic or solver "
            "factory. All available pre-defined variables and their detailed "
            "descriptions can be listed via the sub-command 'list-features' "
            "(see 'list-features --help').\n"
            "Please note that shadowing identifiers is not supported, i.e., it "
            "is not possible to re-define the same symbol in a nested "
            "let-expression or to shadow any of the available feature "
            "pre-definitions.\n\n"
            "Example: let ppdbs() as h in ilao(heuristic=h)\n"
            "Declares a variable h that is bound to the result of the "
            "expression ppdbs() of type TaskHeuristicFactory "
            "(see also 'list-features ppdbs'). "
            "Evaluates to a TaskSolverFactory that will create an instance of "
            "the iLAO* algorithm to solve the input task, which will use "
            "the heuristic for the task that is constructed with the "
            "TaskHeuristicFactory bound to h, here a probabilistic PDB "
            "heuristic (see also 'list-features ilao'.")
        .required()
        .metavar("FEATURE_EXPRESSION");

    search_parser.add_argument("sas_file")
        .help("The translated PPDDL planning problem file.")
        .required()
        .filepath();
}

} // namespace probfd