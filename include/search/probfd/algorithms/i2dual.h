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

#include <vector>

namespace lp {
class LinearProgram;
}

namespace probfd {
class ProgressReport;

namespace algorithms {
namespace i2dual {

class I2Dual : public MDPAlgorithm<State, OperatorID> {
    struct IDualData;

    struct Statistics {
        utils::Timer idual_timer_ = utils::Timer(true);
        utils::Timer lp_solver_timer_ = utils::Timer(true);
        utils::Timer hpom_timer_ = utils::Timer(true);

        unsigned long long iterations_ = 0;
        unsigned long long expansions_ = 0;
        unsigned long long open_states_ = 0;
        unsigned long long num_lp_vars_ = 0;
        unsigned long long num_lp_constraints_ = 0;
        unsigned long long hpom_num_vars_ = 0;
        unsigned long long hpom_num_constraints_ = 0;

        void print(std::ostream& out) const;
    };

    ProbabilisticTaskProxy task_proxy;
    std::shared_ptr<FDRCostFunction> task_cost_function;

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
        const State& state,
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
        const unsigned start);

    void remove_fringe_state_from_hpom(
        const State& state,
        const IDualData& data,
        named_vector::NamedVector<lp::LPConstraint>& constraints) const;

    void add_fringe_state_to_hpom(
        const State& state,
        const IDualData& data,
        named_vector::NamedVector<lp::LPConstraint>& constraints) const;
};

} // namespace i2dual
} // namespace algorithms
} // namespace probfd

#endif // __I2DUAL_H__