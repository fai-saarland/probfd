#include "pdbs/cegar/abstract_solution_data.h"

#include "pdbs/cegar/pattern_collection_generator_cegar.h" // TODO: only for Verbosity

#include "global_state.h"

#include "utils/hash.h"
#include "utils/rng.h"

#include <functional>
#include <limits>
#include <queue>

using namespace std;
using utils::Verbosity;

namespace pdbs {

AbstractSolutionData::AbstractSolutionData(
    OperatorCost cost_type,
    const Pattern& pattern,
    set<int> blacklist,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool generate_extended,
    Verbosity verbosity)
    : pdb(new PatternDatabase(
          pattern,
          cost_type,
          false,
          true,
          rng,
          generate_extended))
    , blacklist(std::move(blacklist))
    , extended_plan(pdb->extract_wildcard_plan())
    , is_solvable(true)
    , solved(false)
{
    if (pdb->get_value(g_initial_state()) == numeric_limits<int>::max()) {
        is_solvable = false;
        if (verbosity >= Verbosity::VERBOSE) {
            cout << "PDB with pattern " << get_pattern()
                 << " is unsolvable" << endl;
        }
        return;
    }
}

const Pattern& AbstractSolutionData::get_pattern() const
{
    return pdb->get_pattern();
}

void AbstractSolutionData::blacklist_variable(int var)
{
    blacklist.insert(var);
}

bool AbstractSolutionData::is_blacklisted(int var) const
{
    return utils::contains(blacklist, var);
}

const std::set<int>& AbstractSolutionData::get_blacklist() const
{
    return blacklist;
}

const PatternDatabase&
AbstractSolutionData::get_pdb() const
{
    assert(pdb);
    return *pdb;
}

std::unique_ptr<PatternDatabase>
AbstractSolutionData::steal_pdb() {
    return std::move(pdb);
}

bool AbstractSolutionData::solution_exists() const
{
    return is_solvable;
}

void AbstractSolutionData::mark_as_solved()
{
    solved = true;
}

bool AbstractSolutionData::is_solved() const
{
    return solved;
}

const std::vector<std::vector<int>>&
AbstractSolutionData::get_plan() const
{
    return extended_plan;
}

int AbstractSolutionData::compute_plan_cost() const {
    return pdb->get_value(g_initial_state());
}

void AbstractSolutionData::print_plan(std::ostream& out) const {
    out << "##### Plan for pattern " << get_pattern() << " #####" << endl;
    int i = 1;
    for (const auto &eqv_ops : extended_plan) {
        out << "step #" << i << endl;
        for (int opid : eqv_ops) {
            GlobalOperator& op = g_operators[opid];
            out << op.get_name() << " " << op.get_cost() << endl;
        }
        ++i;
    }
    out << "##### End of plan #####" << endl;
}

}
