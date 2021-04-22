#pragma once

#include "../../global_state.h"
#include "../../lp/lp_solver.h"
#include "../../solver_interface.h"
#include "../../state_registry.h"
#include "../../utils/timer.h"
#include "../probabilistic_operator.h"
#include "../progress_report.h"
#include "../state_evaluator.h"
#include "../storage/per_state_storage.h"
#include "../transition_generator.h"
#include "../value_type.h"

#include <memory>
#include <vector>

namespace options {
class Options;
class OptionParser;
} // namespace options

class StateRegistry;

namespace probabilistic {
namespace solvers {
namespace i2dual {

class IDualData;

class I2Dual : public SolverInterface {
public:
    explicit I2Dual(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);
    virtual bool found_solution() const override { return true; }
    virtual void print_statistics() const override;
    virtual void solve() override;

private:
    bool evaluate_state(const GlobalState& state, IDualData& data);
    void prepare_lp();
    void prepare_hpom(
        std::vector<lp::LPVariable>& vars,
        std::vector<lp::LPConstraint>& constraints);
    void update_hpom_constraints_expanded(
        storage::PerStateStorage<IDualData>& data,
        const std::vector<StateID>& expanded);
    void update_hpom_constraints_frontier(
        storage::PerStateStorage<IDualData>& data,
        const std::vector<StateID>& frontier,
        const int start);
    void remove_fringe_state_from_hpom(
        const GlobalState& state,
        const IDualData& data,
        std::vector<lp::LPConstraint>& constraints) const;
    void add_fringe_state_to_hpom(
        const GlobalState& state,
        const IDualData& data,
        std::vector<lp::LPConstraint>& constraints) const;

    ProgressReport progress_;

    StateRegistry state_registry_;

    std::shared_ptr<StateEvaluator<GlobalState>> state_reward_function_;
    value_type::value_t minimal_reward_;
    value_type::value_t maximal_reward_;
    TransitionGenerator<const ProbabilisticOperator*> transition_generator_;
    ApplicableActionsGenerator<const ProbabilisticOperator*> aops_generator_;

    std::shared_ptr<GlobalStateEvaluator> eval_;

    const bool hpom_enabled_;
    const bool incremental_hpom_updates_;

    lp::LPSolver lp_solver_;
    unsigned next_lp_var_ = 0;
    unsigned next_lp_constraint_id_ = 0;

    utils::Timer lp_solver_timer_;

    bool hpom_initialized_ = false;
    std::vector<int> offset_;
    std::vector<lp::LPConstraint> hpom_constraints_;

    utils::Timer hpom_timer_;
    unsigned hpom_num_vars_ = 0;
    unsigned hpom_num_constraints_ = 0;
};

} // namespace i2dual
} // namespace solvers
} // namespace probabilistic
