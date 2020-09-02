#pragma once

#include "../algorithms/segmented_vector.h"
#include "../algorithms/ub_tree.h"
#include "../fact_set.h"
#include "../heuristic.h"

#include <iostream>
#include <memory>
#include <vector>

namespace strips_utils {
class STRIPS;
}

namespace critical_path_heuristic {

struct CounterInfo;

struct ConjunctionInfo {
    static const int UNACHIEVED;

    ConjunctionInfo();
    ~ConjunctionInfo() = default;

    unsigned id;
    int cost;
    std::vector<std::pair<CounterInfo*, unsigned>> achievers;
    std::vector<CounterInfo*> in_regression_of; // sorted!
};

struct CounterInfo {
    CounterInfo();
    ~CounterInfo() = default;

    unsigned id;
    int unsat;
    std::vector<std::pair<ConjunctionInfo*, unsigned>> achieving;
    std::vector<unsigned> regression_conjunction_ids; // sorted!
    strips_utils::FactSet regression;
};

class CriticalPathHeuristic : public Heuristic {
public:
    explicit CriticalPathHeuristic(const options::Options& opts);
    virtual ~CriticalPathHeuristic() override = default;

    virtual bool supports_partial_state_evaluation() const override;

    double counter_ratio() const;
    double counter_effect_ratio() const;
    double conjunction_ratio() const;
    unsigned num_conjunctions() const;
    unsigned num_counters() const;
    unsigned num_counter_effects() const;
    unsigned num_no_effect_counters() const;
    unsigned num_base_counters() const;

    std::shared_ptr<strips_utils::STRIPS> get_task() const;

    void print_size_info(std::ostream& out = std::cout) const;
    void print_construction_statistics(std::ostream& out = std::cout) const;
    void print_statistics() const;

    bool set_exploration_early_termination(bool term);
    void reset_exploration_cache();
    void reset_exploration_cache(const std::vector<unsigned>& conjunctions);
    int
    exploration_unit_cost(const std::vector<unsigned>& satisfied_conjunctions);
    int incremental_exploration_unit_cost(
        std::vector<unsigned>& newly_achieved_conjunctions);
    int exploration_general_cost(
        const std::vector<unsigned>& satisfied_conjunctions);

    bool is_conjunction_reachable(unsigned conjunction_id) const;
    int get_cost(unsigned conjunction_id) const;
    const strips_utils::FactSet&
    get_conjunction_fact_set(unsigned conjunction_id) const;

    std::vector<unsigned>
    get_satisfied_conjunctions(const GlobalState& state) const;
    std::vector<unsigned>
    get_satisfied_conjunctions(const PartialState& state) const;
    std::vector<unsigned>
    get_satisfied_conjunctions(const strips_utils::FactSet& fact_set) const;

    std::pair<unsigned, bool>
    add_conjunction(const strips_utils::FactSet& fact_set);

    CounterInfo* get_counter_info(unsigned counter_id);
    CounterInfo* get_goal_counter();
    const CounterInfo* get_counter_info(unsigned counter_id) const;
    ConjunctionInfo* get_conjunction_info(unsigned conjunction_id);
    const ConjunctionInfo* get_conjunction_info(unsigned conjunction_id) const;
    const strips_utils::FactSetSet& get_conjunction_fact_sets() const;

    const std::vector<unsigned>& get_goal_conjunctions() const;

    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual int compute_heuristic(const GlobalState& state) override;
    virtual int compute_heuristic(const PartialState& state) override;

private:
    void initialize_with_unit_conjunctions();
    void add_m_conjunctions(int m);
    void add_m_conjunctions(int var, strips_utils::FactSet& fact_set, int m);

    const bool prune_dominated_preconditions_;
    const bool prune_dominated_achievers_;
    const bool is_unit_cost_;
    bool early_termination_;
    std::string store_conjunctions_;

    std::shared_ptr<strips_utils::STRIPS> task_;

    std::vector<int> action_costs_;
    unsigned true_conjunction_;
    unsigned goal_counter_;

    strips_utils::FactSetSet conjunctions_;
    segmented_vector::SegmentedVector<ConjunctionInfo> conjunction_infos_;

    ub_tree::UBTree<
        strips_utils::Fact,
        CounterInfo,
        ub_tree::defaults::allocator<CounterInfo>,
        segmented_vector::SegmentedVector<CounterInfo>>
        counter_infos_;

    unsigned num_unit_conjunctions_;
    unsigned num_fact_counters_;
    unsigned num_fact_counters_effects_;
    unsigned num_removed_precondition_relations_;
    unsigned num_removed_effect_relations_;
    unsigned num_removed_counters_;
};

} // namespace critical_path_heuristic
