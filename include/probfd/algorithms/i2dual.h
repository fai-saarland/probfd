#ifndef PROBFD_ALGORITHMS_I2DUAL_H
#define PROBFD_ALGORITHMS_I2DUAL_H

#include "probfd/storage/per_state_storage.h"

#include "probfd/fdr_types.h"
#include "probfd/mdp_algorithm.h"
#include "probfd/probabilistic_task.h"
#include "probfd/transition_tail.h"
#include "probfd/value_type.h"

#include "downward/lp/lp_solver.h"

#include "downward/utils/timer.h"

#include <cstddef>
#include <iosfwd>
#include <limits>
#include <memory>
#include <vector>

namespace probfd {
class ProgressReport;
}

namespace probfd::algorithms::i2dual {

class I2Dual : public MDPAlgorithm<downward::State, downward::OperatorID> {
    struct IDualData;

    struct Statistics {
        downward::utils::Timer idual_timer = downward::utils::Timer(true);
        downward::utils::Timer lp_solver_timer = downward::utils::Timer(true);
        downward::utils::Timer hpom_timer = downward::utils::Timer(true);

        unsigned long long iterations = 0;
        unsigned long long expansions = 0;
        unsigned long long open_states = 0;
        unsigned long long num_lp_vars = 0;
        unsigned long long num_lp_constraints = 0;
        unsigned long long hpom_num_vars = 0;
        unsigned long long hpom_num_constraints = 0;

        void print(std::ostream& out) const;
    };

    ProbabilisticTaskTuple task_;

    const bool hpom_enabled_;
    const bool incremental_hpom_updates_;

    downward::lp::LPSolver lp_solver_;

    const double fp_epsilon_ = 0.001;

    size_t next_lp_var_ = 0;
    size_t next_lp_constr_id_ = 0;

    bool hpom_initialized_ = false;
    std::vector<int> offset_;
    downward::named_vector::NamedVector<downward::lp::LPConstraint>
        hpom_constraints_;

    Statistics statistics_;

    value_t objective_;

    std::vector<downward::OperatorID> aops_;
    SuccessorDistribution succs_;

public:
    I2Dual(
        SharedProbabilisticTask task,
        bool hpom_enabled,
        bool incremental_updates,
        downward::lp::LPSolverType solver_type,
        double fp_precision = 0.0001);

    ~I2Dual() override;

    void print_statistics(std::ostream& out) const override;

    Interval solve(
        FDRMDP& mdp,
        FDRHeuristic& heuristic,
        const downward::State& initial_state,
        ProgressReport progress,
        downward::utils::Duration max_time);

    std::unique_ptr<PolicyType> compute_policy(
        FDRMDP& mdp,
        HeuristicType& heuristic,
        const downward::State& initial_state,
        ProgressReport progress,
        downward::utils::Duration max_time) override;

private:
    bool evaluate_state(
        FDRMDP& mdp,
        FDRHeuristic& heuristic,
        const downward::State& state,
        IDualData& data);

    void prepare_lp();

    void prepare_hpom(downward::lp::LinearProgram& lp);

    void update_hpom_constraints_expanded(
        FDRMDP& mdp,
        storage::PerStateStorage<IDualData>& data,
        const std::vector<StateID>& expanded);

    void update_hpom_constraints_frontier(
        FDRMDP& mdp,
        storage::PerStateStorage<IDualData>& data,
        const std::vector<StateID>& frontier,
        unsigned start);

    void remove_fringe_state_from_hpom(
        const downward::State& state,
        const IDualData& data,
        downward::named_vector::NamedVector<downward::lp::LPConstraint>&
            constraints) const;

    void add_fringe_state_to_hpom(
        const downward::State& state,
        const IDualData& data,
        downward::named_vector::NamedVector<downward::lp::LPConstraint>&
            constraints) const;
};

} // namespace probfd::algorithms::i2dual

#endif // PROBFD_ALGORITHMS_I2DUAL_H