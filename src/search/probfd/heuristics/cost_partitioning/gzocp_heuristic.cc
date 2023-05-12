#include "probfd/heuristics/cost_partitioning/gzocp_heuristic.h"

#include "probfd/heuristics/pdbs/probabilistic_pattern_database.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/value_type.h"

#include "utils/collections.h"

#include "option_parser.h"
#include "plugin.h"

#include "utils/rng_options.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

namespace {
class ExplicitTaskCostFunction : public TaskCostFunction {
    ProbabilisticTaskProxy task_proxy;
    std::vector<value_t> costs;
    std::vector<std::set<int>> affected_vars;

public:
    ExplicitTaskCostFunction(const ProbabilisticTaskProxy& task_proxy)
        : TaskCostFunction(0_vt, INFINITE_VALUE)
        , task_proxy(task_proxy)
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

    value_t get_action_cost(OperatorID op) { return costs[op.get_index()]; }

    bool is_goal(const State& state) const
    {
        return ::task_properties::is_goal_state(task_proxy, state);
    }

    void decrease_costs(const ProbabilisticPatternDatabase& pdb)
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

GZOCPHeuristic::GZOCPHeuristic(const options::Options& opts)
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
        StateRankingFunction rankingf(task_proxy, pattern);
        ProjectionStateSpace state_space(
            task_proxy,
            rankingf,
            task_costs,
            false);
        InducedProjectionCostFunction costs(task_proxy, rankingf, &task_costs);
        StateRank init_rank = rankingf.rank(initial_state);
        auto& pdb = pdbs.emplace_back(
            state_space,
            std::move(rankingf),
            costs,
            init_rank);

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

void GZOCPHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    TaskDependentHeuristic::add_options_to_parser(parser);

    parser.add_option<std::shared_ptr<PatternCollectionGenerator>>(
        "patterns",
        "",
        "det_adapter_ec(generator=systematic(pattern_max_size=2))");
    std::vector<std::string> names(
        {"random", "size_asc", "size_desc", "inherit"});
    parser.add_enum_option<OrderingStrategy>("order", names, "", "random");

    utils::add_rng_options(parser);
}

static Plugin<TaskEvaluator>
    _plugin("gzocp_heuristic", options::parse<TaskEvaluator, GZOCPHeuristic>);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd