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
    , state_id_map_(&state_registry_)
    , cost_function_(g_cost_model->get_cost_function())
    , transition_generator_(
          task,
          &state_registry_,
          opts.get_list<std::shared_ptr<Evaluator>>(
              "path_dependent_evaluators"),
          opts.get<bool>("cache"))
    , progress_(
          opts.get<double>("report_epsilon"),
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
    std::unique_ptr<engines::MDPEngineInterface<State>> engine(create_engine());

    const State initial_state = state_registry_.get_initial_state();

    value_t val = engine->solve(initial_state);
    progress_.print();
    total_timer.stop();

    std::cout << "analysis done. [t=" << utils::g_timer << "]" << std::endl;

    std::cout << std::endl;

    print_analysis_result(val, engine->get_error(initial_state));

    std::cout << std::endl;
    std::cout << "State space interface:" << std::endl;
    std::cout << "  Registered state(s): " << state_registry_.size()
              << std::endl;
    transition_generator_.print_statistics(std::cout);

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
    parser.add_option<double>("report_epsilon", "", "1e-4");
    parser.add_option<bool>("report_enabled", "", "true");
}

} // namespace solvers
} // namespace probfd
