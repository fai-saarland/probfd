#include "probfd/heuristics/cost_partitioning/ucp_heuristic.h"

#include "probfd/heuristics/pdbs/probabilistic_pattern_database.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/value_type.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

namespace {
class UniformTaskCostFunction : public TaskCostFunction {
    ProbabilisticTaskProxy task_proxy;
    std::vector<value_t> costs;

public:
    UniformTaskCostFunction(
        const ProbabilisticTaskProxy& task_proxy,
        size_t num_abstractions)
        : TaskCostFunction(0_vt, INFINITE_VALUE)
        , task_proxy(task_proxy)
    {
        const auto operators = task_proxy.get_operators();
        costs.reserve(operators.size());

        for (const ProbabilisticOperatorProxy op : operators) {
            costs.push_back(
                static_cast<value_t>(op.get_cost()) / num_abstractions);
        }
    }

    value_t get_action_cost(OperatorID op) { return costs[op.get_index()]; }

    bool is_goal(const State& state) const
    {
        return ::task_properties::is_goal_state(task_proxy, state);
    }
};
} // namespace

UCPHeuristic::UCPHeuristic(const options::Options& opts)
    : UCPHeuristic(
          opts.get<std::shared_ptr<ProbabilisticTask>>("transform"),
          utils::get_log_from_options(opts),
          opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"))
{
}

UCPHeuristic::UCPHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    utils::LogProxy log,
    std::shared_ptr<PatternCollectionGenerator> generator)
    : TaskDependentHeuristic(task, log)
{
    auto pattern_collection_info = generator->generate(task);

    auto patterns = pattern_collection_info.get_patterns();

    const size_t num_abstractions = patterns->size();

    pdbs.reserve(num_abstractions);

    UniformTaskCostFunction task_costs(task_proxy, num_abstractions);

    const State& initial_state = task_proxy.get_initial_state();

    for (const Pattern& pattern : *patterns) {
        StateRankingFunction rankingf(task_proxy, pattern);
        ProjectionStateSpace state_space(
            task_proxy,
            rankingf,
            task_costs,
            true);
        InducedProjectionCostFunction costs(task_proxy, rankingf, &task_costs);
        StateRank init_rank = rankingf.rank(initial_state);
        pdbs.emplace_back(state_space, std::move(rankingf), costs, init_rank);
    }
}

EvaluationResult UCPHeuristic::evaluate(const State& state) const
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

void UCPHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    TaskDependentHeuristic::add_options_to_parser(parser);

    parser.add_option<std::shared_ptr<PatternCollectionGenerator>>(
        "patterns",
        "",
        "det_adapter_ec(generator=systematic(pattern_max_size=2))");
}

static Plugin<TaskEvaluator>
    _plugin("ucp_heuristic", options::parse<TaskEvaluator, UCPHeuristic>);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd