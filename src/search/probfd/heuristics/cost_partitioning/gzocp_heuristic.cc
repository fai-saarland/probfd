#include "probfd/heuristics/cost_partitioning/gzocp_heuristic.h"

#include "probfd/heuristics/pdbs/pattern_collection_information.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/value_type.h"

#include "downward/utils/collections.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/rng_options.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

namespace {
class ExplicitTaskCostFunction : public TaskSimpleCostFunction {
    ProbabilisticTaskProxy task_proxy;
    std::vector<value_t> costs;
    std::vector<std::set<int>> affected_vars;

public:
    ExplicitTaskCostFunction(const ProbabilisticTaskProxy& task_proxy)
        : task_proxy(task_proxy)
    {
        const auto operators = task_proxy.get_operators();

        costs.reserve(operators.size());
        affected_vars.reserve(operators.size());

        for (const ProbabilisticOperatorProxy op : operators) {
            costs.push_back(op.get_cost());
            auto& var_set = affected_vars.emplace_back();
            task_properties::get_affected_vars(
                op,
                std::inserter(var_set, var_set.begin()));
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

    void decrease_costs(const ProbabilityAwarePatternDatabase& pdb)
    {
        for (size_t op_id = 0; op_id != costs.size(); ++op_id) {
            const bool affects_pdb = utils::have_common_element(
                pdb.get_pattern(),
                affected_vars[op_id]);

            if (affects_pdb) {
                costs[op_id] = 0;
            }
        }
    }

    value_t& operator[](size_t i) { return costs[i]; }
    const value_t& operator[](size_t i) const { return costs[i]; }
};
} // namespace

GZOCPHeuristic::GZOCPHeuristic(const plugins::Options& opts)
    : GZOCPHeuristic(
          opts.get<std::shared_ptr<ProbabilisticTask>>("transform"),
          utils::get_log_from_options(opts),
          opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"),
          opts.get<OrderingStrategy>("order"),
          utils::parse_rng_from_options(opts))
{
}

GZOCPHeuristic::GZOCPHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    utils::LogProxy log,
    std::shared_ptr<PatternCollectionGenerator> generator,
    OrderingStrategy order,
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : TaskDependentHeuristic(task, log)
    , ordering(order)
    , rng(rng)
{
    auto pattern_collection_info = generator->generate(task);

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

    ExplicitTaskCostFunction task_costs(task_proxy);

    const State& initial_state = task_proxy.get_initial_state();

    for (const Pattern& pattern : *patterns) {
        StateRankingFunction rankingf(task_proxy.get_variables(), pattern);
        ProjectionStateSpace state_space(
            task_proxy,
            rankingf,
            task_costs,
            false);
        StateRank init_rank = rankingf.get_abstract_rank(initial_state);
        auto& pdb =
            pdbs.emplace_back(state_space, std::move(rankingf), init_rank);

        task_costs.decrease_costs(pdb);
    }
}

EvaluationResult GZOCPHeuristic::evaluate(const State& state) const
{
    value_t value = 0.0_vt;

    for (const auto& pdb : pdbs) {
        auto eval_result = pdb.evaluate(state);

        if (eval_result.is_unsolvable()) {
            return eval_result;
        }

        value += eval_result.get_estimate();
    }

    return EvaluationResult{false, value};
}

class GZOCPHeuristicFeature
    : public plugins::TypedFeature<TaskEvaluator, GZOCPHeuristic> {
public:
    GZOCPHeuristicFeature()
        : TypedFeature("gzocp_heuristic")
    {
        TaskDependentHeuristic::add_options_to_feature(*this);
        utils::add_rng_options(*this);

        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "The pattern generation algorithm.",
            "det_adapter_ec(generator=systematic(pattern_max_size=2))");
        add_option<GZOCPHeuristic::OrderingStrategy>(
            "order",
            "The order in which patterns are considered",
            "random");
    }
};

static plugins::FeaturePlugin<GZOCPHeuristicFeature> _plugin;

static plugins::TypedEnumPlugin<GZOCPHeuristic::OrderingStrategy> _enum_plugin(
    {{"random", "the order is random"},
     {"size_asc", "orders the PDBs by increasing size"},
     {"size_desc", "orders the PDBs by decreasing size"},
     {"inherit",
      "inherits the order from the underlying pattern generation algorithm"}});

} // namespace pdbs
} // namespace heuristics
} // namespace probfd