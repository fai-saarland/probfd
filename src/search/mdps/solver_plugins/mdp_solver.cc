#include "mdp_solver.h"

#include "../../globals.h"
#include "../../option_parser.h"
#include "../../utils/system.h"
#include "../../utils/timer.h"
#include "../globals.h"
#include "../logging.h"

#include <iomanip>

namespace probabilistic {
namespace solvers {

MDPSolver::MDPSolver(const options::Options& opts)
    : handler_([&opts]() {
        std::vector<std::shared_ptr<NewGlobalStateHandler>> handlers =
            opts.get_list<std::shared_ptr<NewGlobalStateHandler>>(
                "on_new_state");
        if (handlers.empty()) {
            return std::shared_ptr<NewGlobalStateHandlerList>(nullptr);
        }
        return std::make_shared<NewGlobalStateHandlerList>(handlers);
    }())
    , cache_aops_(opts.get<bool>("cache_aops"))
    , cache_transitions_(opts.get<bool>("cache_transitions"))
    , path_dependent_heuristics_(opts.get_list<std::shared_ptr<Heuristic>>(
          "path_dependent_heuristics"))
    , report_difference_(opts.get<double>("report_difference"))
{
}

void
MDPSolver::solve()
{
    utils::Timer total_timer;

    g_log << "Setting up model ..." << std::endl;

    StateRegistry state_registry(
        g_state_packer, ::g_axiom_evaluator, g_initial_state_values);
    algorithms::TransitionGenerator<GlobalState, const ProbabilisticOperator*>
        transition_generator(
            &state_registry,
            g_operators,
            path_dependent_heuristics_,
            g_step_cost_type,
            g_step_var,
            cache_transitions_);
    algorithms::
        ApplicableActionsGenerator<GlobalState, const ProbabilisticOperator*>
            aops_generator(
                cache_transitions_ ? &transition_generator : nullptr,
                g_successor_generator.get(),
                g_operators,
                g_step_var,
                g_step_cost_type,
                cache_aops_);
    algorithms::StateIDMap<GlobalState> state_id_map(&state_registry);
    GlobalStateEvaluator* state_reward =
        g_analysis_objective->state_evaluator();
    TransitionEvaluator* transition_reward =
        g_analysis_objective->transition_evaluator();

    ProgressReport report(report_difference_, g_log);
    report.register_print([&state_registry](std::ostream& out) {
        out << "registered=" << state_registry.size();
    });

    g_log << "set up model [t=" << total_timer << "]" << std::endl;

    g_log << "Creating engine '" << name() << "' ..." << std::endl;
    IMDPEngine<GlobalState>* engine = create_engine(
        &report,
        &state_id_map,
        &aops_generator,
        &transition_generator,
        g_analysis_objective->min(),
        g_analysis_objective->max(),
        state_reward,
        transition_reward);

    g_log << "engine '" << name() << "' has been set up [t=" << total_timer
          << "]" << std::endl;

    utils::Timer engine_timer;
    g_log << "Starting analysis ..." << std::endl;
    AnalysisResult result = engine->solve(state_registry.get_initial_state());
    g_log << name() << " terminated after " << engine_timer
          << " [t=" << total_timer << "]" << std::endl;

    delete (engine);
    delete (state_reward);
    delete (transition_reward);

    g_log << *result.statistics;

    this->print_additional_statistics(g_log);

    g_log << "**** General ****" << std::endl;
    g_log << std::setprecision(std::numeric_limits<long double>::digits10 + 1)
          << "Value computed for s0: " << result.result << std::endl;
    g_log << "Registered state(s): " << state_registry.size() << std::endl;
    aops_generator.get_statistics().print(g_log);
    transition_generator.get_statistics().print(g_log);
}

IMDPEngine<QuotientSystem::QState>*
MDPSolver::create_engine(
    QuotientSystemFunctionFactory&,
    QuotientSystem* /*quotient*/,
    ProgressReport* /*report*/,
    algorithms::StateIDMap<QuotientSystem::QState>* /*state_id_map*/,
    algorithms::ApplicableActionsGenerator<
        QuotientSystem::QState,
        QuotientSystem::QAction>* /*aops_generator*/,
    algorithms::TransitionGenerator<
        QuotientSystem::QState,
        QuotientSystem::QAction>* /*transition_generator*/,
    value_type::value_t /*minval*/,
    value_type::value_t /*maxval*/,
    algorithms::StateEvaluationFunction<
        QuotientSystem::QState>* /*state_reward*/,
    algorithms::TransitionRewardFunction<
        QuotientSystem::QState,
        QuotientSystem::QAction>* /*transition_reward*/)
{
    g_err << name() << " does not support the analysis of quotient systems"
          << std::endl;
    utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    return nullptr;
}

void
MDPSolver::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<bool>("cache_aops", "", "false");
    parser.add_option<bool>("cache_transitions", "", "false");
    parser.add_list_option<std::shared_ptr<Heuristic>>(
        "path_dependent_heuristics", "", "[]");
    parser.add_option<double>("report_difference", "", "1e-4");
    parser.add_list_option<std::shared_ptr<NewGlobalStateHandler>>(
        "on_new_state", "", "[]");
}

} // namespace solvers
} // namespace probabilistic
