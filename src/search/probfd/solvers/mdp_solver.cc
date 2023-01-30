#include "probfd/solvers/mdp_solver.h"

#include "probfd/analysis_objectives/analysis_objective.h"

#include "probfd/utils/logging.h"

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
    : progress_(
          opts.get<double>("report_epsilon"),
          std::cout,
          opts.get<bool>("report_enabled"))
    , task(tasks::g_root_task)
    , task_proxy(*task)
    , state_registry_(task_proxy)
    , state_id_map_(&state_registry_)
    , reward_function_(g_analysis_objective->reward())
    , transition_generator_(
          task,
          &state_registry_,
          opts.get_list<std::shared_ptr<Evaluator>>(
              "path_dependent_evaluators"),
          opts.get<bool>("cache"))
{
    StateRegistry* state_registry = &state_registry_;
    progress_.register_print([state_registry](std::ostream& out) {
        out << "registered=" << state_registry->size();
    });
}

void MDPSolver::solve()
{
    logging::out << "Running MDP engine " << get_engine_name() << "..."
                 << std::endl;
    utils::Timer total_timer;
    std::unique_ptr<engines::MDPEngineInterface<State>> engine(create_engine());

    const State initial_state = state_registry_.get_initial_state();

    value_type::value_t val = engine->solve(initial_state);
    progress_.print();
    total_timer.stop();

    logging::out << "analysis done. [t=" << utils::g_timer << "]" << std::endl;

    logging::out << std::endl;
    logging::print_analysis_result(
        val,
        engine->supports_error_bound(),
        engine->get_error(initial_state));

    logging::out << std::endl;
    logging::out << "State space interface:" << std::endl;
    logging::out << "  Registered state(s): " << state_registry_.size()
                 << std::endl;
    transition_generator_.print_statistics(logging::out);

    logging::out << std::endl;
    logging::out << "Engine " << get_engine_name()
                 << " statistics:" << std::endl;
    logging::out << "  Actual solver time: " << total_timer << std::endl;
    engine->print_statistics(logging::out);

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
