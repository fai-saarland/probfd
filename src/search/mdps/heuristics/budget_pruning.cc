#include "budget_pruning.h"

#include "../../global_state.h"
#include "../../heuristic.h"
#include "../../option_parser.h"
#include "../../plugin.h"
#include "../../state_registry.h"
#include "../../utils/hash.h"
#include "../analysis_objective.h"
#include "../globals.h"
#include "../logging.h"

namespace probabilistic {
namespace budget_pruning {

std::size_t
BudgetPruningHeuristic::Hash::operator()(const StateID& state_id) const
{
    const GlobalState s = registry_->lookup_state(::StateID(state_id));
    utils::HashState res;
    for (int var = g_initial_state_values.size() - g_steps_bounded - 1;
         var >= 0;
         --var) {
        utils::feed(res, s[var]);
    }
    return res.get_hash64();
}

bool
BudgetPruningHeuristic::Equal::operator()(const StateID& x, const StateID& y)
    const
{
    const GlobalState s = registry_->lookup_state(::StateID(x));
    const GlobalState t = registry_->lookup_state(::StateID(y));
    for (int var = g_initial_state_values.size() - g_steps_bounded - 1;
         var >= 0;
         --var) {
        if (s[var] != t[var]) {
            return false;
        }
    }
    return true;
}

BudgetPruningHeuristic::BudgetPruningHeuristic(
    value_type::value_t default_value,
    value_type::value_t dead_end_value,
    std::shared_ptr<Heuristic> pruning_function,
    bool cache_estimates)
    : default_value_(default_value)
    , dead_end_value_(dead_end_value)
    , pruning_function_(pruning_function)
    , cache_estimates_(cache_estimates)
{
}

BudgetPruningHeuristic::BudgetPruningHeuristic(const options::Options& opts)
    : BudgetPruningHeuristic(
        opts.get<bool>("pessimistic") ? g_analysis_objective->min()
                                      : g_analysis_objective->max(),
        g_analysis_objective->min(),
        opts.get<std::shared_ptr<Heuristic>>("heuristic"),
        opts.get<bool>("cache_estimates"))
{
}

void
BudgetPruningHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<Heuristic>>("heuristic");
    parser.add_option<bool>("pessimistic", "", "false");
    parser.add_option<bool>("cache_estimates", "", "true");
}

EvaluationResult
BudgetPruningHeuristic::evaluate(const GlobalState& state)
{
    int est = 0;
    if (cache_estimates_) {
        if (cached_estimates_ == nullptr) {
            cached_estimates_ = std::unique_ptr<
                std::unordered_map<probabilistic::StateID, int, Hash, Equal>>(
                new std::
                    unordered_map<probabilistic::StateID, int, Hash, Equal>(
                        1024,
                        Hash(&state.get_registry()),
                        Equal(&state.get_registry())));
        }
        auto res =
            cached_estimates_->emplace(StateID(state.get_id().hash()), 0);
        if (res.second) {
            pruning_function_->evaluate(state);
            res.first->second = pruning_function_->is_dead_end()
                ? -1
                : pruning_function_->get_value();
        }
        est = res.first->second;
    } else {
        pruning_function_->evaluate(state);
        est = pruning_function_->is_dead_end() ? -1
                                               : pruning_function_->get_value();
    }
    if (est == -1 || (g_steps_bounded && est > state[g_step_var])) {
        return EvaluationResult(true, dead_end_value_);
    }
    return EvaluationResult(false, default_value_);
}

void
BudgetPruningHeuristic::print_statistics() const
{
    pruning_function_->print_statistics();
    logging::out << "  Cached heuristic estimates: "
                 << (cached_estimates_ == nullptr ? std::size_t(0)
                                                  : cached_estimates_->size())
                 << std::endl;
}

static Plugin<GlobalStateEvaluator> _plugin(
    "prune_budget",
    options::parse<GlobalStateEvaluator, BudgetPruningHeuristic>);

} // namespace budget_pruning
} // namespace probabilistic
