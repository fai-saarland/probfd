#include "probfd/command_line.h"

#include "options/doc_printer.h"
#include "options/predefinitions.h"
#include "options/registries.h"

#include "utils/rng.h"
#include "utils/strings.h"
#include "utils/timer.h"

#include "probfd/analysis_objectives/expected_cost_objective.h"
#include "probfd/analysis_objectives/goal_probability_objective.h"

#include "probfd/solvers/solver_interface.h"

#include "probfd/globals.h"
#include "probfd/value_type.h"

#include "legacy/global_operator.h"
#include "legacy/globals.h"
#include "legacy/operator_cost.h"
#include "legacy/successor_generator.h"

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
    using namespace probfd::analysis_objectives;

    vector<string> args;
    bool active = true;

    bool build_causal_graph = true;
    bool build_successor_generator = true;

    bool expected_cost = false;

    for (int i = 1; i < argc; ++i) {
        string arg = sanitize_arg_string(argv[i]);

        if (arg == "--if-unit-cost") {
            active = is_unit_cost;
        } else if (arg == "--if-non-unit-cost") {
            active = !is_unit_cost;
        } else if (arg == "--always") {
            active = true;
        } else if (
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
                probfd::g_step_cost_type = legacy::NORMAL;
            } else if (type == "one") {
                probfd::g_step_cost_type = legacy::ONE;
            } else if (type == "plusone") {
                probfd::g_step_cost_type = legacy::PLUSONE;
            } else if (type == "zero") {
                probfd::g_step_cost_type = legacy::ZERO;
            } else if (type == "minone") {
                probfd::g_step_cost_type = legacy::MINONE;
            } else {
                throw ArgError("unknown operator cost type " + type);
            }
        } else if (arg == "-d" || arg == "--define") {
            active = true;
            if (i + 1 < argc) {
                string defs = sanitize_arg_string(argv[i + 1]);
                size_t j = defs.find(',');
                while (true) {
                    string def = defs.substr(0, j);
                    if (def == "no_cg") {
                        build_causal_graph = false;
                    } else if (def == "no_sg") {
                        build_successor_generator = false;
                    } else {
                        throw ArgError("Unknown define " + def);
                    }
                    if (j == string::npos) {
                        break;
                    }
                    defs = defs.substr(j + 1, string::npos);
                    j = defs.find(',');
                }
                i++;
            }
        } else if (arg == "--seed") {
            active = true;
            if (i + 1 == argc) {
                throw ArgError("missing argument after " + arg);
            }
            int seed = parse_int_arg(arg, argv[i + 1]);
            cout << "Setting RNG seed: " << seed << endl;
            legacy::g_rng.seed(seed);
            ++i;
        } else if (arg == "--epsilon") {
            active = true;
            if (i + 1 == argc) {
                throw ArgError("missing argument after " + arg);
            }
            probfd::value_type::g_epsilon = parse_double_arg(arg, argv[i + 1]);
            ++i;
        } else if (arg == "--property") {
            active = true;
            if (i + 1 == argc) {
                throw ArgError("missing argument after " + arg);
            }
            string prop = sanitize_arg_string(argv[i + 1]);
            if (prop == "maxprob") {
            } else if (prop == "expcost") {
                expected_cost = true;
            } else {
                throw ArgError("unknown property " + prop);
            }
            ++i;
        } else if (active) {
            // We use the unsanitized arguments because sanitizing is
            // inappropriate for things like filenames.
            args.push_back(argv[i]);
        }
    }

    if (!dry_run) {
        if (build_causal_graph) {
            cout << "building causal graph..." << flush;
            legacy::g_causal_graph = new causal_graph::CausalGraph;
            cout << "done! [t=" << utils::g_timer << "]" << endl;
        }

        if (build_successor_generator) {
            cout << "building successor generator..." << flush;
            legacy::successor_generator::g_successor_generator =
                std::make_shared<
                    legacy::successor_generator::SuccessorGenerator<
                        const legacy::GlobalOperator*>>();
            cout << "done! [t=" << utils::g_timer << "]" << endl;
        }

        std::shared_ptr<probfd::analysis_objectives::AnalysisObjective> obj =
            nullptr;
        if (expected_cost) {
            obj.reset(new ExpectedCostObjective());
            std::cout << "expected cost analysis." << std::endl;
        } else {
            obj.reset(new GoalProbabilityObjective());
            std::cout << "max goal prob analysis." << std::endl;
        }

        probfd::prepare_globals(obj);
        probfd::print_task_info();
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
           "--internal-plan-file FILENAME\n"
           "    Plan will be output to a file called FILENAME\n\n"
           "--internal-previous-portfolio-plans COUNTER\n"
           "    This planner call is part of a portfolio which already "
           "created\n"
           "    plan files FILENAME.1 up to FILENAME.COUNTER.\n"
           "    Start enumerating plan files with COUNTER+1, i.e. "
           "FILENAME.COUNTER+1\n\n"
           "See http://www.fast-downward.org/ for details.";
}
} // namespace probfd