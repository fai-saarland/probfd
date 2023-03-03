#include "probfd/solvers/mdp_solver.h"

#include "probfd/cost_model.h"

#include "utils/timer.h"

#include "heuristic.h"
#include "operator_cost.h"
#include "option_parser.h"
#include "plugin.h"

#include "probfd/tasks/root_task.h"

#include <iomanip>
#include <vector>

namespace probfd {
namespace solvers {

MDPSolver::MDPSolver(const options::Options& opts)
    : task(tasks::g_root_task)
    , task_proxy(*task)
    , state_registry_(task_proxy)
    , state_space_(
          task,
          &state_registry_,
          opts.get_list<std::shared_ptr<Evaluator>>(
              "path_dependent_evaluators"),
          opts.get<bool>("cache"))
    , cost_function_(g_cost_model->get_cost_function())
    , progress_(
          opts.contains("report_epsilon")
              ? std::optional<value_t>(opts.get<value_t>("report_epsilon"))
              : std::nullopt,
          std::cout,
          opts.get<bool>("report_enabled"))
{
    StateRegistry* state_registry = &state_registry_;
    progress_.register_print([state_registry](std::ostream& out) {
        out << "registered=" << state_registry->size();
    });
}

void MDPSolver::solve()
{
    std::cout << "Running MDP engine " << get_engine_name() << "..."
              << std::endl;
    utils::Timer total_timer;
    std::unique_ptr<engines::MDPEngineInterface<State, OperatorID>> engine(
        create_engine());

    const State initial_state = state_registry_.get_initial_state();

    Interval val = engine->solve(initial_state);
    progress_.force_print();
    total_timer.stop();

    std::cout << "analysis done. [t=" << utils::g_timer << "]" << std::endl;

    std::cout << std::endl;

    print_analysis_result(val);

    std::cout << std::endl;
    std::cout << "State space interface:" << std::endl;
    std::cout << "  Registered state(s): " << state_registry_.size()
              << std::endl;
    state_space_.print_statistics(std::cout);

    std::cout << std::endl;
    std::cout << "Engine " << get_engine_name() << " statistics:" << std::endl;
    std::cout << "  Actual solver time: " << total_timer << std::endl;
    engine->print_statistics(std::cout);

    print_additional_statistics();
}

void MDPSolver::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<bool>("cache", "", "false");
    parser.add_list_option<std::shared_ptr<Evaluator>>(
        "path_dependent_evaluators",
        "",
        "[]");
    parser.add_option<value_t>("report_epsilon", "", "1e-4");
    parser.add_option<bool>("report_enabled", "", "true");
}

} // namespace solvers
} // namespace probfd
