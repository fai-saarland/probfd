#include "probfd/heuristics/gzocp_heuristic.h"

#include "downward/initial_state_values.h"
#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/probabilistic_task.h"
#include "probfd/value_type.h"
#include "probfd/probabilistic_operator_space.h"

#include "downward/utils/collections.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/task_utils/task_properties.h"

#include <algorithm>
#include <iterator>
#include <set>
#include <utility>

using namespace downward;
using namespace probfd::pdbs;

namespace probfd::heuristics {

namespace {

class RunningCostFunction final
    : public downward::OperatorCostFunction<value_t> {
    std::vector<value_t> costs;
    std::vector<std::set<int>> affected_vars;

public:
    explicit RunningCostFunction(
        const ProbabilisticOperatorSpace& operators,
        const OperatorCostFunction<value_t>& cost_function)
    {
        costs.reserve(operators.size());
        affected_vars.reserve(operators.size());

        for (const ProbabilisticOperatorProxy op : operators) {
            costs.push_back(cost_function.get_operator_cost(op.get_id()));
            auto& var_set = affected_vars.emplace_back();
            task_properties::get_affected_vars(
                op,
                std::inserter(var_set, var_set.begin()));
        }
    }

    value_t get_operator_cost(int index) const override { return costs[index]; }

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

std::unique_ptr<FDRHeuristic>
GZOCPHeuristicFactory::create_object(const SharedProbabilisticTask& task)
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

    const auto& variables = get_variables(task);
    const auto& operators = get_operators(task);
    const auto& init_vals = get_init(task);
    const auto& cost_function =
        get_cost_function(task);
    const auto& term_costs = get_termination_costs(task);

    const State& init_state = init_vals.get_initial_state();

    const auto gzo_cost_function =
        std::make_shared<RunningCostFunction>(operators, cost_function);

    auto adapted = replace(task, gzo_cost_function);

    BlindEvaluator<StateRank> h(operators, *gzo_cost_function, term_costs);

    for (const Pattern& pattern : patterns) {
        auto& pdb = pdbs.emplace_back(variables, pattern);

        ProjectionStateSpace state_space(adapted, pdb.ranking_function, false);

        compute_distances(
            pdb,
            state_space,
            pdb.get_abstract_state(init_state),
            h);
        gzo_cost_function->decrease_costs(pdb);
    }

    return std::make_unique<GZOCPHeuristic>(
        term_costs.get_non_goal_termination_cost(),
        std::move(pdbs));
}

} // namespace probfd::heuristics
