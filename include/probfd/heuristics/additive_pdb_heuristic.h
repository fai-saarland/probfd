//
// Created by Thorsten Klößner on 29.05.2026.
// Copyright (c) 2026 ProbFD contributors.
//

#ifndef ADDITIVE_PDB_HEURISTIC_H
#define ADDITIVE_PDB_HEURISTIC_H

#include "probfd/pdbs/probability_aware_pattern_database.h"

#include <vector>

namespace probfd::heuristics {

class AdditivePDBHeuristic final : public FDRHeuristic {
    std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs_;

public:
    explicit AdditivePDBHeuristic(
        std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs);

    ~AdditivePDBHeuristic() override;

protected:
    value_t evaluate(const downward::State& state) const override;
};

}

#endif //ADDITIVE_PDB_HEURISTIC_H
