#include "probfd/heuristics/gzocp_heuristic.h"

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/probabilistic_task.h"
#include "probfd/value_type.h"

#include "downward/utils/collections.h"

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/task_utils/task_properties.h"
#include "probfd/tasks/delegating_task.h"

#include <algorithm>
#include <iterator>
#include <set>
#include <utility>

using namespace downward;
using namespace probfd::pdbs;

namespace probfd::heuristics {

namespace {
class AdaptedTask final : public tasks::DelegatingTask {
    std::vector<value_t> costs;
    std::vector<std::set<int>> affected_vars;

public:
    explicit AdaptedTask(std::shared_ptr<ProbabilisticTask> task)
        : DelegatingTask(std::move(task))
    {
        const auto operators = task->get_operators();

        costs.reserve(operators.size());
        affected_vars.reserve(operators.size());

        for (const ProbabilisticOperatorProxy op : operators) {
            costs.push_back(task->get_operator_cost(op.get_id()));
            auto& var_set = affected_vars.emplace_back();
            task_properties::get_affected_vars(
                op,
                std::inserter(var_set, var_set.begin()));
        }
    }

    void decrease_costs(const ProbabilityAwarePatternDatabase& pdb)
    {
        for (size_t op_id = 0; op_id != costs.size(); ++op_id) {
            const bool affects_pdb = utils::have_common_element(
                pdb.get_pattern(),
                affected_vars[op_id]);

            if (affects_pdb) { costs[op_id] = 0; }
        }
    }

    value_t& operator[](size_t i) { return costs[i]; }

    const value_t& operator[](size_t i) const { return costs[i]; }
};
} // namespace

GZOCPHeuristic::GZOCPHeuristic(
    value_t termination_cost,
    std::vector<ProbabilityAwarePatternDatabase> pdbs)
    : termination_cost_(termination_cost)
    , pdbs_(std::move(pdbs))
{
}

GZOCPHeuristic::~GZOCPHeuristic() = default;

value_t GZOCPHeuristic::evaluate(const State& state) const
{
    value_t value = 0.0_vt;

    for (const auto& pdb : pdbs_) {
        const value_t estimate = pdb.lookup_estimate(state);

        if (estimate == termination_cost_) { return estimate; }

        value += estimate;
    }

    return value;
}

GZOCPHeuristicFactory::GZOCPHeuristicFactory(
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

std::unique_ptr<FDREvaluator>
GZOCPHeuristicFactory::create_heuristic(std::shared_ptr<ProbabilisticTask> task)
{
    auto pattern_collection_info =
        pattern_collection_generator_->generate(task);

    auto patterns = pattern_collection_info.get_patterns();

    std::vector<ProbabilityAwarePatternDatabase> pdbs;
    pdbs.reserve(patterns.size());

    const auto rng = utils::get_rng(random_seed_);

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

    const State& init_state = task->get_initial_state();

    BlindEvaluator<StateRank> h(task->get_operators(), *task, *task);

    const auto adapted = std::make_shared<AdaptedTask>(task);

    for (const Pattern& pattern : patterns) {
        auto& pdb = pdbs.emplace_back(task->get_variables(), pattern);

        ProjectionStateSpace state_space(adapted, pdb.ranking_function, false);

        compute_distances(
            pdb,
            state_space,
            pdb.get_abstract_state(init_state),
            h);
        adapted->decrease_costs(pdb);
    }

    return std::make_unique<GZOCPHeuristic>(
        task->get_non_goal_termination_cost(),
        std::move(pdbs));
}

} // namespace probfd::heuristics
