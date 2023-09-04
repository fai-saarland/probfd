#include "probfd/heuristics/cost_partitioning/scp_heuristic.h"

#include "probfd/heuristics/pdbs/pattern_collection_information.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/task_evaluator_factory.h"
#include "probfd/value_type.h"

#include "downward/utils/rng_options.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

namespace {
class ExplicitTaskCostFunction : public FDRSimpleCostFunction {
    ProbabilisticTaskProxy task_proxy;
    std::vector<value_t> costs;

public:
    ExplicitTaskCostFunction(const ProbabilisticTaskProxy& task_proxy)
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

    bool is_goal(const State& state) const override
    {
        return ::task_properties::is_goal_state(task_proxy, state);
    }

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
    : TaskDependentHeuristic(task, log)
    , termination_cost(task_cost_function->get_non_goal_termination_cost())
    , ordering(order)
    , rng(rng)
{
    auto pattern_collection_info =
        generator->generate(task, task_cost_function);

    auto patterns = pattern_collection_info.get_patterns();

    pdbs.reserve(patterns->size());

    switch (ordering) {
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

    const size_t num_operators = task_proxy.get_operators().size();

    ExplicitTaskCostFunction task_costs(task_proxy);

    std::vector<value_t> saturated_costs(num_operators);

    const State& initial_state = task_proxy.get_initial_state();

    for (const Pattern& pattern : *patterns) {
        StateRankingFunction rankingf(task_proxy.get_variables(), pattern);
        ProjectionStateSpace state_space(
            task_proxy,
            task_costs,
            rankingf,
            false);
        const StateRank initial_state_rank =
            rankingf.get_abstract_rank(initial_state);

        auto& pdb = pdbs.emplace_back(
            state_space,
            std::move(rankingf),
            initial_state_rank);

        compute_saturated_costs(
            state_space,
            pdb.get_value_table(),
            saturated_costs);

        for (size_t i = 0; i != num_operators; ++i) {
            task_costs[i] -= saturated_costs[i];
            assert(!is_approx_less(task_costs[i], 0.0_vt));

            // Avoid floating point imprecision. The PDB implementation is not
            // stable with respect to action costs very close to zero.
            if (is_approx_equal(task_costs[i], 0.0_vt, 0.0001)) {
                task_costs[i] = 0.0_vt;
            }
        }
    }
}

value_t SCPHeuristic::evaluate(const State& state) const
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

class SCPHeuristicFactory : public TaskEvaluatorFactory {
    const utils::LogProxy log_;
    const std::shared_ptr<PatternCollectionGenerator>
        pattern_collection_generator_;
    const SCPHeuristic::OrderingStrategy ordering_;
    const std::shared_ptr<utils::RandomNumberGenerator> rng_;

public:
    explicit SCPHeuristicFactory(const plugins::Options& opts);

    std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

SCPHeuristicFactory::SCPHeuristicFactory(const plugins::Options& opts)
    : log_(utils::get_log_from_options(opts))
    , pattern_collection_generator_(
          opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"))
    , ordering_(opts.get<SCPHeuristic::OrderingStrategy>("order"))
    , rng_(utils::parse_rng_from_options(opts))
{
}

std::unique_ptr<FDREvaluator> SCPHeuristicFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function)
{
    return std::make_unique<SCPHeuristic>(
        task,
        task_cost_function,
        log_,
        pattern_collection_generator_,
        ordering_,
        rng_);
}

class SCPHeuristicFactoryFeature
    : public plugins::TypedFeature<TaskEvaluatorFactory, SCPHeuristicFactory> {
public:
    SCPHeuristicFactoryFeature()
        : TypedFeature("scp_heuristic")
    {
        TaskDependentHeuristic::add_options_to_feature(*this);
        utils::add_rng_options(*this);

        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "The pattern generation algorithm.",
            "det_adapter_ec(generator=systematic(pattern_max_size=2))");
        add_option<SCPHeuristic::OrderingStrategy>(
            "order",
            "The order in which patterns are considered",
            "random");
    }
};

static plugins::FeaturePlugin<SCPHeuristicFactoryFeature> _plugin;

static plugins::TypedEnumPlugin<SCPHeuristic::OrderingStrategy> _enum_plugin(
    {{"random", "the order is random"},
     {"size_asc", "orders the PDBs by increasing size"},
     {"size_desc", "orders the PDBs by decreasing size"},
     {"inherit",
      "inherits the order from the underlying pattern generation algorithm"}});

} // namespace
} // namespace pdbs
} // namespace heuristics
} // namespace probfd