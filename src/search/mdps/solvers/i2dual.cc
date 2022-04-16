#include "i2dual.h"

#include "../../globals.h"
#include "../../operator_cost.h"
#include "../../option_parser.h"
#include "../../plugin.h"
#include "../../utils/system.h"
#include "../analysis_objectives/goal_probability_objective.h"
#include "../globals.h"
#include "../heuristics/occupation_measure/occupation_measure_heuristic.h"
#include "../logging.h"

#include <cassert>
#include <iomanip>
#include <iostream>

namespace probabilistic {
namespace solvers {
namespace i2dual {

enum class IDualStateStatus {
    NEW = 0,
    FRONTIER = 1,
    TERMINAL = 2,
    CLOSED = 3,
};

struct IDualData {
    std::vector<std::pair<double, unsigned>> incoming;
    double estimate;
    unsigned constraint;
    IDualStateStatus status;

    explicit IDualData()
        : estimate(-1)
        , constraint(-1)
        , status(IDualStateStatus::NEW)
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
    : progress_(opts.get<double>("report_epsilon"))
    , state_registry_(
          g_state_packer,
          ::g_axiom_evaluator,
          g_initial_state_values)
    , state_reward_function_(g_analysis_objective->state_reward())
    , reward_bound_(g_analysis_objective->reward_bound())
    , transition_generator_(
          g_operators,
          g_successor_generator.get(),
          g_step_var,
          g_step_cost_type,
          &state_registry_,
          false,
          opts.get_list<std::shared_ptr<Heuristic>>(
              "path_dependent_heuristics"))
    , aops_generator_(&transition_generator_)
    , eval_(opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval"))
    , hpom_enabled_(!opts.get<bool>("disable_hpom"))
    , incremental_hpom_updates_(!opts.get<bool>("incremental_updates"))
    , lp_solver_(lp::LPSolverType(opts.get_enum("lpsolver")))
{
}

void I2Dual::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
        "eval",
        "",
        "const");
    parser.add_list_option<std::shared_ptr<Heuristic>>(
        "path_dependent_heuristics",
        "",
        "[]");
    parser.add_option<double>("report_epsilon", "", "1e-4");
    parser.add_option<bool>("disable_hpom", "", "false");
    parser.add_option<bool>("incremental_updates", "", "true");
    lp::add_lp_solver_option_to_parser(parser);
}

void I2Dual::print_statistics() const
{
}

bool I2Dual::evaluate_state(const GlobalState& state, IDualData& data)
{
    assert(data.is_new());
    auto x = state_reward_function_->operator()(state);
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

void I2Dual::solve()
{
    using namespace analysis_objectives;

    lp_solver_timer_.reset();
    lp_solver_timer_.stop();
    hpom_timer_.reset();
    hpom_timer_.stop();

    logging::out << "Initializing I2-Dual..." << std::endl;

    if (std::dynamic_pointer_cast<GoalProbabilityObjective>(
            g_analysis_objective) == nullptr) {
        logging::err
            << "I2-Dual currently only supports goal probability analysis"
            << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }

    if (hpom_enabled_ && (::has_axioms() || ::has_conditional_effects())) {
        logging::err << "hpom doesn't support axioms and conditional effects!"
                     << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }

    storage::PerStateStorage<IDualData> idual_data;

    unsigned iterations = 0;
    unsigned expansions = 0;
    unsigned open_states = 0;

    auto report_statistics = [&]() {
        logging::out << "  Iterations: " << iterations << std::endl;
        logging::out << "  Expansions: " << expansions << std::endl;
        logging::out << "  Open states: " << open_states << std::endl;
        logging::out << "  LP Variables: " << next_lp_var_ << std::endl;
        logging::out << "  LP Constraints: " << next_lp_constraint_id_
                     << std::endl;
        logging::out << "  LP Timer: " << lp_solver_timer_ << std::endl;
        logging::out << "  hPom LP Variables: " << hpom_num_vars_ << std::endl;
        logging::out << "  hPom LP Constraints: " << hpom_num_constraints_
                     << std::endl;
        logging::out << "  hPom Overhead: " << hpom_timer_ << std::endl;
    };

    logging::out << std::endl << "Running MDP engine I2-Dual..." << std::endl;
    utils::Timer idual_timer;

    prepare_lp();

    double temp = 1;
    progress_.register_print([this](std::ostream& out) {
        out << "registered=" << state_registry_.size();
    });
    progress_.register_value("v", [&temp]() { return temp; });

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
    Distribution<StateID> succs;

    {
        GlobalState istate = state_registry_.get_initial_state();
        IDualData& idata =
            idual_data[state_registry_.get_initial_state().get_id().hash()];
        if (!evaluate_state(istate, idata)) {
            frontier.push_back(
                state_registry_.get_initial_state().get_id().hash());
        }
    }

    while (!frontier.empty()) {
        progress_();
        // report_progress(temp);
        // std::cout << "[v=" << temp << "]" << std::endl;

        update_hpom_constraints_expanded(idual_data, frontier);

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
            if (!hpom_enabled_) {
                for (int j = state_data.incoming.size() - 1; j >= 0; --j) {
                    const unsigned& var_id = state_data.incoming[j].second;
                    const double amount =
                        state_data.estimate * state_data.incoming[j].first;
                    obj_coef[var_id] -= amount;
                    assert(obj_coef[var_id] >= -value_type::g_epsilon);
                    lp_solver_.set_objective_coefficient(
                        var_id,
                        std::max(0.0, obj_coef[var_id]));
                }
            }
            state_data.close();

            // generate transitions
            aops_generator_(state, aops);
            for (int j = aops.size() - 1; j >= 0; --j) {
                succs.clear();
                transition_generator_(state, aops[j], succs);
                succs.make_unique();

                if (succs.size() == 1 && succs.begin()->first == state) {
                    continue;
                }

                unsigned lp_var_id = next_lp_var_++;

                double p_self = 0;
                for (auto it = succs.begin(); it != succs.end(); ++it) {
                    StateID succ_id = it->first;
                    if (succ_id == state) {
                        p_self += it->second;
                    } else {
                        IDualData& succ_data = idual_data[succ_id];
                        if (succ_data.is_new() &&
                            !evaluate_state(
                                state_registry_.lookup_state(
                                    ::StateID(it->first)),
                                succ_data)) {
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
                                it->second,
                                lp_var_id);
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
                        if (succ_data.is_terminal() ||
                            (!hpom_enabled_ && succ_data.is_frontier())) {
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
                    (int)lp_var_id == lp_solver_.add_variable(
                                          dummy_variable,
                                          var_constraint_ids,
                                          var_constraint_coefs));
#else
                lp_solver_.add_variable(
                    dummy_variable,
                    var_constraint_ids,
                    var_constraint_coefs);
#endif
                dummy_variable.objective_coefficient = 0.0;
                var_constraint_ids.clear();
                var_constraint_coefs.clear();
            }
            aops.clear();
        }
        frontier.clear();

        update_hpom_constraints_frontier(
            idual_data,
            frontier_candidates,
            start_new_states);

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
                if (solution[state_data.incoming[j].second] >
                    value_type::g_epsilon) {
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

    idual_timer.stop();

    logging::out << "analysis done. [t=" << utils::g_timer << "]" << std::endl;

    logging::out << std::endl;
    logging::print_analysis_result(temp);

    logging::out << std::endl;
    logging::out << "State space interface:" << std::endl;
    logging::out << "  Registered state(s): " << state_registry_.size()
                 << std::endl;
    transition_generator_.print_statistics(logging::out);

    logging::out << std::endl;
    logging::out << "Engine I2-Dual statistics:" << std::endl;
    logging::out << "  Actual solver time: " << idual_timer << std::endl;
    report_statistics();
}

void I2Dual::prepare_hpom(
    std::vector<lp::LPVariable>& vars,
    std::vector<lp::LPConstraint>&)
{
    if (!hpom_enabled_) {
        return;
    }
    hpom_timer_.resume();
    occupation_measure_heuristic::ProjectionOccupationMeasureHeuristic::
        generate_hpom_lp(lp_solver_, vars, hpom_constraints_, offset_);
    hpom_num_vars_ = vars.size();
    hpom_num_constraints_ = hpom_constraints_.size();
    // constraints.insert(
    //     constraints.end(), hpom_constraints_.begin(),
    //     hpom_constraints_.end());
    hpom_timer_.stop();
}

void I2Dual::add_fringe_state_to_hpom(
    const GlobalState& state,
    const IDualData& var_infos,
    std::vector<lp::LPConstraint>& constraints) const
{
    for (int var = g_variable_domain.size() - 1; var >= 0; --var) {
        const int val = state[var];
        lp::LPConstraint& c = constraints[offset_[var] + val];
        for (const auto& om : var_infos.incoming) {
            // std::cout << var << ":" << val << ", " << om.second << " -> "
            // << (offset_[var] + val) << std::endl;
            c.insert(om.second, om.first);
        }
    }
}

void I2Dual::remove_fringe_state_from_hpom(
    const GlobalState& state,
    const IDualData& var_infos,
    std::vector<lp::LPConstraint>& constraints) const
{
    for (int var = g_variable_domain.size() - 1; var >= 0; --var) {
        const int val = state[var];
        lp::LPConstraint& c = constraints[offset_[var] + val];
        for (const auto& om : var_infos.incoming) {
#ifndef NDEBUG
            const double coef = c.remove(om.second);
            assert(std::abs(coef - om.first) <= 1e-6);
#else
            c.remove(om.second);
#endif
            // std::cout << var << ":" << val << ", " << om.second << " -> "
            // << (offset_[var] + val) << std::endl;
        }
    }
}

void I2Dual::update_hpom_constraints_expanded(
    storage::PerStateStorage<IDualData>& data,
    const std::vector<StateID>& expanded)
{
    if (!hpom_enabled_ || !incremental_hpom_updates_) {
        return;
    }
    if (hpom_initialized_) {
        hpom_timer_.resume();
        for (int i = expanded.size() - 1; i >= 0; --i) {
            const StateID& state_id = expanded[i];
            const IDualData& var_infos = data[state_id];
            GlobalState state =
                state_registry_.lookup_state(::StateID(state_id));
            remove_fringe_state_from_hpom(state, var_infos, hpom_constraints_);
        }
        hpom_timer_.stop();
    }
    hpom_initialized_ = true;
}

void I2Dual::update_hpom_constraints_frontier(
    storage::PerStateStorage<IDualData>& data,
    const std::vector<StateID>& frontier,
    const int start)
{
    if (!hpom_enabled_) {
        return;
    }
    hpom_timer_.resume();
    if (incremental_hpom_updates_) {
        for (int i = frontier.size() - 1; i >= start; --i) {
            const StateID& state_id = frontier[i];
            const IDualData& var_infos = data[state_id];
            GlobalState state =
                state_registry_.lookup_state(::StateID(state_id));
            add_fringe_state_to_hpom(state, var_infos, hpom_constraints_);
        }
        lp_solver_.add_temporary_constraints(hpom_constraints_);
    } else {
        std::vector<lp::LPConstraint> constraints(hpom_constraints_);
        for (int i = frontier.size() - 1; i >= 0; --i) {
            const StateID& state_id = frontier[i];
            // std::cout << "[" << i << "/" << (frontier.size()) << "] : " <<
            // state_id << std::endl;
            const IDualData& var_infos = data[state_id];
            GlobalState state =
                state_registry_.lookup_state(::StateID(state_id));
            add_fringe_state_to_hpom(state, var_infos, constraints);
        }
        lp_solver_.add_temporary_constraints(constraints);
    }
    hpom_timer_.stop();
}

void I2Dual::prepare_lp()
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
} // namespace solvers
} // namespace probabilistic
