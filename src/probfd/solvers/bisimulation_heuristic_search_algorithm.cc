#include "probfd/solvers/bisimulation_heuristic_search_algorithm.h"

#include "probfd/algorithms/open_list.h"

#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/policies/empty_policy.h"

#include "probfd/tasks/determinization_task.h"

#include "probfd/utils/not_implemented.h"

#include "probfd/task_proxy.h"

#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/merge_and_shrink_representation.h"
#include "downward/merge_and_shrink/transition_system.h"

#include "downward/utils/timer.h"

using namespace downward;

namespace probfd::solvers {

void BisimulationTimer::print(std::ostream& out) const
{
    out << "  Bisimulation time: " << time << std::endl;
    out << "  Bisimilar states: " << states << std::endl;
    out << "  Transitions in bisimulation: " << transitions << std::endl;
}

BisimulationBasedHeuristicSearchAlgorithm ::
    BisimulationBasedHeuristicSearchAlgorithm(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::string algorithm_name,
        std::shared_ptr<MDPAlgorithm<QState, QAction>> algorithm)
    : task_(std::move(task))
    , task_cost_function_(std::move(task_cost_function))
    , algorithm_name_(std::move(algorithm_name))
    , algorithm_(std::move(algorithm))
{
}

class BisimulationPolicy : public Policy<State, OperatorID> {
    std::unique_ptr<merge_and_shrink::MergeAndShrinkRepresentation>
        state_mapping;

    std::unique_ptr<Policy<bisimulation::QuotientState, OperatorID>> qpolicy;

public:
    BisimulationPolicy(
        std::unique_ptr<merge_and_shrink::MergeAndShrinkRepresentation>
            state_mapping,
        std::unique_ptr<Policy<bisimulation::QuotientState, OperatorID>>
            qpolicy)
        : state_mapping(std::move(state_mapping))
        , qpolicy(std::move(qpolicy))
    {
    }

    std::optional<PolicyDecision<OperatorID>>
    get_decision(const State& state) const override
    {
        const auto abstract_state = state_mapping->get_value(state);
        return qpolicy->get_decision(
            static_cast<bisimulation::QuotientState>(abstract_state));
    }
};

auto BisimulationBasedHeuristicSearchAlgorithm::compute_policy(
    FDRMDP&,
    FDREvaluator&,
    const State&,
    ProgressReport progress,
    double max_time) -> std::unique_ptr<PolicyType>
{
    utils::Timer timer;

    std::cout << "Building bisimulation..." << std::endl;

    ProbabilisticTaskProxy task_proxy(*task_);

    std::shared_ptr determinization =
        std::make_shared<tasks::DeterminizationTask>(task_);

    TaskProxy det_task_proxy(*determinization);

    auto [transition_system, state_mapping, distances] =
        bisimulation::compute_bisimulation_on_determinization(det_task_proxy);

    std::cout << "Done." << std::endl;

    if (!transition_system->is_solvable(*distances)) {
        std::cout << "Initial state recognized as unsolvable!" << std::endl;
        return std::make_unique<policies::EmptyPolicy<State, OperatorID>>();
    }

    State initial = task_proxy.get_initial_state();
    initial.unpack();
    const auto initial_state =
        bisimulation::QuotientState(state_mapping->get_value(initial));

    bisimulation::BisimilarStateSpace state_space(
        task_,
        task_cost_function_,
        *transition_system);

    stats_.time = timer();
    stats_.states = state_space.num_bisimilar_states();
    stats_.transitions = state_space.num_transitions();

    std::cout << "Bisimulation built after " << stats_.time << std::endl;
    std::cout << "Bisimilar state space contains "
              << state_space.num_bisimilar_states() << " states and "
              << state_space.num_transitions() << " transitions." << std::endl;
    std::cout << std::endl;

    ProbabilisticOperatorsProxy ops(*task_);
    heuristics::BlindEvaluator<QState> heuristic(ops, *task_cost_function_);

    std::cout << "Running " << algorithm_name_ << "..." << std::endl;
    auto pi = algorithm_->compute_policy(
        state_space,
        heuristic,
        initial_state,
        progress,
        max_time);

    return std::make_unique<BisimulationPolicy>(
        std::move(state_mapping),
        std::move(pi));
}

void BisimulationBasedHeuristicSearchAlgorithm::print_statistics(
    std::ostream& out) const
{
    stats_.print(out);

    out << std::endl;
    out << "Algorithm " << algorithm_name_ << " statistics:" << std::endl;
    algorithm_->print_statistics(out);
}

} // namespace probfd::solvers
