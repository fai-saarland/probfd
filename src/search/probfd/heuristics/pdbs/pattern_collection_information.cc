#include "probfd/heuristics/pdbs/pattern_collection_information.h"

#include "probfd/heuristics/pdbs/maxprob_pattern_database.h"
#include "probfd/heuristics/pdbs/ssp_pattern_database.h"
#include "probfd/heuristics/pdbs/trivial_finder.h"


#include "pdbs/pattern_cliques.h"
#include "pdbs/pattern_collection_information.h"

#include "utils/collections.h"
#include "utils/timer.h"

#include <algorithm>
#include <cassert>
#include <unordered_set>
#include <utility>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {

template <typename PDBType>
PatternCollectionInformation<PDBType>::PatternCollectionInformation(
    const ProbabilisticTaskProxy& task_proxy,
    ::pdbs::PatternCollectionInformation det_info,
    shared_ptr<SubCollectionFinder> subcollection_finder)
    : task_proxy(task_proxy)
    , patterns_(det_info.get_patterns())
    , subcollections_(det_info.get_pattern_cliques())
    , subcollection_finder_(std::move(subcollection_finder))
{
    auto pdbs = det_info.get_pdbs();

    if (!pdbs) {
        return;
    }

    pdbs_ = make_shared<PPDBCollection<PDBType>>();

    for (size_t i = 0; i != pdbs->size(); ++i) {
        pdbs_->emplace_back(new PDBType(
            task_proxy,
            *pdbs->operator[](i),
            task_proxy.get_initial_state()));
    }
}

template <typename PDBType>
PatternCollectionInformation<PDBType>::PatternCollectionInformation(
    const ProbabilisticTaskProxy& task_proxy,
    shared_ptr<PatternCollection> patterns)
    : PatternCollectionInformation(
          task_proxy,
          std::move(patterns),
          make_shared<TrivialFinder>())
{
}

template <typename PDBType>
PatternCollectionInformation<PDBType>::PatternCollectionInformation(
    const ProbabilisticTaskProxy& task_proxy,
    shared_ptr<PatternCollection> patterns,
    shared_ptr<SubCollectionFinder> subcollection_finder)
    : task_proxy(task_proxy)
    , patterns_(std::move(patterns))
    , subcollection_finder_(std::move(subcollection_finder))
{
    assert(this->patterns_);
    assert(this->subcollection_finder_);
    // validate_and_normalize_patterns(*patterns);
}

template <typename PDBType>
bool PatternCollectionInformation<PDBType>::information_is_valid() const
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

template <typename PDBType>
void PatternCollectionInformation<PDBType>::create_pdbs_if_missing()
{
    assert(patterns_);
    if (!pdbs_) {
        utils::Timer timer;
        cout << "Computing PDBs for pattern collection..." << endl;
        pdbs_ = make_shared<PPDBCollection<PDBType>>();
        for (const Pattern& pattern : *patterns_) {
            pdbs_->emplace_back(new PDBType(
                task_proxy,
                pattern,
                task_proxy.get_initial_state()));
        }
        cout << "Done computing PDBs for pattern collection: " << timer << endl;
    }
}

template <typename PDBType>
void PatternCollectionInformation<PDBType>::create_pattern_cliques_if_missing()
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

template <typename PDBType>
void PatternCollectionInformation<PDBType>::set_pdbs(
    const shared_ptr<PPDBCollection<PDBType>>& pdbs)
{
    pdbs_ = pdbs;
    assert(information_is_valid());
}

template <typename PDBType>
void PatternCollectionInformation<PDBType>::set_subcollections(
    const shared_ptr<vector<PatternSubCollection>>& subcollections)
{
    subcollections_ = subcollections;
    assert(information_is_valid());
}

template <typename PDBType>
shared_ptr<PatternCollection>
PatternCollectionInformation<PDBType>::get_patterns() const
{
    assert(patterns_);
    return patterns_;
}

template <typename PDBType>
shared_ptr<PPDBCollection<PDBType>>
PatternCollectionInformation<PDBType>::get_pdbs()
{
    create_pdbs_if_missing();
    return pdbs_;
}

template <typename PDBType>
shared_ptr<vector<PatternSubCollection>>
PatternCollectionInformation<PDBType>::get_subcollections()
{
    create_pattern_cliques_if_missing();
    return subcollections_;
}

template class PatternCollectionInformation<MaxProbPatternDatabase>;
template class PatternCollectionInformation<SSPPatternDatabase>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd