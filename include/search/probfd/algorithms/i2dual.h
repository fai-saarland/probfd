#ifndef PROBFD_ALGORITHMS_I2DUAL_H
#define PROBFD_ALGORITHMS_I2DUAL_H

#include "probfd/storage/per_state_storage.h"

#include "probfd/distribution.h"
#include "probfd/fdr_types.h"
#include "probfd/mdp_algorithm.h"
#include "probfd/task_proxy.h"
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

class I2Dual : public MDPAlgorithm<State, OperatorID> {
    struct IDualData;

    struct Statistics {
        utils::Timer idual_timer = utils::Timer(true);
        utils::Timer lp_solver_timer = utils::Timer(true);
        utils::Timer hpom_timer = utils::Timer(true);

        unsigned long long iterations = 0;
        unsigned long long expansions = 0;
        unsigned long long open_states = 0;
        unsigned long long num_lp_vars = 0;
        unsigned long long num_lp_constraints = 0;
        unsigned long long hpom_num_vars = 0;
        unsigned long long hpom_num_constraints = 0;

        void print(std::ostream& out) const;
    };

    ProbabilisticTaskProxy task_proxy_;
    std::shared_ptr<FDRCostFunction> task_cost_function_;

    const bool hpom_enabled_;
    const bool incremental_hpom_updates_;

    lp::LPSolver lp_solver_;

    size_t next_lp_var_ = 0;
    size_t next_lp_constr_id_ = 0;

    bool hpom_initialized_ = false;
    std::vector<int> offset_;
    named_vector::NamedVector<lp::LPConstraint> hpom_constraints_;

    Statistics statistics_;

    value_t objective_;

    std::vector<OperatorID> aops_;
    Distribution<StateID> succs_;

public:
    I2Dual(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        bool hpom_enabled,
        bool incremental_updates,
        lp::LPSolverType solver_type);

    void print_statistics(std::ostream& out) const override;

    Interval solve(
        FDRMDP& mdp,
        FDREvaluator& heuristic,
        const State& initial_state,
        ProgressReport progress,
        double max_time) override;

    std::unique_ptr<PolicyType> compute_policy(
        FDRMDP& mdp,
        EvaluatorType& heuristic,
        const State& initial_state,
        ProgressReport progress,
        double max_time) override;

private:
    bool evaluate_state(
        FDRMDP& mdp,
        FDREvaluator& heuristic,
        const State& state,
        IDualData& data);

    void prepare_lp();

    void prepare_hpom(lp::LinearProgram& lp);

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
        const State& state,
        const IDualData& data,
        named_vector::NamedVector<lp::LPConstraint>& constraints) const;

    void add_fringe_state_to_hpom(
        const State& state,
        const IDualData& data,
        named_vector::NamedVector<lp::LPConstraint>& constraints) const;
};

} // namespace probfd::algorithms::i2dual

#endif // PROBFD_ALGORITHMS_I2DUAL_H