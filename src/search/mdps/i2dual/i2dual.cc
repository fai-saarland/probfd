#include "i2dual.h"

#include "../../global_state.h"
#include "../../globals.h"
#include "../../operator_cost.h"
#include "../../option_parser.h"
#include "../../plugin.h"
#include "../../solver_interface.h"
#include "../../state_id.h"
#include "../../state_registry.h"
#include "../../utils/system.h"
#include "../../utils/timer.h"
#include "../analysis_objective.h"
#include "../applicable_actions_generator.h"
#include "../globals.h"
#include "../heuristics/occupation_measure/occupation_measure_heuristic.h"
#include "../logging.h"
#include "../new_state_handler.h"
#include "../progress_report.h"
#include "../state_evaluator.h"
#include "../transition_generator.h"

#include <cassert>
#include <iomanip>
#include <iostream>

namespace probabilistic {
namespace i2dual {

enum class IDualStateStatus {
    NEW = 0,
    FRONTIER = 1,
    TERMINAL = 2,
    CLOSED = 3,
};

struct IDualData {
    IDualStateStatus status;
    unsigned constraint;
    std::vector<std::pair<double, unsigned>> incoming;
    double estimate;
    IDualData()
        : status(IDualStateStatus::NEW)
        , constraint(-1)
        , estimate(-1)
    {
    }
    bool is_new() const { return status == IDualStateStatus::NEW; }
    bool is_terminal() const { return status == IDualStateStatus::TERMINAL; }
    bool is_frontier() const { return status == IDualStateStatus::FRONTIER; }
    void open(unsigned c, double est)
    {
        status = IDualStateStatus::FRONTIER;
        constraint = c;
        estimate = est;
    }
    void set_terminal(double val)
    {
        status = IDualStateStatus::TERMINAL;
        estimate = val;
    }
    void close()
    {
        status = IDualStateStatus::CLOSED;
        std::vector<std::pair<double, unsigned>>().swap(incoming);
    }
};

I2Dual::I2Dual(const options::Options& opts)
    : reward_(g_analysis_objective->state_evaluator())
    , report_difference_(opts.get<double>("report_difference"))
    , eval_(opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval"))
    , hpom_(!opts.get<bool>("disable_hpom"))
    , incremental_hpom_updates_(!opts.get<bool>("incremental_updates"))
    , lp_solver_(lp::LPSolverType(opts.get_enum("lpsolver")))
{
}

void
I2Dual::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
        "eval", "", "const");
    parser.add_option<double>("report_difference", "", "1e-4");
    parser.add_option<bool>("disable_hpom", "", "false");
    parser.add_option<bool>("incremental_updates", "", "true");
    lp::add_lp_solver_option_to_parser(parser);
}

void
I2Dual::print_statistics() const
{
}

bool
I2Dual::evaluate_state(const GlobalState& state, IDualData& data)
{
    assert(data.is_new());
    auto x = reward_->operator()(state);
    if ((bool)x) {
        data.set_terminal((value_type::value_t)x);
        return true;
    }
    x = eval_->operator()(state);
    if ((bool)x) {
        data.set_terminal(0);
        return true;
    }
    data.open(next_lp_constraint_id_++, ((value_type::value_t)x));
    return false;
}

void
I2Dual::solve()
{
    lp_solver_timer_.reset();
    lp_solver_timer_.stop();
    hpom_timer_.reset();
    hpom_timer_.stop();

    StateRegistry state_registry(
        g_state_packer, ::g_axiom_evaluator, g_initial_state_values);
    std::vector<std::shared_ptr<Heuristic>> _dummy;
    algorithms::TransitionGenerator<GlobalState, const ProbabilisticOperator*>
        transition_generator(
            &state_registry,
            g_operators,
            _dummy,
            g_step_cost_type,
            g_step_var,
            false);
    algorithms::
        ApplicableActionsGenerator<GlobalState, const ProbabilisticOperator*>
            aops_generator(
                nullptr,
                g_successor_generator.get(),
                g_operators,
                g_step_var,
                g_step_cost_type,
                false);
    algorithms::storage::PerStateStorage<IDualData> idual_data;

    unsigned iterations = 0;
    unsigned expansions = 0;
    unsigned open_states = 0;

    auto report_statistics = [&](const double result) {
        g_log << "*** I2-Dual ***" << std::endl;
        g_log << "Iterations: " << iterations << std::endl;
        g_log << "Expansions: " << expansions << std::endl;
        g_log << "Open states: " << open_states << std::endl;
        g_log << "LP Variables: " << next_lp_var_ << std::endl;
        g_log << "LP Constraints: " << next_lp_constraint_id_ << std::endl;
        g_log << "LP Timer: " << lp_solver_timer_ << std::endl;
        g_log << "hPom LP Variables: " << hpom_num_vars_ << std::endl;
        g_log << "hPom LP Constraints: " << hpom_num_constraints_ << std::endl;
        g_log << "hPom Overhead: " << hpom_timer_ << std::endl;

        g_log << "*** General ***" << std::endl;
        g_log << std::setprecision(
            std::numeric_limits<long double>::digits10 + 1)
              << "Value computed for s0: " << result << std::endl;
        g_log << "Registered state(s): " << state_registry.size() << std::endl;
        aops_generator.get_statistics().print(g_log);
        transition_generator.get_statistics().print(g_log);
    };

    g_log << "Running I2-Dual..." << std::endl;

    prepare_lp();

    {
        GlobalState istate = state_registry.get_initial_state();
        IDualData& idata =
            idual_data[state_registry.get_initial_state().get_id()];
        if (evaluate_state(istate, idata)) {
            report_statistics(idata.estimate);
            return;
        }
    }

    double temp = 1;
    ProgressReport report(report_difference_, g_log);
    report.register_print([&state_registry](std::ostream& out) {
        out << "registered=" << state_registry.size();
    });
    report.register_value("v", [&temp]() { return temp; });

    const double infinity = lp_solver_.get_infinity();

    // Data structures for refining LP
    lp::LPVariable dummy_variable(0, infinity, 0);
    lp::LPConstraint dummy_constraint(-infinity, 0);
    std::vector<int> var_constraint_ids;
    std::vector<double> var_constraint_coefs;
    std::vector<double> obj_coef(next_lp_var_, 0);
    std::vector<double> solution;

    // IDual data structures
    std::vector<StateID> frontier;
    std::vector<StateID> frontier_candidates;
    std::vector<StateID> new_states;
    std::vector<const ProbabilisticOperator*> aops;
    Distribution<GlobalState> succs;

    frontier.push_back(state_registry.get_initial_state().get_id());

    while (!frontier.empty()) {
        report();
        // report_progress(temp);
        // std::cout << "[v=" << temp << "]" << std::endl;

        update_hpom_constraints_expanded(state_registry, idual_data, frontier);

        // Expand each state in frontier
        expansions += frontier.size();
        unsigned start_new_states = frontier_candidates.size();
        for (unsigned i = frontier.size() - 1; i < frontier.size(); i--) {
            const StateID& state = frontier[i];
            IDualData& state_data = idual_data[state];
            assert(state_data.is_frontier());

            // state is expanded -> no longer in frontier -> remove it from
            // LP's objective function
            // if (state != istate) {
            //     lp_solver_.set_constraint_lower_bound(state_data.constraint,
            //     0);
            // }
            if (!hpom_) {
                for (int j = state_data.incoming.size() - 1; j >= 0; --j) {
                    const unsigned& var_id = state_data.incoming[j].second;
                    const double amount =
                        state_data.estimate * state_data.incoming[j].first;
                    obj_coef[var_id] -= amount;
                    assert(obj_coef[var_id] >= -value_type::g_epsilon);
                    lp_solver_.set_objective_coefficient(
                        var_id, std::max(0.0, obj_coef[var_id]));
                }
            }
            state_data.close();

            // generate transitions
            GlobalState statex = state_registry.lookup_state(state);
            aops_generator(statex, aops);
            for (int j = aops.size() - 1; j >= 0; --j) {
                succs = transition_generator(statex, aops[j]);

                if (succs.size() == 1
                    && succs.begin()->first.get_id() == state) {
                    continue;
                }

                unsigned lp_var_id = next_lp_var_++;

                double p_self = 0;
                for (auto it = succs.begin(); it != succs.end(); ++it) {
                    StateID succ_id = it->first.get_id();
                    if (succ_id == state) {
                        p_self += it->second;
                    } else {
                        IDualData& succ_data = idual_data[succ_id];
                        if (succ_data.is_new()
                            && !evaluate_state(it->first, succ_data)) {
                            lp_solver_.add_constraint(dummy_constraint);
                            frontier_candidates.push_back(succ_id);
                        }
                        if (!succ_data.is_terminal()) {
                            assert(
                                succ_data.constraint < next_lp_constraint_id_);
                            var_constraint_ids.push_back(succ_data.constraint);
                            var_constraint_coefs.push_back(-it->second);
                        }
                        if (succ_data.is_frontier()) {
                            succ_data.incoming.emplace_back(
                                it->second, lp_var_id);
#ifndef NDEBUG
                            unsigned x = 0;
                            for (auto it = succ_data.incoming.begin();
                                 it != succ_data.incoming.end();
                                 ++it) {
                                if (it->second == lp_var_id) {
                                    ++x;
                                }
                            }
                            assert(x == 1);
#endif
                        }
                        if (succ_data.is_terminal()
                            || (!hpom_ && succ_data.is_frontier())) {
                            dummy_variable.objective_coefficient +=
                                (it->second * succ_data.estimate);
                        }
                    }
                }
                var_constraint_ids.push_back(state_data.constraint);
                var_constraint_coefs.push_back(1.0 - p_self);
                obj_coef.push_back(dummy_variable.objective_coefficient);

#ifndef NDEBUG
                assert(
                    (int)lp_var_id
                    == lp_solver_.add_variable(
                        dummy_variable,
                        var_constraint_ids,
                        var_constraint_coefs));
#else
                lp_solver_.add_variable(
                    dummy_variable, var_constraint_ids, var_constraint_coefs);
#endif
                dummy_variable.objective_coefficient = 0.0;
                var_constraint_ids.clear();
                var_constraint_coefs.clear();
            }
            aops.clear();
        }
        frontier.clear();

        update_hpom_constraints_frontier(
            state_registry, idual_data, frontier_candidates, start_new_states);

        lp_solver_timer_.resume();
        lp_solver_.solve();
        lp_solver_timer_.stop();

        assert(lp_solver_.has_optimal_solution());
        temp = lp_solver_.get_objective_value();
        solution = lp_solver_.extract_solution();

        unsigned j = 0;
        for (unsigned i = 0; i < frontier_candidates.size(); i++) {
            bool visited = false;
            const IDualData& state_data = idual_data[frontier_candidates[i]];
            for (unsigned j = state_data.incoming.size() - 1;
                 j < state_data.incoming.size();
                 j--) {
                if (solution[state_data.incoming[j].second]
                    > value_type::g_epsilon) {
                    visited = true;
                    break;
                }
            }
            if (visited) {
                frontier.push_back(frontier_candidates[i]);
            } else {
                if (i != j) {
                    frontier_candidates[j] = frontier_candidates[i];
                }
                j++;
            }
        }

        while (frontier_candidates.size() > j)
            frontier_candidates.pop_back();

        lp_solver_.clear_temporary_constraints();

        iterations++;
        open_states = frontier_candidates.size() + frontier.size();
    }

    report_statistics(temp);
}

void
I2Dual::prepare_hpom(
    std::vector<lp::LPVariable>& vars,
    std::vector<lp::LPConstraint>& )
{
    if (!hpom_) {
        return;
    }
    hpom_timer_.resume();
    occupation_measure_heuristic::ProjectionOccupationMeasureHeuristic::
        generate_hpom_lp(lp_solver_, vars, hpom_constraints_, offset_);
    hpom_num_vars_ = vars.size();
    hpom_num_constraints_ = hpom_constraints_.size();
    hpom_timer_.stop();
}

void
I2Dual::update_hpom_constraints_for_state(
    const GlobalState& state,
    const IDualData& var_infos,
    std::vector<lp::LPConstraint>& constraints,
    const double negate) const
{
    for (int var = g_variable_domain.size() - 1; var >= 0; --var) {
        const int val = state[var];
        lp::LPConstraint& c = constraints[offset_[var] + val];
        for (const auto& om : var_infos.incoming) {
            // std::cout << var << ":" << val << ", " << om.second << " -> "
            // << (offset_[var] + val) << std::endl;
            c.insert(om.second, negate * om.first);
        }
    }
}

void
I2Dual::update_hpom_constraints_expanded(
    StateRegistry& registry,
    algorithms::storage::PerStateStorage<IDualData>& data,
    const std::vector<StateID>& expanded)
{
    if (!hpom_) {
        return;
    }
    hpom_timer_.resume();
    if (hpom_initialized_ && incremental_hpom_updates_) {
        for (int i = expanded.size() - 1; i >= 0; --i) {
            const StateID& state_id = expanded[i];
            const IDualData& var_infos = data[state_id];
            GlobalState state = registry.lookup_state(state_id);
            update_hpom_constraints_for_state(
                state, var_infos, hpom_constraints_, -1);
        }
    }
    hpom_initialized_ = true;
    hpom_timer_.stop();
}

void
I2Dual::update_hpom_constraints_frontier(
    StateRegistry& registry,
    algorithms::storage::PerStateStorage<IDualData>& data,
    const std::vector<StateID>& frontier,
    const int start)
{
    if (!hpom_) {
        return;
    }
    hpom_timer_.resume();
    if (incremental_hpom_updates_) {
        for (int i = frontier.size() - 1; i >= start; --i) {
            const StateID& state_id = frontier[i];
            const IDualData& var_infos = data[state_id];
            GlobalState state = registry.lookup_state(state_id);
            update_hpom_constraints_for_state(
                state, var_infos, hpom_constraints_, 1);
        }
        lp_solver_.add_temporary_constraints(hpom_constraints_);
    } else {
        std::vector<lp::LPConstraint> constraints(hpom_constraints_);
        for (int i = frontier.size() - 1; i >= 0; --i) {
            const StateID& state_id = frontier[i];
            // std::cout << "[" << i << "/" << (frontier.size()) << "] : " <<
            // state_id << std::endl;
            const IDualData& var_infos = data[state_id];
            GlobalState state = registry.lookup_state(state_id);
            update_hpom_constraints_for_state(state, var_infos, constraints, 1);
        }
        lp_solver_.add_temporary_constraints(constraints);
    }
    hpom_timer_.stop();
}

void
I2Dual::prepare_lp()
{
    // setup empty LP
    std::vector<lp::LPVariable> vars;
    std::vector<lp::LPConstraint> constr;
    prepare_hpom(vars, constr);

    next_lp_var_ = vars.size();
    next_lp_constraint_id_ = constr.size();

    constr.emplace_back(-lp_solver_.get_infinity(), 1);
    lp_solver_.load_problem(lp::LPObjectiveSense::MAXIMIZE, vars, constr);
}

static Plugin<SolverInterface>
    _plugin("i2dual", options::parse<SolverInterface, I2Dual>);

} // namespace i2dual
} // namespace probabilistic
