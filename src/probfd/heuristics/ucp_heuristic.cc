#include "probfd/heuristics/ucp_heuristic.h"

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/task_evaluator_factory.h"
#include "probfd/value_type.h"

#include "downward/task_utils/task_properties.h"

#include <utility>

using namespace probfd::pdbs;

namespace probfd::heuristics {

namespace {
class UniformTaskCostFunction : public FDRSimpleCostFunction {
    ProbabilisticTaskProxy task_proxy;
    std::vector<value_t> costs;

public:
    UniformTaskCostFunction(
        const ProbabilisticTaskProxy& task_proxy,
        size_t num_abstractions)
        : task_proxy(task_proxy)
    {
        const auto operators = task_proxy.get_operators();
        costs.reserve(operators.size());

        for (const ProbabilisticOperatorProxy op : operators) {
            costs.push_back(
                static_cast<value_t>(op.get_cost()) /
                static_cast<value_t>(num_abstractions));
        }
    }

    value_t get_action_cost(OperatorID op) override
    {
        return costs[op.get_index()];
    }

    bool is_goal(const State& state) const override
    {
        return ::task_properties::is_goal_state(task_proxy, state);
    }

    value_t get_non_goal_termination_cost() const override
    {
        return INFINITE_VALUE;
    }
};
} // namespace

UCPHeuristic::UCPHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function,
    utils::LogProxy log,
    std::shared_ptr<PatternCollectionGenerator> generator)
    : TaskDependentHeuristic(task, std::move(log))
    , termination_cost_(task_cost_function->get_non_goal_termination_cost())
{
    auto pattern_collection_info =
        generator->generate(task, task_cost_function);

    auto patterns = pattern_collection_info.get_patterns();

    const size_t num_abstractions = patterns->size();

    pdbs_.reserve(num_abstractions);

    auto task_costs = std::make_shared<UniformTaskCostFunction>(
        task_proxy_,
        num_abstractions);

    const State& initial_state = task_proxy_.get_initial_state();

    heuristics::BlindEvaluator<StateRank> h(
        task_proxy_.get_operators(),
        *task_cost_function);

    for (const Pattern& pattern : *patterns) {
        auto& pdb = pdbs_.emplace_back(task_proxy_.get_variables(), pattern);
        const StateRank init_rank = pdb.get_abstract_state(initial_state);
        compute_distances(pdb, task_proxy_, task_costs, init_rank, h);
    }
}

UCPHeuristic::~UCPHeuristic() = default;

value_t UCPHeuristic::evaluate(const State& state) const
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

UCPHeuristicFactory::UCPHeuristicFactory(
    utils::Verbosity verbosity,
    std::shared_ptr<PatternCollectionGenerator> generator)
    : verbosity_(verbosity)
    , pattern_collection_generator_(std::move(generator))
{
}

std::unique_ptr<FDREvaluator> UCPHeuristicFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function)
{
    return std::make_unique<UCPHeuristic>(
        task,
        task_cost_function,
        utils::get_log_for_verbosity(verbosity_),
        pattern_collection_generator_);
}

} // namespace probfd::heuristics
