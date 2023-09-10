#include "probfd/heuristics/pdbs/subcollection_combinator.h"

#include "probfd/heuristics/pdbs/dominance_pruning.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"

#include <numeric>

namespace probfd {
namespace heuristics {
namespace pdbs {

int SubCollectionCombinator::count_improvements_if_added(
    const PPDBCollection& pdbs,
    const ProbabilityAwarePatternDatabase& new_pdb,
    const std::vector<State>& states,
    value_t termination_cost)
{
    const std::vector<PatternSubCollection> subcollections =
        this->compute_subcollections_with_pdbs(pdbs, new_pdb);

    int count = 0;

    for (const auto& sample : states) {
        const value_t h_pattern = new_pdb.lookup_estimate(sample);

        if (h_pattern == termination_cost) {
            ++count;
            continue;
        }

        // h_collection: h-value of the current collection heuristic
        const value_t h_collection =
            this->evaluate(pdbs, sample, termination_cost);

        if (h_collection == termination_cost) continue;

        std::vector<value_t> h_values;
        h_values.reserve(pdbs.size());

        for (const auto& p : pdbs) {
            const value_t h = p->lookup_estimate(sample);
            if (h == termination_cost) goto next_sample;
            h_values.push_back(h);
        }

        for (const PatternSubCollection& subcollection : subcollections) {
            const value_t h_subcollection =
                this->evaluate_subcollection(h_values, subcollection);

            if (this->combine(h_subcollection, h_pattern) > h_collection) {
                /*
                  return true if a pattern clique is found for
                  which the condition is met
                */
                ++count;
                goto next_sample;
            }
        }

    next_sample:;
    }

    return count;
}

value_t SubCollectionCombinator::evaluate(
    const PPDBCollection& database,
    const State& state,
    value_t termination_cost)
{
    if (database.empty()) return 0_vt;

    // Get pattern estimates
    std::vector<value_t> estimates(database.size());
    for (std::size_t i = 0; i != database.size(); ++i) {
        const value_t estimate = database[i]->lookup_estimate(state);

        if (estimate == termination_cost) {
            return estimate;
        }

        estimates[i] = estimate;
    }

    // Get lowest additive subcollection value
    auto transformer = [&, this](const std::vector<int>& subcollection) {
        return this->evaluate_subcollection(estimates, subcollection);
    };

    return std::transform_reduce(
        subcollections_.begin(),
        subcollections_.end(),
        0_vt,
        static_cast<const value_t& (*)(const value_t&, const value_t&)>(
            std::max<value_t>),
        transformer);
}

void SubCollectionCombinator::print_statistics(utils::LogProxy log) const
{
    size_t total_subcollections_size = 0;

    for (auto subcollection : subcollections_) {
        total_subcollections_size += subcollection.size();
    }

    const double avg_subcollection_size =
        (double)total_subcollections_size / subcollections_.size();

    log << "Total number of subcollections: " << subcollections_.size() << "\n"
        << "Total number of subcollection PDBs: " << total_subcollections_size
        << "\n"
        << "Average size of subcollection PDBs: " << avg_subcollection_size
        << "\n";
}

double SubCollectionCombinator::run_dominance_pruning(
    ProbabilisticTaskProxy task_proxy,
    PPDBCollection& pdbs,
    double max_time,
    utils::LogProxy log)
{
    if (max_time > 0.0) {
        utils::Timer timer;

        prune_dominated_cliques(
            pdbs,
            subcollections_,
            task_proxy.get_variables().size(),
            max_time,
            log);

        return timer();
    }

    return 0.0;
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd