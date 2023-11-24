#include "probfd/heuristics/ucp_heuristic.h"

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/task_evaluator_factory.h"
#include "probfd/value_type.h"

#include "downward/plugins/plugin.h"

using namespace probfd::pdbs;

namespace probfd {
namespace heuristics {

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
                static_cast<value_t>(op.get_cost()) / num_abstractions);
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
    : TaskDependentHeuristic(task, log)
    , termination_cost(task_cost_function->get_non_goal_termination_cost())
{
    auto pattern_collection_info =
        generator->generate(task, task_cost_function);

    auto patterns = pattern_collection_info.get_patterns();

    const size_t num_abstractions = patterns->size();

    pdbs.reserve(num_abstractions);

    UniformTaskCostFunction task_costs(task_proxy, num_abstractions);

    const State& initial_state = task_proxy.get_initial_state();

    for (const Pattern& pattern : *patterns) {
        StateRankingFunction rankingf(task_proxy.get_variables(), pattern);
        ProjectionStateSpace state_space(
            task_proxy,
            task_costs,
            rankingf,
            true);
        StateRank init_rank = rankingf.get_abstract_rank(initial_state);
        pdbs.emplace_back(state_space, std::move(rankingf), init_rank);
    }
}

value_t UCPHeuristic::evaluate(const State& state) const
{
    value_t value = 0.0_vt;

    for (const auto& pdb : pdbs) {
        const value_t estimate = pdb.lookup_estimate(state);

        if (estimate == termination_cost) {
            return estimate;
        }

        value += estimate;
    }

    return value;
}

namespace {

class UCPHeuristicFactory : public TaskEvaluatorFactory {
    const utils::LogProxy log_;
    const std::shared_ptr<PatternCollectionGenerator>
        pattern_collection_generator_;

public:
    explicit UCPHeuristicFactory(const plugins::Options& opts);

    std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

UCPHeuristicFactory::UCPHeuristicFactory(const plugins::Options& opts)
    : log_(utils::get_log_from_options(opts))
    , pattern_collection_generator_(
          opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"))
{
}

std::unique_ptr<FDREvaluator> UCPHeuristicFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function)
{
    return std::make_unique<UCPHeuristic>(
        task,
        task_cost_function,
        log_,
        pattern_collection_generator_);
}

class UCPHeuristicFactoryFeature
    : public plugins::TypedFeature<TaskEvaluatorFactory, UCPHeuristicFactory> {
public:
    UCPHeuristicFactoryFeature()
        : TypedFeature("ucp_heuristic")
    {
        TaskDependentHeuristic::add_options_to_feature(*this);
        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "The pattern generation algorithm.",
            "det_adapter_ec(generator=systematic(pattern_max_size=2))");
    }
};

static plugins::FeaturePlugin<UCPHeuristicFactoryFeature> _plugin;

} // namespace

} // namespace heuristics
} // namespace probfd