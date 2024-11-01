#include "probfd/command_line.h"

#include "probfd/solver_interface.h"
#include "probfd/value_type.h"

#include "downward/parser/lexical_analyzer.h"
#include "downward/parser/syntax_analyzer.h"
#include "downward/parser/token_stream.h"

#include "downward/plugins/doc_printer.h"
#include "downward/plugins/raw_registry.h"
#include "downward/plugins/registry.h"

#include "downward/utils/logging.h"
#include "downward/utils/strings.h"
#include "downward/utils/system.h"

#include <any>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace plugins;

namespace probfd {

[[noreturn]] static void input_error(const string& msg)
{
    utils::g_log << msg << endl;
    utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
}

static double parse_double_arg(const string& name, const string& value)
{
    try {
        return stod(value);
    } catch (invalid_argument&) {
        input_error("argument for " + name + " must be a number");
    } catch (out_of_range&) {
        input_error("argument for " + name + " is out of range");
    }
}

static vector<string>
replace_old_style_predefinitions(const vector<string>& args)
{
    vector<string> new_args;
    int num_predefinitions = 0;
    bool has_search_argument = false;
    ostringstream new_search_argument;

    for (size_t i = 0; i < args.size(); ++i) {
        string arg = args[i];
        bool is_last = (i == args.size() - 1);

        if (arg == "--evaluator" || arg == "--heuristic" ||
            arg == "--landmarks") {
            if (has_search_argument)
                input_error("predefinitions are forbidden after the '--search' "
                            "argument");
            if (is_last) input_error("missing argument after " + arg);
            vector<string> predefinition = utils::split(args[++i], "=", 1);
            if (predefinition.size() < 2)
                input_error("predefinition expects format 'key=definition'");
            string key = predefinition[0];
            string definition = predefinition[1];
            if (!utils::is_alpha_numeric(key))
                input_error(
                    "predefinition key has to be alphanumeric: '" + key + "'");
            new_search_argument << "let(" << key << "," << definition << ",";
            num_predefinitions++;
        } else if (arg == "--search") {
            if (has_search_argument)
                input_error("at most one '--search' argument may be specified");
            if (is_last) input_error("missing argument after --search");
            arg = args[++i];
            new_args.emplace_back("--search");
            new_search_argument << arg << string(num_predefinitions, ')');
            new_args.push_back(new_search_argument.str());
            has_search_argument = true;
        } else {
            new_args.push_back(arg);
        }
    }
    if (!has_search_argument && num_predefinitions > 0)
        input_error("predefinitions require a '--search' argument");

    return new_args;
}

static std::shared_ptr<SolverInterface>
parse_cmd_line_aux(const vector<string>& args)
{
    using SearchPtr = shared_ptr<SolverInterface>;
    SearchPtr algorithm = nullptr;

    // TODO: Remove code duplication.
    for (size_t i = 0; i < args.size(); ++i) {
        const string& arg = args[i];
        bool is_last = (i == args.size() - 1);
        if (arg == "--search") {
            if (algorithm) input_error("multiple --search arguments defined");
            if (is_last) input_error("missing argument after --search");
            const string& search_arg = args[++i];
            try {
                parser::TokenStream tokens = parser::split_tokens(search_arg);
                parser::ASTNodePtr parsed = parser::parse(tokens);
                parser::DecoratedASTNodePtr decorated = parsed->decorate();
                std::any constructed = decorated->construct();
                algorithm = std::any_cast<SearchPtr>(constructed);
            } catch (const utils::ContextError& e) {
                input_error(e.get_message());
            }
        } else if (arg == "--help") {
            cout << "Help:" << endl;
            bool txt2tags = false;
            vector<string> plugin_names;
            for (size_t j = i + 1; j < args.size(); ++j) {
                const string& help_arg = args[j];
                if (help_arg == "--txt2tags") {
                    txt2tags = true;
                } else {
                    plugin_names.push_back(help_arg);
                }
            }
            plugins::Registry registry =
                plugins::RawRegistry::instance()->construct_registry();
            unique_ptr<plugins::DocPrinter> doc_printer;
            if (txt2tags) {
                doc_printer =
                    std::make_unique<plugins::Txt2TagsPrinter>(cout, registry);
            } else {
                doc_printer =
                    std::make_unique<plugins::PlainPrinter>(cout, registry);
            }

            if (plugin_names.empty()) {
                doc_printer->print_all();
            } else {
                for (const string& name : plugin_names) {
                    doc_printer->print_feature(name);
                }
            }

            cout << "Help output finished." << endl;
            exit(0);
        } else if (arg == "--epsilon") {
            if (is_last) input_error("missing argument after " + arg);
            probfd::g_epsilon = parse_double_arg(arg, args[++i]);
        } /* else if (
             arg == "--horizon" || arg == "--budget" || arg == "--step-bound") {
             if (is_last) throw ArgError("missing argument after " + arg);
             const std::string budget = argv[++i];

             if (budget != "infinity") {
                 probfd::g_step_bound = parse_int_arg(arg, budget);
                 probfd::g_steps_bounded = true;
                 if (probfd::g_step_bound < 0) input_error("budget must be
         non-negative");
             }
         } else if (arg == "--step-cost-type" || arg == "--budget-cost-type") {
             if (is_last == argc) input_error("missing argument after " + arg);
             const std::string type = argv[++i];

             if (type == "normal") {
                 probfd::g_step_cost_type = OperatorCost::NORMAL;
             } else if (type == "one") {
                 probfd::g_step_cost_type = OperatorCost::ONE;
             } else if (type == "plusone") {
                 probfd::g_step_cost_type = OperatorCost::PLUSONE;
             } else if (type == "max") {
                 probfd::g_step_cost_type = OperatorCost::MAX_OPERATOR_COST;
             } else {
                 input_error("unknown operator cost type " + type);
             }
         }*/
        else {
            input_error("unknown option " + arg);
        }
    }

    return algorithm;
}

std::shared_ptr<SolverInterface>
parse_cmd_line(int argc, const char** argv, bool is_unit_cost)
{
    vector<string> args;
    bool active = true;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "--if-unit-cost") {
            active = is_unit_cost;
        } else if (arg == "--if-non-unit-cost") {
            active = !is_unit_cost;
        } else if (arg == "--always") {
            active = true;
        } else if (active) {
            args.emplace_back(argv[i]);
        }
    }

    args = replace_old_style_predefinitions(args);
    return parse_cmd_line_aux(args);
}

string usage(const string& progname)
{
    return "usage: \n" + progname +
           " [OPTIONS] --search SEARCH < OUTPUT\n\n"
           "* SEARCH (SearchAlgorithm): configuration of the search algorithm\n"
           "* OUTPUT (filename): translator output\n\n"
           "Options:\n"
           "--maxprob\n"
           "    Use the MaxProb cost model, specifying a termination cost\n"
           "    of -1 for goal states and 0 otherwise, an no action costs.\n"
           "--help [NAME]\n"
           "    Prints help for all heuristics, open lists, etc. called NAME.\n"
           "    Without parameter: prints help for everything available\n"
           "--landmarks LANDMARKS_PREDEFINITION\n"
           "    Predefines a set of landmarks that can afterwards be "
           "referenced\n"
           "    by the name that is specified in the definition.\n"
           "--evaluator EVALUATOR_PREDEFINITION\n"
           "    Predefines an evaluator that can afterwards be referenced\n"
           "    by the name that is specified in the definition.\n"
           "    plan files FILENAME.1 up to FILENAME.COUNTER.\n"
           "    Start enumerating plan files with COUNTER+1, i.e. "
           "FILENAME.COUNTER+1";
}
} // namespace probfd