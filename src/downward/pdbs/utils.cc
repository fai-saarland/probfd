#include "downward/pdbs/utils.h"

#include "downward/pdbs/pattern_collection_information.h"
#include "downward/pdbs/pattern_database.h"
#include "downward/pdbs/pattern_information.h"

#include "downward/task_utils/causal_graph.h"
#include "downward/task_utils/task_properties.h"

#include "downward/utils/collections.h"
#include "downward/utils/logging.h"
#include "downward/utils/math.h"
#include "downward/utils/rng.h"

#include "downward/goal_fact_list.h"

#include <limits>
#include <print>

using namespace std;

namespace downward::pdbs {
int compute_pdb_size(const VariableSpace& variables, const Pattern& pattern)
{
    int size = 1;
    for (int var : pattern) {
        int domain_size = variables[var].get_domain_size();
        if (utils::is_product_within_limit(
                size,
                domain_size,
                numeric_limits<int>::max())) {
            size *= domain_size;
        } else {
            std::println(
                cerr,
                "Given pattern is too large! (Overflow occured): {}",
                pattern);
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }
    }
    return size;
}

int compute_total_pdb_size(
    const VariableSpace& variables,
    const PatternCollection& pattern_collection)
{
    int size = 0;
    for (const Pattern& pattern : pattern_collection) {
        size += compute_pdb_size(variables, pattern);
    }
    return size;
}

vector<FactPair> get_goals_in_random_order(
    const GoalFactList& goals,
    utils::RandomNumberGenerator& rng)
{
    vector<FactPair> goal_facts = task_properties::get_fact_pairs(goals);
    rng.shuffle(goal_facts);
    return goal_facts;
}

vector<int> get_non_goal_variables(
    const VariableSpace& variables,
    const GoalFactList& goals)
{
    size_t num_vars = variables.size();
    vector<bool> is_goal(num_vars, false);
    for (FactPair goal : goals) { is_goal[goal.var] = true; }

    vector<int> non_goal_variables;
    non_goal_variables.reserve(num_vars - goals.size());
    for (int var_id = 0; var_id < static_cast<int>(num_vars); ++var_id) {
        if (!is_goal[var_id]) { non_goal_variables.push_back(var_id); }
    }
    return non_goal_variables;
}

vector<vector<int>>
compute_cg_neighbors(const SharedAbstractTask& task, bool bidirectional)
{
    const auto& variables = get_variables(task);

    int num_vars = variables.size();
    const auto& cg = causal_graph::get_causal_graph(to_refs(task));
    vector<vector<int>> cg_neighbors(num_vars);
    for (int var_id = 0; var_id < num_vars; ++var_id) {
        cg_neighbors[var_id] = cg.get_predecessors(var_id);
        if (bidirectional) {
            const vector<int>& successors = cg.get_successors(var_id);
            cg_neighbors[var_id].insert(
                cg_neighbors[var_id].end(),
                successors.begin(),
                successors.end());
        }
        utils::sort_unique(cg_neighbors[var_id]);
    }
    return cg_neighbors;
}

PatternCollectionInformation get_pattern_collection_info(
    const AbstractTaskTuple& task,
    const shared_ptr<PDBCollection>& pdbs,
    utils::LogProxy& log)
{
    shared_ptr<PatternCollection> patterns = make_shared<PatternCollection>();
    patterns->reserve(pdbs->size());
    for (const shared_ptr<PatternDatabase>& pdb : *pdbs) {
        patterns->push_back(pdb->get_pattern());
    }
    PatternCollectionInformation result(task, patterns, log);
    result.set_pdbs(pdbs);
    return result;
}

void dump_pattern_generation_statistics(
    const string& identifier,
    utils::Duration runtime,
    const PatternInformation& pattern_info,
    utils::LogProxy& log)
{
    const Pattern& pattern = pattern_info.get_pattern();
    if (log.is_at_least_normal()) {
        log << identifier << " pattern: " << pattern << endl;
        log << identifier << " number of variables: " << pattern.size() << endl;
        log << identifier << " PDB size: "
            << compute_pdb_size(get_variables(pattern_info.get_task()), pattern)
            << endl;
        log << identifier << " computation time: " << runtime << endl;
    }
}

void dump_pattern_collection_generation_statistics(
    const string& identifier,
    utils::Duration runtime,
    const PatternCollectionInformation& pci,
    utils::LogProxy& log)
{
    const PatternCollection& pattern_collection = *pci.get_patterns();
    if (log.is_at_least_normal()) {
        log << identifier
            << " number of patterns: " << pattern_collection.size() << endl;
        log << identifier << " total PDB size: "
            << compute_total_pdb_size(
                   get_variables(pci.get_task()),
                   pattern_collection)
            << endl;
        log << identifier << " computation time: " << runtime << endl;
    }
}

} // namespace downward::pdbs
