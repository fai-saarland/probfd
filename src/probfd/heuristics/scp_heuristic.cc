#include "probfd/heuristics/scp_heuristic.h"

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/saturation.h"

#include "probfd/value_type.h"

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/task_utils/task_properties.h"

#include <algorithm>
#include <cassert>
#include <utility>

using namespace downward;
using namespace probfd::pdbs;

namespace probfd::heuristics {

namespace {
class ExplicitTaskCostFunction final : public FDRSimpleCostFunction {
    ProbabilisticTaskProxy task_proxy;
    std::vector<value_t> costs;

public:
    explicit ExplicitTaskCostFunction(const ProbabilisticTaskProxy& task_proxy)
        : task_proxy(task_proxy)
    {
        const auto operators = task_proxy.get_operators();
        costs.reserve(operators.size());

        for (const ProbabilisticOperatorProxy op : operators) {
            costs.push_back(op.get_cost());
        }
    }

    value_t get_action_cost(OperatorID op) override
    {
        return costs[op.get_index()];
    }

    [[nodiscard]]
    bool is_goal(const State& state) const override
    {
        return ::task_properties::is_goal_state(task_proxy, state);
    }

    [[nodiscard]]
    value_t get_goal_termination_cost() const override
    {
        return 0_vt;
    }

    [[nodiscard]]
    value_t get_non_goal_termination_cost() const override
    {
        return INFINITE_VALUE;
    }

    value_t& operator[](size_t i) { return costs[i]; }
    const value_t& operator[](size_t i) const { return costs[i]; }
};
} // namespace

SCPHeuristicFactory::SCPHeuristicFactory(
    std::shared_ptr<PatternCollectionGenerator> pattern_collection_generator,
    OrderingStrategy ordering,
    int random_seed,
    utils::Verbosity verbosity)
    : pattern_collection_generator_(std::move(pattern_collection_generator))
    , ordering_(ordering)
    , random_seed_(random_seed)
    , verbosity_(verbosity)
{
}

std::unique_ptr<FDREvaluator> SCPHeuristicFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function)
{
    ProbabilisticTaskProxy task_proxy(*task);

    auto pattern_collection_info =
        pattern_collection_generator_->generate(task, task_cost_function);

    auto patterns = pattern_collection_info.get_patterns();

    std::vector<ProbabilityAwarePatternDatabase> pdbs;
    pdbs.reserve(patterns.size());

    const std::shared_ptr<utils::RandomNumberGenerator> rng =
        utils::get_rng(random_seed_);

    switch (ordering_) {
    case RANDOM: rng->shuffle(patterns); break;

    case SIZE_ASC:
        std::ranges::stable_sort(patterns, std::less<>(), &Pattern::size);
        break;

    case SIZE_DESC:
        std::ranges::stable_sort(patterns, std::greater<>(), &Pattern::size);
        break;

    case INHERIT:
    default: break;
    }

    const size_t num_operators = task_proxy.get_operators().size();

    auto task_costs = std::make_shared<ExplicitTaskCostFunction>(task_proxy);
    std::vector<value_t> saturated_costs(num_operators);

    const State& initial_state = task_proxy.get_initial_state();

    BlindEvaluator<StateRank> h(
        task_proxy.get_operators(),
        *task_cost_function);

    for (const Pattern& pattern : patterns) {
        auto& pdb = pdbs.emplace_back(task_proxy.get_variables(), pattern);

        ProjectionStateSpace state_space(
            task_proxy,
            task_costs,
            pdb.ranking_function,
            false);

        compute_distances(
            pdb,
            state_space,
            pdb.get_abstract_state(initial_state),
            h);

        compute_saturated_costs(state_space, pdb.value_table, saturated_costs);

        auto& costs_ref = *task_costs;

        for (size_t i = 0; i != num_operators; ++i) {
            costs_ref[i] -= saturated_costs[i];
            assert(!is_approx_less(costs_ref[i], 0.0_vt, 0.0001));

            // Avoid floating point imprecision. The PDB implementation is not
            // stable with respect to action costs very close to zero.
            if (is_approx_equal(costs_ref[i], 0.0_vt, 0.0001)) {
                costs_ref[i] = 0.0_vt;
            }
        }
    }

    return std::make_unique<SCPHeuristic>(
        task_cost_function->get_non_goal_termination_cost(),
        std::move(pdbs));
}

SCPHeuristic::SCPHeuristic(
    value_t termination_cost,
    std::vector<ProbabilityAwarePatternDatabase> pdbs)
    : termination_cost_(termination_cost)
    , pdbs_(std::move(pdbs))
{
}

SCPHeuristic::~SCPHeuristic() = default;

value_t SCPHeuristic::evaluate(const State& state) const
{
    value_t value = 0.0_vt;

    for (const auto& pdb : pdbs_) {
        const value_t estimate = pdb.lookup_estimate(state);

        if (estimate == termination_cost_) {
            return estimate;
        }

        value += estimate;
    }

    return value;
}

} // namespace probfd::heuristics
