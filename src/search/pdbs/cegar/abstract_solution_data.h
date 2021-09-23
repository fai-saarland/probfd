#ifndef PDBS_ABSTRACT_SOLUTION_DATA_H
#define PDBS_ABSTRACT_SOLUTION_DATA_H

#include "../pattern_database.h"

#include "../utils/logging.h"

#include <memory>
#include <ostream>
#include <set>
#include <vector>

namespace utils {
class RandomNumberGenerator;
}

namespace pdbs {

class AbstractSolutionData {
    std::unique_ptr<PatternDatabase> pdb;
    std::set<int> blacklist;
    std::vector<std::vector<int>> extended_plan;

    bool is_solvable;
    bool solved;

public:
    AbstractSolutionData(
        OperatorCost cost_type,
        const Pattern& pattern,
        std::set<int> blacklist,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool generate_extended,
        utils::Verbosity verbosity);

    const Pattern& get_pattern() const;

    void blacklist_variable(int var);

    bool is_blacklisted(int var) const;

    const std::set<int>& get_blacklist() const;

    const PatternDatabase& get_pdb() const;

    std::unique_ptr<PatternDatabase> steal_pdb();

    bool solution_exists() const;

    void mark_as_solved();

    bool is_solved() const;

    const std::vector<std::vector<int>>& get_plan() const;

    int compute_plan_cost() const;

    void print_plan(std::ostream& out) const;
};
}

#endif
