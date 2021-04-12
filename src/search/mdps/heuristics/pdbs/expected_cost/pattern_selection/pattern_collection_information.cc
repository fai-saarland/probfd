#include "pattern_collection_information.h"

#include "../expcost_projection.h"
#include "../additivity/additivity_strategy.h"
#include "../additivity/additivity_none.h"

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

using namespace additivity;

namespace pattern_selection {

PatternCollectionInformation::PatternCollectionInformation(
    shared_ptr<PatternCollection> patterns)
    : PatternCollectionInformation(
        std::move(patterns), make_shared<AdditivityNone>())
{
}

PatternCollectionInformation::PatternCollectionInformation(
    shared_ptr<PatternCollection> patterns,
    shared_ptr<AdditivityStrategy> additivity_strategy)
    : patterns(std::move(patterns))
    , additivity_strategy(std::move(additivity_strategy))
{
    assert(this->patterns);
    assert(this->additivity_strategy);
    //validate_and_normalize_patterns(*patterns);
}

bool PatternCollectionInformation::information_is_valid() const {
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
        for (const PatternClique &clique : *pattern_cliques) {
            for (PatternID pattern_id : clique) {
                if (!utils::in_bounds(pattern_id, *patterns)) {
                    return false;
                }
            }
        }
    }
    return true;
}

void PatternCollectionInformation::create_pdbs_if_missing() {
    assert(patterns);
    if (!pdbs) {
        utils::Timer timer;
        cout << "Computing PDBs for pattern collection..." << endl;
        pdbs = make_shared<ExpCostPDBCollection>();
        for (const Pattern &pattern : *patterns) {
            shared_ptr<ExpCostProjection> pdb(new ExpCostProjection(pattern));
            pdbs->push_back(pdb);
        }
        cout << "Done computing PDBs for pattern collection: " << timer << endl;
    }
}

void PatternCollectionInformation::create_pattern_cliques_if_missing() {
    if (!pattern_cliques) {
        utils::Timer timer;
        cout << "Computing pattern cliques for pattern collection..." << endl;
        pattern_cliques =
            additivity_strategy->compute_additive_subcollections(*patterns);
        cout << "Done computing pattern cliques for pattern collection: "
             << timer << endl;
    }
}

void PatternCollectionInformation::set_pdbs(
    const shared_ptr<ExpCostPDBCollection> &pdbs_)
{
    pdbs = pdbs_;
    assert(information_is_valid());
}

void PatternCollectionInformation::set_pattern_cliques(
    const shared_ptr<vector<PatternClique>> &pattern_cliques_)
{
    pattern_cliques = pattern_cliques_;
    assert(information_is_valid());
}

shared_ptr<PatternCollection>
PatternCollectionInformation::get_patterns() const {
    assert(patterns);
    return patterns;
}

shared_ptr<ExpCostPDBCollection> PatternCollectionInformation::get_pdbs() {
    create_pdbs_if_missing();
    return pdbs;
}

shared_ptr<vector<PatternClique>> PatternCollectionInformation::get_pattern_cliques() {
    create_pattern_cliques_if_missing();
    return pattern_cliques;
}
}
}
}