#include "probfd/command_line.h"

#include "options/doc_printer.h"
#include "options/predefinitions.h"
#include "options/registries.h"

#include "utils/rng.h"
#include "utils/strings.h"
#include "utils/timer.h"

#include "probfd/cost_models/maxprob_cost_model.h"
#include "probfd/cost_models/ssp_cost_model.h"

#include "probfd/solver_interface.h"

#include "probfd/value_type.h"

#include "probfd/tasks/root_task.h"

#include "operator_cost.h"
#include "task_utils/successor_generator.h"

#include "option_parser.h"
#include "search_engine.h"

#include "task_utils/causal_graph.h"

#include <algorithm>
#include <limits>
#include <vector>

using namespace std;

namespace probfd {

ArgError::ArgError(const string& msg)
    : msg(msg)
{
}

void ArgError::print() const
{
    cerr << "argument error: " << msg << endl;
}

static string sanitize_arg_string(string s)
{
    // Convert newlines to spaces.
    replace(s.begin(), s.end(), '\n', ' ');
    // Convert string to lower case.
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

/*
static int parse_int_arg(const string& name, const string& value)
{
    try {
        return stoi(value);
    } catch (invalid_argument&) {
        throw ArgError("argument for " + name + " must be an integer");
    } catch (out_of_range&) {
        throw ArgError("argument for " + name + " is out of range");
    }
}
*/

static double parse_double_arg(const string& name, const string& value)
{
    try {
        return stod(value);
    } catch (invalid_argument&) {
        throw ArgError("argument for " + name + " must be a number");
    } catch (out_of_range&) {
        throw ArgError("argument for " + name + " is out of range");
    }
}

static std::shared_ptr<SolverInterface> parse_cmd_line_aux(
    const vector<string>& args,
    options::Registry& registry,
    bool dry_run)
{
    options::Predefinitions predefinitions;

    std::shared_ptr<SolverInterface> engine;
    /*
      Note that we don’t sanitize all arguments beforehand because filenames
      should remain as-is (no conversion to lower-case, no conversion of
      newlines to spaces).
    */
    // TODO: Remove code duplication.
    for (size_t i = 0; i < args.size(); ++i) {
        string arg = sanitize_arg_string(args[i]);
        bool is_last = (i == args.size() - 1);
        if (arg == "--search") {
            if (is_last) throw ArgError("missing argument after --search");
            ++i;
            OptionParser parser(
                sanitize_arg_string(args[i]),
                registry,
                predefinitions,
                dry_run);
            engine = parser.start_parsing<std::shared_ptr<SolverInterface>>();
        } else if (arg == "--help" && dry_run) {
            cout << "Help:" << endl;
            bool txt2tags = false;
            vector<string> plugin_names;
            for (size_t j = i + 1; j < args.size(); ++j) {
                string help_arg = sanitize_arg_string(args[j]);
                if (help_arg == "--txt2tags") {
                    txt2tags = true;
                } else {
                    plugin_names.push_back(help_arg);
                }
            }
            unique_ptr<options::DocPrinter> doc_printer;
            if (txt2tags)
                doc_printer =
                    std::make_unique<options::Txt2TagsPrinter>(cout, registry);
            else
                doc_printer =
                    std::make_unique<options::PlainPrinter>(cout, registry);
            if (plugin_names.empty()) {
                doc_printer->print_all();
            } else {
                for (const string& name : plugin_names) {
                    doc_printer->print_plugin(name);
                }
            }
            cout << "Help output finished." << endl;
            exit(0);
        } else if (
            utils::startswith(arg, "--") &&
            registry.is_predefinition(arg.substr(2))) {
            if (is_last) throw ArgError("missing argument after " + arg);
            ++i;
            registry.handle_predefinition(
                arg.substr(2),
                sanitize_arg_string(args[i]),
                predefinitions,
                dry_run);
        } else {
            throw ArgError("unknown option " + arg);
        }
    }

    return engine;
}

std::shared_ptr<SolverInterface> parse_cmd_line(
    int argc,
    const char** argv,
    options::Registry& registry,
    bool dry_run,
    bool is_unit_cost)
{
    vector<string> args;
    bool active = true;

    bool expected_cost = true;

    for (int i = 1; i < argc; ++i) {
        string arg = sanitize_arg_string(argv[i]);

        if (arg == "--if-unit-cost") {
            active = is_unit_cost;
        } else if (arg == "--if-non-unit-cost") {
            active = !is_unit_cost;
        } else if (arg == "--always") {
            active = true;
        } /* else if (
             arg == "--horizon" || arg == "--budget" || arg == "--step-bound") {
             active = true;
             if (i + 1 == argc) {
                 throw ArgError("missing argument after " + arg);
             }
             const std::string budget = argv[++i];
             if (budget != "infinity") {
                 probfd::g_step_bound = parse_int_arg(arg, budget);
                 probfd::g_steps_bounded = true;
                 if (probfd::g_step_bound < 0) {
                     throw ArgError("budget must be non-negative");
                 }
             }
         } else if (arg == "--step-cost-type" || arg == "--budget-cost-type") {
             active = true;
             if (i + 1 == argc) {
                 throw ArgError("missing argument after " + arg);
             }
             string type = sanitize_arg_string(argv[++i]);
             if (type == "normal") {
                 probfd::g_step_cost_type = OperatorCost::NORMAL;
             } else if (type == "one") {
                 probfd::g_step_cost_type = OperatorCost::ONE;
             } else if (type == "plusone") {
                 probfd::g_step_cost_type = OperatorCost::PLUSONE;
             } else if (type == "max") {
                 probfd::g_step_cost_type = OperatorCost::MAX_OPERATOR_COST;
                 throw ArgError("unknown operator cost type " + type);
             }
         }*/
        else if (arg == "--epsilon") {
            active = true;
            if (i + 1 == argc) {
                throw ArgError("missing argument after " + arg);
            }
            probfd::g_epsilon = parse_double_arg(arg, argv[i + 1]);
            ++i;
        } else if (arg == "--maxprob") {
            expected_cost = false;
        } else if (active) {
            // We use the unsanitized arguments because sanitizing is
            // inappropriate for things like filenames.
            args.push_back(argv[i]);
        }
    }

    if (!dry_run) {
        if (expected_cost) {
            g_cost_model.reset(new cost_models::SSPCostModel());
            std::cout << "Using SSP cost model." << std::endl;
        } else {
            g_cost_model.reset(new cost_models::MaxProbCostModel());
            std::cout << "Using MaxProb cost model." << std::endl;
        }
    }

    return parse_cmd_line_aux(args, registry, dry_run);
}

string usage(const string& progname)
{
    return "usage: \n" + progname +
           " [OPTIONS] --search SEARCH < OUTPUT\n\n"
           "* SEARCH (SearchEngine): configuration of the search algorithm\n"
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