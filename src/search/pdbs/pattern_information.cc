#include "pattern_information.h"

#include "pattern_database.h"
#include "validation.h"

#include <cassert>

using namespace std;

namespace pdbs {
PatternInformation::PatternInformation(
    OperatorCost operator_cost,
    Pattern pattern)
    : operator_cost(operator_cost),
      pattern(move(pattern)),
      pdb(nullptr) {
    validate_and_normalize_pattern(this->pattern);
}

bool PatternInformation::information_is_valid() const {
    return !pdb || pdb->get_pattern() == pattern;
}

void PatternInformation::create_pdb_if_missing() {
    if (!pdb) {
        pdb = make_shared<PatternDatabase>(pattern, operator_cost);
    }
}

void PatternInformation::set_pdb(const shared_ptr<PatternDatabase> &pdb_) {
    pdb = pdb_;
    assert(information_is_valid());
}

const Pattern &PatternInformation::get_pattern() const {
    return pattern;
}

shared_ptr<PatternDatabase> PatternInformation::get_pdb() {
    create_pdb_if_missing();
    return pdb;
}
}
