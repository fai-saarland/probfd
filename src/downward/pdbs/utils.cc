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

#include <limits>

using namespace std;

namespace downward::pdbs {
int compute_pdb_size(const AbstractTask& task, const Pattern& pattern)
{
    int size = 1;
    for (int var : pattern) {
        int domain_size = task.get_variables()[var].get_domain_size();
        if (utils::is_product_within_limit(
                size,
                domain_size,
                numeric_limits<int>::max())) {
            size *= domain_size;
        } else {
            cerr << "Given pattern is too large! (Overflow occurred): " << endl;
            cerr << pattern << endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }
    }
    return size;
}

int compute_total_pdb_size(
    const AbstractTask& task,
    const PatternCollection& pattern_collection)
{
    int size = 0;
    for (const Pattern& pattern : pattern_collection) {
        size += compute_pdb_size(task, pattern);
    }
    return size;
}

vector<FactPair> get_goals_in_random_order(
    const AbstractTask& task,
    utils::RandomNumberGenerator& rng)
{
    vector<FactPair> goals =
        task_properties::get_fact_pairs(task.get_goals());
    rng.shuffle(goals);
    return goals;
}

vector<int> get_non_goal_variables(const AbstractTask& task)
{
    size_t num_vars = task.get_variables().size();
    GoalsProxy goals = task.get_goals();
    vector<bool> is_goal(num_vars, false);
    for (FactPair goal : goals) {
        is_goal[goal.var] = true;
    }

    vector<int> non_goal_variables;
    non_goal_variables.reserve(num_vars - goals.size());
    for (int var_id = 0; var_id < static_cast<int>(num_vars); ++var_id) {
        if (!is_goal[var_id]) {
            non_goal_variables.push_back(var_id);
        }
    }
    return non_goal_variables;
}

vector<vector<int>>
compute_cg_neighbors(const shared_ptr<AbstractTask>& task, bool bidirectional)
{
    int num_vars = task->get_variables().size();
    const causal_graph::CausalGraph& cg =
        causal_graph::get_causal_graph(task.get());
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
    const AbstractTask& task,
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
            << compute_pdb_size(pattern_info.get_task(), pattern) << endl;
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
            << compute_total_pdb_size(pci.get_task(), pattern_collection)
            << endl;
        log << identifier << " computation time: " << runtime << endl;
    }
}

} // namespace pdbs
