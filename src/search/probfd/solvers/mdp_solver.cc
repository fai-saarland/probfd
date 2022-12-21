#include "probfd/solvers/mdp_solver.h"

#include "probfd/analysis_objectives/analysis_objective.h"

#include "probfd/utils/logging.h"

#include "probfd/globals.h"

#include "utils/timer.h"

#include "operator_cost.h"
#include "option_parser.h"
#include "plugin.h"

#include "legacy/globals.h"
#include "legacy/heuristic.h"

#include <iomanip>
#include <vector>

namespace probfd {
namespace solvers {

MDPSolver::MDPSolver(const options::Options& opts)
    : progress_(
          opts.get<double>("report_epsilon"),
          std::cout,
          opts.get<bool>("report_enabled"))
    , state_registry_(
          g_state_packer,
          legacy::g_axiom_evaluator,
          g_initial_state_values)
    , state_id_map_(&state_registry_)
    , action_id_map_(g_operators)
    , reward_function_(g_analysis_objective->reward())
    , transition_generator_(
          g_operators,
          g_successor_generator.get(),
          g_step_var,
          g_step_cost_type,
          &state_registry_,
          opts.get<bool>("cache"),
          opts.get_list<std::shared_ptr<legacy::Heuristic>>(
              "path_dependent_heuristics"))
{
    legacy::StateRegistry* state_registry = &state_registry_;
    progress_.register_print([state_registry](std::ostream& out) {
        out << "registered=" << state_registry->size();
    });
}

void MDPSolver::solve()
{
    logging::out << "Running MDP engine " << get_engine_name() << "..."
                 << std::endl;
    utils::Timer total_timer;
    std::unique_ptr<engines::MDPEngineInterface<legacy::GlobalState>> engine(
        create_engine());

    const legacy::GlobalState initial_state =
        state_registry_.get_initial_state();

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
    parser.add_list_option<std::shared_ptr<legacy::Heuristic>>(
        "path_dependent_heuristics",
        "",
        "[]");
    parser.add_option<double>("report_epsilon", "", "1e-4");
    parser.add_option<bool>("report_enabled", "", "true");
}

} // namespace solvers
} // namespace probfd
