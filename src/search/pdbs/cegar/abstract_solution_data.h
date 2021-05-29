#ifndef PDBS_ABSTRACT_SOLUTION_DATA_H
#define PDBS_ABSTRACT_SOLUTION_DATA_H

#include "../pattern_database.h"

#include "tasks/pdb_abstracted_task.h"
#include "tasks/task_proxy.h"

#include <set>
#include <utility>
#include <ostream>

namespace successor_generator {
template <typename Entry>
class SuccessorGenerator;
}

namespace utils {
class RandomNumberGenerator;
}

namespace pdbs {

struct SearchNode {
    AbstractState state;
    std::size_t hash;
    int g;
    int cost;
    std::shared_ptr<SearchNode> predecessor;

    SearchNode(
        AbstractState state,
        std::size_t hash,
        int g,
        int cost,
        std::shared_ptr<SearchNode> predecessor)
        : state(std::move(state))
        , hash(hash)
        , g(g)
        , cost(cost)
        , predecessor(predecessor) {}
};

enum class Verbosity;

class AbstractSolutionData {
    std::shared_ptr<PatternDatabase> pdb;
    tasks::PDBAbstractedTask abstracted_task;
    TaskProxy abs_task_proxy;
    std::set<int> blacklist;
    std::vector<std::vector<int>> extended_plan;
    bool generate_extended;
    bool is_solvable;
    bool solved;

    std::vector<AbstractState> extract_state_sequence(
        const SearchNode &goal_node) const;
    std::vector<AbstractState> bfs_for_improving_state(
        const successor_generator::SuccessorGenerator<int> &succ_gen,
        const std::shared_ptr<utils::RandomNumberGenerator> &rng,
        const int f_star,
        std::shared_ptr<SearchNode> &current_node) const;
    std::vector<AbstractState> steepest_ascent_enforced_hillclimbing(
        const successor_generator::SuccessorGenerator<int> &succ_gen,
        const std::shared_ptr<utils::RandomNumberGenerator> &rng,
        Verbosity verbosity) const;
    void turn_state_sequence_into_plan(
        const successor_generator::SuccessorGenerator<int> &succ_gen,
        const std::shared_ptr<utils::RandomNumberGenerator> &rng,
        Verbosity verbosity,
        const std::vector<AbstractState> &state_sequence);
        
public:
    AbstractSolutionData(
        OperatorCost cost_type,
        const Pattern& pattern,
        std::set<int> blacklist,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool extended,
        Verbosity verbosity);

    const Pattern& get_pattern() const;

    void blacklist_variable(int var);

    bool is_blacklisted(int var) const;

    const std::set<int>& get_blacklist() const;

    const std::shared_ptr<PatternDatabase>& get_pdb();

    bool solution_exists() const;

    void mark_as_solved();

    bool is_solved() const;

    const std::vector<std::vector<int>>& get_plan() const;

    int convert_operator_index_to_parent(int abs_op_id) const;

    int compute_plan_cost() const;

    void print_plan(std::ostream& out) const;
};
}

namespace utils {
inline void feed(HashState &hash_state, const pdbs::SearchNode &search_node) {
    feed(hash_state, search_node.state);
}
}

#endif
