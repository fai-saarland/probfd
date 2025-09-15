#ifndef LANDMARKS_LANDMARK_FACTORY_RPG_SASP_H
#define LANDMARKS_LANDMARK_FACTORY_RPG_SASP_H

#include "downward/landmarks/landmark_factory_relaxation.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace downward {
class AxiomOrOperatorProxy;
}

namespace downward::landmarks {
class LandmarkFactoryRpgSasp : public LandmarkFactoryRelaxation {
    const bool disjunctive_landmarks;
    const bool use_orders;
    std::list<LandmarkNode*> open_landmarks;
    std::vector<std::vector<int>> disjunction_classes;

    std::unordered_map<LandmarkNode*, utils::HashSet<FactPair>> forward_orders;

    // dtg_successors[var_id][val] contains all successor values of val in the
    // domain transition graph for the variable
    std::vector<std::vector<std::unordered_set<int>>> dtg_successors;

    void build_dtg_successors(
        const VariableSpace& variables,
        const ClassicalOperatorSpace& operators);

    void add_dtg_successor(int var_id, int pre, int post);

    void find_forward_orders(
        const VariableSpace& variables,
        const std::vector<std::vector<bool>>& reached,
        LandmarkNode* lm_node);

    void add_lm_forward_orders();

    void get_greedy_preconditions_for_lm(
        const VariableSpace& variables,
        const State& initial_state,
        const Landmark& landmark,
        const AxiomOrOperatorProxy& op,
        std::unordered_map<int, int>& result) const;

    void compute_shared_preconditions(
        const VariableSpace& variables,
        const AxiomSpace& axioms,
        const ClassicalOperatorSpace& operators,
        const State& initial_state,
        std::unordered_map<int, int>& shared_pre,
        const std::vector<std::vector<bool>>& reached,
        const Landmark& landmark) const;

    void compute_disjunctive_preconditions(
        const VariableSpace& variables,
        const AxiomSpace& axioms,
        const ClassicalOperatorSpace& operators,
        const State& initial_state,
        std::vector<std::set<FactPair>>& disjunctive_pre,
        std::vector<std::vector<bool>>& reached,
        const Landmark& landmark) const;

    void generate_relaxed_landmarks(
        const SharedAbstractTask& task,
        Exploration& exploration) override;

    void
    found_simple_lm_and_order(const FactPair& a, LandmarkNode& b, EdgeType t);

    void found_disj_lm_and_order(
        const State& state,
        const std::set<FactPair>& a,
        LandmarkNode& b,
        EdgeType t);

    void approximate_lookahead_orders(
        const VariableSpace& variables,
        const State& initial_state,
        const std::vector<std::vector<bool>>& reached,
        LandmarkNode* lmp);

    bool domain_connectivity(
        const VariableSpace& variables,
        const State& initial_state,
        const FactPair& landmark,
        const std::unordered_set<int>& exclude) const;

    void build_disjunction_classes(const VariableSpace& variables);

    void discard_disjunctive_landmarks() const;

public:
    LandmarkFactoryRpgSasp(
        bool disjunctive_landmarks,
        bool use_orders,
        utils::Verbosity verbosity);

    bool supports_conditional_effects() const override;
};
} // namespace downward::landmarks

#endif
