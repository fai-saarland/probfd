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
#include <stdexcept>
#include <vector>

#include <argparse/argparse.hpp>

using namespace std;
using namespace downward::cli::parser;
using namespace downward::cli::plugins;

using utils::ExitCode;

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

static int list_features(argparse::ArgumentParser& parser)
{
    Registry registry = RawRegistry::instance()->construct_registry();
    unique_ptr<DocPrinter> doc_printer;
    if (parser.get<bool>("--txt2tags")) {
        doc_printer = std::make_unique<Txt2TagsPrinter>(cout, registry);
    } else {
        doc_printer = std::make_unique<PlainPrinter>(cout, registry);
    }

    if (auto features = parser.present<std::vector<std::string>>("features")) {
        for (const string& name : *features) {
            doc_printer->print_feature(name);
        }
    } else {
        doc_printer->print_all();
    }

    cout << "Help output finished." << endl;

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
            exit(static_cast<int>(utils::ExitCode::SEARCH_INPUT_ERROR));
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
        exit(static_cast<int>(utils::ExitCode::SEARCH_INPUT_ERROR));
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
    feature_list_parser.add_description("Lists available search features.");

    feature_list_parser.add_argument("--txt2tags")
        .help("List features in txt2tags format.")
        .flag();
    feature_list_parser.add_argument("features")
        .help("The features to list. If none are given, all available features "
              "are listed.")
        .remaining();

    feature_list_parser.add_argument("fn").hidden().default_value<SubCommandFn>(
        list_features);
}

} // namespace probfd