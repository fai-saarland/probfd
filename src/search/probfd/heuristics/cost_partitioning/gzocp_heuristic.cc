#include "probfd/heuristics/cost_partitioning/gzocp_heuristic.h"

#include "probfd/heuristics/pdbs/pattern_collection_information.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/task_evaluator_factory.h"
#include "probfd/value_type.h"

#include "downward/utils/collections.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/rng_options.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

namespace {
class ExplicitTaskCostFunction : public FDRSimpleCostFunction {
    ProbabilisticTaskProxy task_proxy;
    std::vector<value_t> costs;
    std::vector<std::set<int>> affected_vars;

public:
    ExplicitTaskCostFunction(ProbabilisticTaskProxy task_proxy)
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

GZOCPHeuristic::GZOCPHeuristic(
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

    ExplicitTaskCostFunction task_costs(task_proxy);

    const State& initial_state = task_proxy.get_initial_state();

    for (const Pattern& pattern : *patterns) {
        auto& pdb = pdbs.emplace_back(
            task_proxy,
            task_costs,
            pattern,
            initial_state,
            BlindEvaluator<AbstractStateIndex>(),
            false);
        task_costs.decrease_costs(pdb);
    }
}

value_t GZOCPHeuristic::evaluate(const State& state) const
{
    value_t value = 0.0_vt;

    for (const auto& pdb : pdbs) {
        value_t estimate = pdb.lookup_estimate(state);

        if (estimate == termination_cost) {
            return estimate;
        }

        value += estimate;
    }

    return value;
}

namespace {

class GZOCPHeuristicFactory : public TaskEvaluatorFactory {
    const utils::LogProxy log_;
    const std::shared_ptr<PatternCollectionGenerator>
        pattern_collection_generator_;
    const GZOCPHeuristic::OrderingStrategy ordering_;
    const std::shared_ptr<utils::RandomNumberGenerator> rng_;

public:
    explicit GZOCPHeuristicFactory(const plugins::Options& opts);

    std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

GZOCPHeuristicFactory::GZOCPHeuristicFactory(const plugins::Options& opts)
    : log_(utils::get_log_from_options(opts))
    , pattern_collection_generator_(
          opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"))
    , ordering_(opts.get<GZOCPHeuristic::OrderingStrategy>("order"))
    , rng_(utils::parse_rng_from_options(opts))
{
}

std::unique_ptr<FDREvaluator> GZOCPHeuristicFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function)
{
    return std::make_unique<GZOCPHeuristic>(
        task,
        task_cost_function,
        log_,
        pattern_collection_generator_,
        ordering_,
        rng_);
}

class GZOCPHeuristicFactoryFeature
    : public plugins::
          TypedFeature<TaskEvaluatorFactory, GZOCPHeuristicFactory> {
public:
    GZOCPHeuristicFactoryFeature()
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

static plugins::FeaturePlugin<GZOCPHeuristicFactoryFeature> _plugin;

static plugins::TypedEnumPlugin<GZOCPHeuristic::OrderingStrategy> _enum_plugin(
    {{"random", "the order is random"},
     {"size_asc", "orders the PDBs by increasing size"},
     {"size_desc", "orders the PDBs by decreasing size"},
     {"inherit",
      "inherits the order from the underlying pattern generation algorithm"}});

} // namespace
} // namespace pdbs
} // namespace heuristics
} // namespace probfd