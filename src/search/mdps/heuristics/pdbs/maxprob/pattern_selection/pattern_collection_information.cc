#include "pattern_collection_information.h"

#include "../maxprob_projection.h"
#include "../multiplicativity/multiplicativity_none.h"
#include "../multiplicativity/multiplicativity_strategy.h"


#include "../../../../../pdbs/pattern_cliques.h"

#include "../../../../../utils/collections.h"
#include "../../../../../utils/timer.h"

#include <algorithm>
#include <cassert>
#include <unordered_set>
#include <utility>

using namespace std;

namespace probabilistic {
namespace pdbs {
namespace maxprob {

using namespace multiplicativity;

namespace pattern_selection {

PatternCollectionInformation::PatternCollectionInformation(
    shared_ptr<PatternCollection> patterns)
    : PatternCollectionInformation(
          std::move(patterns),
          make_shared<MultiplicativityNone>())
{
}

PatternCollectionInformation::PatternCollectionInformation(
    shared_ptr<PatternCollection> patterns,
    shared_ptr<MultiplicativityStrategy> multiplicativity_strategy)
    : patterns(std::move(patterns))
    , multiplicativity_strategy(std::move(multiplicativity_strategy))
{
    assert(this->patterns);
    assert(this->multiplicativity_strategy);
    // validate_and_normalize_patterns(*patterns);
}

bool PatternCollectionInformation::information_is_valid() const
{
    if (!patterns) {
        return false;
    }
    int num_patterns = patterns->size();
    if (pdbs) {
        if (patterns->size() != pdbs->size()) {
            return false;
        }
        for (int i = 0; i < num_patterns; ++i) {
            if ((*patterns)[i] != (*pdbs)[i]->get_pattern()) {
                return false;
            }
        }
    }
    if (pattern_cliques) {
        for (const PatternClique& clique : *pattern_cliques) {
            for (PatternID pattern_id : clique) {
                if (!utils::in_bounds(pattern_id, *patterns)) {
                    return false;
                }
            }
        }
    }
    return true;
}

void PatternCollectionInformation::create_pdbs_if_missing()
{
    assert(patterns);
    if (!pdbs) {
        utils::Timer timer;
        cout << "Computing PDBs for pattern collection..." << endl;
        pdbs = make_shared<MaxProbPDBCollection>();
        for (const Pattern& pattern : *patterns) {
            pdbs->emplace_back(new MaxProbProjection(pattern));
        }
        cout << "Done computing PDBs for pattern collection: " << timer << endl;
    }
}

void PatternCollectionInformation::create_pattern_cliques_if_missing()
{
    if (!pattern_cliques) {
        utils::Timer timer;
        cout << "Computing pattern cliques for pattern collection..." << endl;
        pattern_cliques =
            multiplicativity_strategy->compute_multiplicative_subcollections(
                *patterns);
        cout << "Done computing pattern cliques for pattern collection: "
             << timer << endl;
    }
}

void PatternCollectionInformation::set_pdbs(
    const shared_ptr<MaxProbPDBCollection>& pdbs_)
{
    pdbs = pdbs_;
    assert(information_is_valid());
}

void PatternCollectionInformation::set_pattern_cliques(
    const shared_ptr<vector<PatternClique>>& pattern_cliques_)
{
    pattern_cliques = pattern_cliques_;
    assert(information_is_valid());
}

shared_ptr<PatternCollection> PatternCollectionInformation::get_patterns() const
{
    assert(patterns);
    return patterns;
}

shared_ptr<MaxProbPDBCollection> PatternCollectionInformation::get_pdbs()
{
    create_pdbs_if_missing();
    return pdbs;
}

shared_ptr<vector<PatternClique>>
PatternCollectionInformation::get_pattern_cliques()
{
    create_pattern_cliques_if_missing();
    return pattern_cliques;
}
} // namespace pattern_selection
} // namespace maxprob
} // namespace pdbs
} // namespace probabilistic