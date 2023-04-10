#include "probfd/heuristics/pdbs/incremental_ppdbs.h"

#include "probfd/heuristics/pdbs/max_orthogonal_finder.h"
#include "probfd/heuristics/pdbs/pattern_collection_information.h"
#include "probfd/heuristics/pdbs/probabilistic_pattern_database.h"

#include "pdbs/pattern_collection_information.h"

#include <limits>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {

unsigned long long compute_total_pdb_size(const PPDBCollection& pdbs)
{
    unsigned long long size = 0;

    for (const auto& pdb : pdbs) {
        size += pdb->num_states();
    }

    return size;
}

IncrementalPPDBs::IncrementalPPDBs(
    const ProbabilisticTaskProxy& task_proxy,
    TaskCostFunction* task_cost_function,
    const PatternCollection& initial_patterns,
    std::shared_ptr<SubCollectionFinder> subcollection_finder)
    : task_proxy(task_proxy)
    , task_cost_function(task_cost_function)
    , patterns(new PatternCollection(initial_patterns))
    , pattern_databases(new PPDBCollection())
    , pattern_subcollections(nullptr)
    , subcollection_finder(subcollection_finder)
    , size(0)
{
    pattern_databases->reserve(patterns->size());
    for (const Pattern& pattern : *patterns)
        add_pdb_for_pattern(pattern, task_proxy.get_initial_state());
    recompute_pattern_subcollections();
}

IncrementalPPDBs::IncrementalPPDBs(
    const ProbabilisticTaskProxy& task_proxy,
    TaskCostFunction* task_cost_function,
    PatternCollectionInformation& initial_patterns,
    std::shared_ptr<SubCollectionFinder> subcollection_finder)
    : task_proxy(task_proxy)
    , task_cost_function(task_cost_function)
    , patterns(initial_patterns.get_patterns())
    , pattern_databases(initial_patterns.get_pdbs())
    , pattern_subcollections(initial_patterns.get_subcollections())
    , subcollection_finder(subcollection_finder)
    , size(compute_total_pdb_size(*pattern_databases))
{
}

void IncrementalPPDBs::add_pdb_for_pattern(
    const Pattern& pattern,
    const State& initial_state)
{
    auto& pdb =
        pattern_databases->emplace_back(new ProbabilisticPatternDatabase(
            task_proxy,
            pattern,
            *task_cost_function,
            initial_state));
    size += pdb->num_states();
}

void IncrementalPPDBs::add_pdb(
    const shared_ptr<ProbabilisticPatternDatabase>& pdb)
{
    patterns->push_back(pdb->get_pattern());
    auto& new_pdb = pattern_databases->emplace_back(pdb);
    size += new_pdb->num_states();
    recompute_pattern_subcollections();
}

void IncrementalPPDBs::recompute_pattern_subcollections()
{
    pattern_subcollections =
        subcollection_finder->compute_subcollections(*patterns);
}

vector<PatternSubCollection>
IncrementalPPDBs::get_pattern_subcollections(const Pattern& new_pattern)
{
    return subcollection_finder->compute_subcollections_with_pattern(
        *patterns,
        *pattern_subcollections,
        new_pattern);
}

value_t IncrementalPPDBs::get_value(const State& state) const
{
    return evaluate(state).get_estimate();
}

EvaluationResult IncrementalPPDBs::evaluate(const State& state) const
{
    return subcollection_finder->evaluate(
        *pattern_databases,
        *pattern_subcollections,
        state);
}

value_t IncrementalPPDBs::evaluate_subcollection(
    const std::vector<value_t>& pdb_estimates,
    const std::vector<int>& subcollection) const
{
    return subcollection_finder->evaluate_subcollection(
        pdb_estimates,
        subcollection);
}

value_t IncrementalPPDBs::combine(value_t left, value_t right) const
{
    return subcollection_finder->combine(left, right);
}

bool IncrementalPPDBs::is_dead_end(const State& state) const
{
    for (const auto& pdb : *pattern_databases) {
        if (pdb->is_dead_end(state)) {
            return true;
        }
    }

    return false;
}

PatternCollectionInformation
IncrementalPPDBs::get_pattern_collection_information() const
{
    PatternCollectionInformation result(
        task_proxy,
        task_cost_function,
        patterns,
        subcollection_finder);
    result.set_pdbs(pattern_databases);
    result.set_subcollections(pattern_subcollections);
    return result;
}

std::shared_ptr<PPDBCollection> IncrementalPPDBs::get_pattern_databases() const
{
    return pattern_databases;
}

long long IncrementalPPDBs::get_size() const
{
    return size;
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd