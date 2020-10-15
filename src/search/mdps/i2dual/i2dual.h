#pragma once

#include "../../lp/lp_solver.h"
#include "../../solver_interface.h"
#include "../../utils/timer.h"
#include "../algorithms/types_storage.h"
#include "../state_evaluator.h"
#include "../value_type.h"

#include <memory>
#include <vector>

namespace options {
class Options;
class OptionParser;
} // namespace options

class StateRegistry;

namespace probabilistic {
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
        StateRegistry& registry,
        algorithms::storage::PerStateStorage<IDualData>& data,
        const std::vector<StateID>& expanded);
    void update_hpom_constraints_frontier(
        StateRegistry& registry,
        algorithms::storage::PerStateStorage<IDualData>& data,
        const std::vector<StateID>& frontier,
        const int start);
    void update_hpom_constraints_for_state(
        const GlobalState& state,
        const IDualData& data,
        std::vector<lp::LPConstraint>& constraints,
        const double negate) const;

    GlobalStateEvaluator* reward_;

    const value_type::value_t report_difference_;
    std::shared_ptr<GlobalStateEvaluator> eval_;

    const bool hpom_;
    const bool incremental_hpom_updates_;

    lp::LPSolver lp_solver_;
    unsigned next_lp_var_ = 0;
    unsigned next_lp_constraint_id_ = 0;

    utils::Timer lp_solver_timer_;

    bool hpom_initialized_ = false;
    std::vector<int> offset_;
    std::vector<int> goal_;
    std::vector<lp::LPConstraint> hpom_constraints_;

    utils::Timer hpom_timer_;
    unsigned hpom_num_vars_ = 0;
    unsigned hpom_num_constraints_ = 0;
};

} // namespace i2dual
} // namespace probabilistic
