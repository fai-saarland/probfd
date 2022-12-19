#include "search_engine.h"

#include "globals.h"
#include "operator_cost.h"
#include "option_parser.h"
#include "plugin.h"

#include "utils/countdown_timer.h"
#include "utils/timer.h"

#include <cassert>
#include <iostream>
#include <limits>

using namespace std;

SearchEngine::SearchEngine(const options::Options& opts)
    : status(IN_PROGRESS)
    , solution_found(false)
    , state_registry(std::make_shared<StateRegistry>())
    , search_space(OperatorCost(opts.get_enum("cost_type")), nullptr)
    , cost_type(OperatorCost(opts.get_enum("cost_type")))
    , max_time(opts.get<double>("max_time"))
{
    if (opts.get<int>("bound") < 0) {
        cerr << "error: negative cost bound " << opts.get<int>("bound") << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
    bound = opts.get<int>("bound");
    search_space.set_state_registry(state_registry);
}

SearchEngine::~SearchEngine() { }

void
SearchEngine::print_statistics() const
{
    std::cout << std::endl;
    std::cout << "Search Statistics:" << std::endl;
    search_progress.print_statistics();
    std::cout << "  Number of registered states: " << state_registry->size()
              << std::endl;
    search_space.statistics();
}

bool
SearchEngine::found_solution() const
{
    return solution_found;
}

SearchStatus
SearchEngine::get_status() const
{
    return status;
}

const Plan& SearchEngine::get_plan() const
{
    assert(solution_found);
    return plan;
}

void
SearchEngine::set_plan(const Plan& p)
{
    solution_found = true;
    plan = p;
}

void
SearchEngine::set_state_registry(std::shared_ptr<StateRegistry> state_registry)
{
    assert(this->state_registry == nullptr);
    this->state_registry = state_registry;
    search_space.set_state_registry(state_registry);
}

void SearchEngine::search()
{
    initialize();
    utils::CountdownTimer timer(max_time);
    while (status == IN_PROGRESS) {
        status = step();
        if (timer.is_expired()) {
            cout << "Time limit reached. Abort search." << endl;
            status = TIMEOUT;
            break;
        }
    }
    cout << "Actual search time: " << timer.get_elapsed_time()
         << " [t=" << utils::g_timer << "]" << endl;
}

bool
SearchEngine::check_goal_and_set_plan(const GlobalState& state)
{
    if (test_goal(state)) {
        cout << "Solution found!" << endl;
        Plan plan;
        search_space.trace_path(state, plan);
        set_plan(plan);
        return true;
    }
    return false;
}

void SearchEngine::save_plan_if_necessary()
{
    if (found_solution()) plan_manager.save_plan(get_plan());
}

int
SearchEngine::get_adjusted_cost(const GlobalOperator& op) const
{
    return get_adjusted_action_cost(op, cost_type);
}

void
SearchEngine::add_options_to_parser(options::OptionParser& parser)
{
    ::add_cost_type_option_to_parser(parser);
    parser.add_option<int>(
        "bound",
        "exclusive depth bound on g-values. Cutoffs are always performed "
        "according to "
        "the real cost, regardless of the cost_type parameter",
        "infinity");
    parser.add_option<double>(
        "max_time",
        "maximum time in seconds the search is allowed to run for. The "
        "timeout is only checked after each complete search step "
        "(usually a node expansion), so the actual runtime can be arbitrarily "
        "longer. Therefore, this parameter should not be used for "
        "time-limiting "
        "experiments. Timed-out searches are treated as failed searches, "
        "just like incomplete search algorithms that exhaust their search "
        "space.",
        "infinity");
}

static PluginTypePlugin<SearchEngine> _type_plugin(
    "SearchEngine",
    // TODO: Replace empty string by synopsis for the wiki page.
    "");