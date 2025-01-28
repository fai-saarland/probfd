#include "probfd/pdbs/pattern_collection_information.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/trivial_finder.h"

#include "probfd/cost_function.h"

#include "downward/pdbs/pattern_collection_information.h"
#include "downward/pdbs/pattern_database.h"

#include "downward/utils/collections.h"
#include "downward/utils/timer.h"

#include <cassert>
#include <iostream>
#include <utility>

using namespace std;

namespace probfd::pdbs {

PatternCollectionInformation::PatternCollectionInformation(
    const ProbabilisticTaskProxy& arg_task_proxy,
    std::shared_ptr<FDRCostFunction> arg_task_cost_function,
    ::pdbs::PatternCollectionInformation det_info,
    shared_ptr<SubCollectionFinder> arg_subcollection_finder)
    : task_proxy_(arg_task_proxy)
    , task_cost_function_(std::move(arg_task_cost_function))
    , patterns_(det_info.get_patterns())
    , subcollection_finder_(std::move(arg_subcollection_finder))
{
    auto pdbs = det_info.get_pdbs();

    if (!pdbs) {
        return;
    }

    pdbs_ = make_shared<PPDBCollection>();

    for (size_t i = 0; i != pdbs->size(); ++i) {
        auto& dpdb = *(*pdbs)[i];
        auto& pdb = pdbs_->emplace_back(
            std::make_shared<ProbabilityAwarePatternDatabase>(
                task_proxy_.get_variables(),
                dpdb.get_pattern()));

        const StateRankEvaluator& h =
            task_cost_function_->get_non_goal_termination_cost() ==
                    INFINITE_VALUE
                ? static_cast<const StateRankEvaluator&>(PDBEvaluator(dpdb))
                : static_cast<const StateRankEvaluator&>(
                      DeadendPDBEvaluator(dpdb));
        const StateRank istate =
            pdb->get_abstract_state(task_proxy_.get_initial_state());
        compute_distances(*pdb, task_proxy_, task_cost_function_, istate, h);
    }
}

PatternCollectionInformation::PatternCollectionInformation(
    const ProbabilisticTaskProxy& task_proxy,
    std::shared_ptr<FDRCostFunction> task_cost_function,
    shared_ptr<PatternCollection> patterns)
    : PatternCollectionInformation(
          task_proxy,
          std::move(task_cost_function),
          std::move(patterns),
          make_shared<TrivialFinder>())
{
}

PatternCollectionInformation::PatternCollectionInformation(
    const ProbabilisticTaskProxy& task_proxy,
    std::shared_ptr<FDRCostFunction> task_cost_function,
    shared_ptr<PatternCollection> patterns,
    shared_ptr<SubCollectionFinder> subcollection_finder)
    : task_proxy_(task_proxy)
    , task_cost_function_(std::move(task_cost_function))
    , patterns_(std::move(patterns))
    , subcollection_finder_(std::move(subcollection_finder))
{
    assert(this->patterns_);
    assert(this->subcollection_finder_);
    // validate_and_normalize_patterns(*patterns);
}

bool PatternCollectionInformation::information_is_valid() const
{
    if (!patterns_) {
        return false;
    }
    int num_patterns = patterns_->size();
    if (pdbs_) {
        if (patterns_->size() != pdbs_->size()) {
            return false;
        }
        for (int i = 0; i < num_patterns; ++i) {
            if ((*patterns_)[i] != (*pdbs_)[i]->get_pattern()) {
                return false;
            }
        }
    }
    if (subcollections_) {
        for (const PatternSubCollection& clique : *subcollections_) {
            for (PatternID pattern_id : clique) {
                if (!utils::in_bounds(pattern_id, *patterns_)) {
                    return false;
                }
            }
        }
    }
    return true;
}

void PatternCollectionInformation::create_pdbs_if_missing()
{
    assert(patterns_);
    if (!pdbs_) {
        utils::Timer timer;
        cout << "Computing PDBs for pattern collection..." << endl;
        pdbs_ = make_shared<PPDBCollection>();
        for (const Pattern& pattern : *patterns_) {
            auto& pdb = pdbs_->emplace_back(
                std::make_unique<ProbabilityAwarePatternDatabase>(
                    task_proxy_.get_variables(),
                    pattern));
            const StateRank istate =
                pdb->get_abstract_state(task_proxy_.get_initial_state());
            compute_distances(*pdb, task_proxy_, task_cost_function_, istate);
        }
        cout << "Done computing PDBs for pattern collection: " << timer << endl;
    }
}

void PatternCollectionInformation::create_pattern_cliques_if_missing()
{
    if (!subcollections_) {
        utils::Timer timer;
        cout << "Computing pattern cliques for pattern collection..." << endl;
        subcollections_ =
            subcollection_finder_->compute_subcollections(*patterns_);
        cout << "Done computing pattern cliques for pattern collection: "
             << timer << endl;
    }
}

void PatternCollectionInformation::set_pdbs(
    const shared_ptr<PPDBCollection>& pdbs)
{
    pdbs_ = pdbs;
    assert(information_is_valid());
}

void PatternCollectionInformation::set_subcollections(
    const shared_ptr<vector<PatternSubCollection>>& subcollections)
{
    subcollections_ = subcollections;
    assert(information_is_valid());
}

shared_ptr<PatternCollection> PatternCollectionInformation::get_patterns() const
{
    assert(patterns_);
    return patterns_;
}

shared_ptr<PPDBCollection> PatternCollectionInformation::get_pdbs()
{
    create_pdbs_if_missing();
    return pdbs_;
}

shared_ptr<vector<PatternSubCollection>>
PatternCollectionInformation::get_subcollections()
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
