#include "probfd/pdbs/pattern_collection_information.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/trivial_finder.h"

#include "probfd/cost_function.h"
#include "probfd/probabilistic_task.h"

#include "downward/pdbs/pattern_collection_information.h"

#include "downward/initial_state_values.h"
#include "downward/pdbs/pattern_database.h"

#include "downward/utils/collections.h"
#include "downward/utils/timer.h"

#include <cassert>
#include <iostream>
#include <utility>

using namespace std;
using namespace downward;

namespace probfd::pdbs {

PatternCollectionInformation::PatternCollectionInformation(
    SharedProbabilisticTask task,
    downward::pdbs::PatternCollectionInformation det_info,
    shared_ptr<SubCollectionFinder> arg_subcollection_finder)
    : task_(std::move(task))
    , patterns_(*det_info.get_patterns())
    , subcollection_finder_(std::move(arg_subcollection_finder))
    , h(get_operators(task_),
        get_cost_function(task_),
        get_termination_costs(task))

{
    const auto& variables = get_variables(task_);
    const auto& init_vals = get_init(task_);
    const auto& term_costs = get_termination_costs(task_);

    auto pdbs = det_info.get_pdbs();

    if (!pdbs) { return; }

    for (size_t i = 0; i != pdbs->size(); ++i) {
        auto& dpdb = *(*pdbs)[i];
        auto& pdb = pdbs_.emplace_back(
            std::make_shared<ProbabilityAwarePatternDatabase>(
                variables,
                dpdb.get_pattern()));

        const StateRankEvaluator& h =
            term_costs.get_non_goal_termination_cost() == INFINITE_VALUE
                ? static_cast<const StateRankEvaluator&>(PDBEvaluator(dpdb))
                : static_cast<const StateRankEvaluator&>(
                      DeadendPDBEvaluator(dpdb));
        const StateRank istate =
            pdb->get_abstract_state(init_vals.get_initial_state());
        compute_distances(*pdb, task_, istate, h);
    }
}

PatternCollectionInformation::PatternCollectionInformation(
    SharedProbabilisticTask task,
    PatternCollection patterns)
    : PatternCollectionInformation(
          task,
          std::move(patterns),
          make_shared<TrivialFinder>())
{
}

PatternCollectionInformation::PatternCollectionInformation(
    SharedProbabilisticTask task,
    PatternCollection patterns,
    shared_ptr<SubCollectionFinder> subcollection_finder)
    : task_(task)
    , patterns_(std::move(patterns))
    , subcollection_finder_(std::move(subcollection_finder))
    , h(get_operators(task_),
        get_cost_function(task_),
        get_termination_costs(task))
{
    assert(this->subcollection_finder_);
    // validate_and_normalize_patterns(*patterns);
}

void PatternCollectionInformation::create_pdbs_if_missing()
{
    const auto& variables = get_variables(task_);
    const auto& init_vals = get_init(task_);

    if (pdbs_.size() != patterns_.size()) {
        assert(pdbs_.empty());

        utils::Timer timer;
        cout << "Computing PDBs for pattern collection..." << endl;
        for (const Pattern& pattern : patterns_) {
            auto& pdb = pdbs_.emplace_back(
                std::make_unique<ProbabilityAwarePatternDatabase>(
                    variables,
                    pattern));
            const StateRank istate =
                pdb->get_abstract_state(init_vals.get_initial_state());
            compute_distances(*pdb, task_, istate, h);
        }
        cout << "Done computing PDBs for pattern collection: " << timer << endl;
    }
}

void PatternCollectionInformation::create_pattern_cliques_if_missing()
{
    if (subcollections_.empty()) {
        utils::Timer timer;
        cout << "Computing pattern cliques for pattern collection..." << endl;
        subcollections_ =
            subcollection_finder_->compute_subcollections(patterns_);
        cout << "Done computing pattern cliques for pattern collection: "
             << timer << endl;
    }
}

void PatternCollectionInformation::set_pdbs(PPDBCollection pdbs)
{
    pdbs_ = pdbs;
}

void PatternCollectionInformation::set_subcollections(
    vector<PatternSubCollection> subcollections)
{
    subcollections_ = subcollections;
}

const PatternCollection& PatternCollectionInformation::get_patterns() const
{
    return patterns_;
}

PPDBCollection& PatternCollectionInformation::get_pdbs()
{
    create_pdbs_if_missing();
    return pdbs_;
}

vector<PatternSubCollection>& PatternCollectionInformation::get_subcollections()
{
    create_pattern_cliques_if_missing();
    return subcollections_;
}

shared_ptr<SubCollectionFinder>
PatternCollectionInformation::get_subcollection_finder()
{
    return subcollection_finder_;
}

} // namespace probfd::pdbs
