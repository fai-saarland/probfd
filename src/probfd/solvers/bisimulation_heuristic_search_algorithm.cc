#include "probfd/solvers/bisimulation_heuristic_search_algorithm.h"

#include "downward/initial_state_values.h"
#include "probfd/algorithms/open_list.h"

#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/policies/empty_policy.h"

#include "probfd/tasks/determinization_task.h"

#include "probfd/utils/not_implemented.h"

#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/merge_and_shrink_representation.h"
#include "downward/merge_and_shrink/transition_system.h"

#include "downward/utils/timer.h"

using namespace downward;

namespace probfd::solvers {

void BisimulationTimer::print(std::ostream& out) const
{
    println(
        out,
        "  Bisimulation time: {}\n"
        "  Bisimilar states: {}\n"
        "  Transitions in bisimulation: {}",
        time,
        states,
        transitions);
}

BisimulationBasedHeuristicSearchAlgorithm ::
    BisimulationBasedHeuristicSearchAlgorithm(
        SharedProbabilisticTask task,
        std::string algorithm_name,
        std::shared_ptr<MDPAlgorithm<QState, QAction>> algorithm)
    : task_(std::move(task))
    , algorithm_name_(std::move(algorithm_name))
    , algorithm_(std::move(algorithm))
{
}

class BisimulationPolicy final : public Policy<State, OperatorID> {
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
    FDRHeuristic&,
    const State&,
    ProgressReport progress,
    downward::utils::Duration max_time) -> std::unique_ptr<PolicyType>
{
    utils::Timer timer;

    std::println(std::cout, "Building bisimulation...");

    SharedAbstractTask determinization =
        probfd::tasks::create_determinization_task(task_);

    auto [transition_system, state_mapping, distances] =
        bisimulation::compute_bisimulation_on_determinization(
            to_refs(determinization));

    std::println(std::cout, "Done.");

    if (!transition_system->is_solvable(*distances)) {
        println(std::cout, "Initial state recognized as unsolvable!");
        return std::make_unique<policies::EmptyPolicy<State, OperatorID>>();
    }

    const auto& init_vals = get_init(task_);

    State initial = init_vals.get_initial_state();
    initial.unpack();

    const auto initial_state = static_cast<bisimulation::QuotientState>(
        state_mapping->get_value(initial));

    bisimulation::BisimilarStateSpace state_space(task_, *transition_system);

    stats_.time = timer();
    stats_.states = state_space.num_bisimilar_states();
    stats_.transitions = state_space.num_transitions();

    println(
        std::cout,
        "Bisimulation built after {}\n"
        "Bisimilar state space contains {} states and {} transitions.",
        stats_.time,
        state_space.num_bisimilar_states(),
        state_space.num_transitions());

    const auto& operators = get_operators(task_);
    const auto& cost_function = get_cost_function(task_);
    const auto& termination_costs = get_termination_costs(task_);

    heuristics::BlindHeuristic<QState> heuristic(
        operators,
        cost_function,
        termination_costs);

    println(std::cout, "Running {}...", algorithm_name_);

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

    println(out);
    println(out, "Algorithm {} statistics:", algorithm_name_);
    algorithm_->print_statistics(out);
}

} // namespace probfd::solvers
