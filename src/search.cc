#include "search.h"

#include "downward/cli/parser/lexical_analyzer.h"
#include "downward/cli/parser/syntax_analyzer.h"
#include "downward/cli/parser/token_stream.h"
#include "register_definitions.h"

#include "downward/cli/plugins/raw_registry.h"

#include "probfd/tasks/root_task.h"

#include "probfd/utils/timed.h"

#include "probfd/probabilistic_operator_space.h"
#include "probfd/probabilistic_task.h"
#include "probfd/solver_interface.h"
#include "probfd/termination_costs.h"

#include "downward/axiom_space.h"
#include "downward/goal_fact_list.h"
#include "downward/initial_state_values.h"
#include "downward/operator_cost_function.h"
#include "downward/variable_space.h"

#include "downward/utils/logging.h"
#include "downward/utils/strings.h"
#include "downward/utils/system.h"
#include "probfd/json/json.h"
#include "probfd/utils/guards.h"

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

static ASTNodePtr tokenize_and_parse(const std::string& expression)
{
    TokenStream tokens = split_tokens(expression);
    return parse(tokens);
}

template <std::ranges::input_range R>
    requires std::convertible_to<
        std::ranges::range_value_t<R>,
        std::pair<std::string, std::string>>
static ASTNodePtr construct_let(ASTNodePtr parsed, R&& predefinitions)
{
    if (std::ranges::empty(predefinitions)) return parsed;

    std::vector<std::pair<std::string, ASTNodePtr>> variable_definitions;

    for (const auto& [key, definition] : predefinitions) {
        variable_definitions.emplace_back(key, tokenize_and_parse(definition));
    }

    return std::make_unique<LetNode>(
        std::move(variable_definitions),
        std::move(parsed));
}

static ASTNodePtr
insert_definitions(ASTNodePtr parsed, const std::string& definitions_file)
{
    std::ifstream fs(definitions_file);
    try {
        auto defs = json::read<std::map<std::string, std::string>>(fs);
        parsed = construct_let(std::move(parsed), defs);
    } catch (std::invalid_argument& e) {
        throw InputError(
            "Could not read definitions from {}:\n{}",
            definitions_file,
            e.what());
    }

    return parsed;
}

static shared_ptr<TaskSolverFactory>
construct_solver(const DecoratedASTNode& decorated)
{
    std::cout << "Constructing solver from feature expression:\n";
    decorated.print(std::cout, 4, false);
    std::println(std::cout);

    std::any constructed = decorated.construct();

    try {
        return std::any_cast<shared_ptr<TaskSolverFactory>>(constructed);
    } catch (const std::bad_any_cast&) {
        throw InputError(
            "Expression is of type {}, not TaskSolverFactory.",
            constructed.type().name());
    }
}

static auto construct_solver(argparse::ArgumentParser& parser)
{
    // Signal handler setup
    register_event_handlers();

    // Parse search string
    ASTNodePtr parsed = tokenize_and_parse(parser.get("algorithm"));

    // Insert user-defined pre-definitions, if given
    if (const auto definitions_file = parser.present("--definitions-file")) {
        parsed = insert_definitions(std::move(parsed), *definitions_file);
    }

    // Register internal pre-definitions
    RawRegistry raw_registry;
    register_definitions(raw_registry);

    // Type check
    const DecoratedASTNodePtr decorated = parsed->decorate(raw_registry);

    // Remove unused definitions if enabled
    if (parser.get<bool>("--ignore-unused-definitions")) {
        if (const auto defs = decorated->prune_unused_definitions();
            !defs.empty()) {
            std::println(
                std::cout,
                "Removed unused variables from feature expression: {}",
                defs | views::transform(&VariableDefinition::variable_name));
        }
    }

    // Construct solver factory.
    const auto solver_factory = construct_solver(*decorated);

    // Read the input task.
    const SharedProbabilisticTask input_task = run_time_logged(
        std::cout,
        "Reading input task...",
        probfd::tasks::read_sas_task_from_file,
        parser.get("problem_file"));

    // Create the solver from the input task and return it.
    return solver_factory->create(input_task);
}

static ExitCode run_solver(SolverInterface& solver, Duration max_time)
{
    // Print search time.
    Timer timer;
    scope_exit _([&] { std::println(cout, "Search time: {}", timer()); });

    const bool found_solution = solver.solve(max_time);
    timer.stop();

    solver.print_statistics();

    return found_solution ? ExitCode::SUCCESS
                          : ExitCode::SEARCH_UNSOLVED_INCOMPLETE;
}

static int search(argparse::ArgumentParser& parser)
{
    // Print total time.
    Timer timer;
    scope_exit _([&] { println(cout, "Total time: {}", timer()); });

    ExitCode exitcode;

    try {
        try {
            const auto solver = construct_solver(parser);
            exitcode = run_solver(
                *solver,
                static_cast<Duration>(parser.get<double>("--max-search-time")));
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
    } catch (const ContextError&) {
        exitcode = ExitCode::SEARCH_INPUT_ERROR;
    } catch (const InputError&) {
        exitcode = ExitCode::SEARCH_INPUT_ERROR;
    } catch (const CriticalError&) {
        exitcode = ExitCode::SEARCH_CRITICAL_ERROR;
    } catch (const UnsupportedError&) {
        exitcode = ExitCode::SEARCH_UNSUPPORTED;
    } catch (const UnimplementedError&) {
        exitcode = ExitCode::SEARCH_UNIMPLEMENTED;
    } catch (const std::exception&) {
        exitcode = ExitCode::SEARCH_CRITICAL_ERROR;
    }

    std::println(std::cout, "");

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
            "A factory that produces a generic solver for the input planning "
            "task. This generic solver will then be run after it was created, "
            "which may for example output a policy.\n"
            "The solver factory is expressed using a formal expression "
            "language. The expression grammar is according to the BNF:\n"
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
            "itself and designate a specific planner feature, e.g., a specific "
            "solver factory implementation. All available pre-defined "
            "variables and expression types can be listed via the sub-command "
            "'list-features' (see 'list-features --help')."
            "The root expression must have type 'TaskSolverFactory'.\n\n"
            "Example:\nlet ppdbs() as h in ilao(heuristic=h)\n\n"
            "Declares a variable h that is bound to the result of the "
            "expression ppdbs(). "
            "Evaluates to a TaskSolverFactory that will create an instance of "
            "the iLAO* algorithm to solve the input task, which will use "
            "the heuristic for the task that is constructed with the "
            "TaskHeuristicFactory bound to h, here a probabilistic PDB "
            "heuristic (see also 'list-features TaskSolverFactory ilao ppdbs'.")
        .required()
        .metavar("FEATURE_EXPRESSION");

    search_parser.add_argument("problem_file")
        .help(
            "The problem file in probabilistic SAS+ format. Note that PPDDL "
            "files are not directly supported. These must be translated into "
            "the SAS+ format first using the translator. "
            "See 'fast-downward.py --help' for more information on how to "
            "invoke the translator before running the search component.")
        .required()
        .filepath();
}

} // namespace probfd