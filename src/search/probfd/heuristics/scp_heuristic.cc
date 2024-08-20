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

using namespace probfd::pdbs;

namespace probfd::heuristics {

SCPHeuristicFactory::SCPHeuristicFactory(
    std::shared_ptr<PatternCollectionGenerator> pattern_collection_generator,
    SCPHeuristic::OrderingStrategy ordering,
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
    return std::make_unique<SCPHeuristic>(
        task,
        task_cost_function,
        utils::get_log_for_verbosity(verbosity_),
        pattern_collection_generator_,
        ordering_,
        utils::get_rng(random_seed_));
}

namespace {
class ExplicitTaskCostFunction : public FDRSimpleCostFunction {
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
    value_t get_non_goal_termination_cost() const override
    {
        return INFINITE_VALUE;
    }

    value_t& operator[](size_t i) { return costs[i]; }
    const value_t& operator[](size_t i) const { return costs[i]; }
};
} // namespace

SCPHeuristic::SCPHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function,
    utils::LogProxy log,
    std::shared_ptr<PatternCollectionGenerator> generator,
    OrderingStrategy order,
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : TaskDependentHeuristic(task, std::move(log))
    , termination_cost_(task_cost_function->get_non_goal_termination_cost())
    , ordering_(order)
    , rng_(rng)
{
    auto pattern_collection_info =
        generator->generate(task, task_cost_function);

    auto patterns = pattern_collection_info.get_patterns();

    pdbs_.reserve(patterns->size());

    switch (ordering_) {
    case RANDOM: rng->shuffle(*patterns); break;

    case SIZE_ASC:
        std::stable_sort(
            patterns->begin(),
            patterns->end(),
            [](const auto& left, const auto& right) {
                return left.size() < right.size();
            });
        break;

    case SIZE_DESC:
        std::stable_sort(
            patterns->begin(),
            patterns->end(),
            [](const auto& left, const auto& right) {
                return left.size() > right.size();
            });
        break;

    case INHERIT:
    default: break;
    }

    const size_t num_operators = task_proxy_.get_operators().size();

    auto task_costs = std::make_shared<ExplicitTaskCostFunction>(task_proxy_);
    std::vector<value_t> saturated_costs(num_operators);

    const State& initial_state = task_proxy_.get_initial_state();

    for (const Pattern& pattern : *patterns) {
        StateRankingFunction rankingf(task_proxy_.get_variables(), pattern);
        ProjectionStateSpace state_space(
            task_proxy_,
            task_costs,
            rankingf,
            false);
        const StateRank initial_state_rank =
            rankingf.get_abstract_rank(initial_state);

        auto& pdb = pdbs_.emplace_back(
            state_space,
            std::move(rankingf),
            initial_state_rank);

        compute_saturated_costs(
            state_space,
            pdb.get_value_table(),
            saturated_costs);

        auto& costs_ref = *task_costs;

        for (size_t i = 0; i != num_operators; ++i) {
            costs_ref[i] -= saturated_costs[i];
            assert(!is_approx_less(costs_ref[i], 0.0_vt));

            // Avoid floating point imprecision. The PDB implementation is not
            // stable with respect to action costs very close to zero.
            if (is_approx_equal(costs_ref[i], 0.0_vt, 0.0001)) {
                costs_ref[i] = 0.0_vt;
            }
        }
    }
}

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
