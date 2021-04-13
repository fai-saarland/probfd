#pragma once

#include "../../lp/lp_solver.h"
#include "../progress_report.h"
#include "../storage/per_state_storage.h"
#include "engine.h"

#include <set>
#include <vector>

namespace probabilistic {
namespace idual {

struct Statistics {
    unsigned long long iterations = 0;
    unsigned long long expansions = 0;
    unsigned long long open = 0;
    unsigned long long lp_variables = 0;
    unsigned long long lp_constraints = 0;

    void print(std::ostream& out) const
    {
        out << "  Iterations: " << iterations << std::endl;
        out << "  Expansions: " << expansions << std::endl;
        out << "  Open states: " << open << std::endl;
        out << "  LP Variables: " << lp_variables << std::endl;
        out << "  LP Constraints: " << lp_constraints << std::endl;
    }

    void register_prints(ProgressReport* report) const
    {
        report->register_print(
            [this](std::ostream& out) { out << "iteration=" << iterations; });
    }
};

struct PerStateInfo {
    static const uint8_t NEW = 0;
    static const uint8_t OPEN = 1;
    static const uint8_t CLOSED = 2;
    static const uint8_t TERMINAL = 3;

    unsigned idx = (unsigned)-1;
    uint8_t status = NEW;
};

struct FrontierStateInfo {
    std::vector<unsigned> incoming;
};

class ValueGroup {
public:
    explicit ValueGroup()
        : values()
        , indices(Comparator(&values))
    {
    }

    const value_type::value_t& operator[](const unsigned& i) const
    {
        return values[i];
    }

    unsigned get_id(const value_type::value_t& val)
    {
        values.push_back(val);
        auto it = indices.insert(values.size() - 1);
        if (!it.second) {
            values.pop_back();
        }
        return *it.first;
    }

private:
    struct Comparator {
        explicit Comparator(const std::vector<value_type::value_t>* values)
            : values(values)
        {
        }

        bool operator()(const unsigned& x, const unsigned& y) const
        {
            return values->at(x) < values->at(y);
        }

        const std::vector<value_type::value_t>* values;
    };

    std::vector<value_type::value_t> values;
    std::set<unsigned, Comparator> indices;
};

template<typename State, typename Action>
class IDual : public MDPEngine<State, Action> {
public:
    template<typename... Args>
    explicit IDual(
        lp::LPSolverType solver_type,
        StateEvaluator<State>* value_initializer,
        ProgressReport* report,
        Args... args)
        : MDPEngine<State, Action>(args...)
        , report_(report)
        , value_initializer_(value_initializer)
        , dead_end_value_(this->get_minimal_reward())
        , lp_solver_(solver_type)
        , state_infos_()
        , terminals_()
        , statistics_()
    {
    }

    virtual void solve(const State& initial_state) override
    {
        const double eps = value_type::g_epsilon;
        const double inf = lp_solver_.get_infinity();

        StateID prev_state = StateID::undefined;
        unsigned next_var_id = 0;
        unsigned next_constraint_id = 0;
        std::vector<StateID> frontier;
        storage::StateHashMap<FrontierStateInfo> open_states;

        {
            // initialize lp
            auto eval = value_initializer_->operator()(initial_state);
            if ((bool)eval) {
                objective_ = value_type::value_t(eval);
                return;
            }
            lp::LPVariable var((value_type::value_t)eval, inf, 1.0);
            lp_solver_.load_problem(
                lp::LPObjectiveSense::MINIMIZE,
                std::vector<lp::LPVariable>({ var }),
                std::vector<lp::LPConstraint>({ lp::LPConstraint(-inf, inf) }));
            prev_state = this->get_state_id(initial_state);
            PerStateInfo& info = state_infos_[prev_state];
            info.idx = next_var_id;
            info.status = PerStateInfo::CLOSED;
            frontier.push_back(prev_state);
            ++next_var_id;
            ++next_constraint_id;
        }

        std::vector<Action> aops;
        std::vector<Distribution<StateID>> transitions;
        std::vector<double> lp_sol;

        report_->register_value("v", [this] { return objective_; });
        statistics_.register_prints(report_);

        do {
            ++statistics_.iterations;
            statistics_.expansions += frontier.size();

            for (int i = frontier.size() - 1; i >= 0; --i) {
                const StateID state_id = frontier[i];
                const State state = this->lookup_state(state_id);
                auto rew = this->get_state_reward(state);
                const unsigned var_id = state_infos_[state_id].idx;
                assert(state_infos_[state_id].status == PerStateInfo::CLOSED);
                if (rew) {
                    lp_solver_.set_variable_lower_bound(
                        var_id, (value_type::value_t)rew);
                } else {
                    lp_solver_.set_variable_lower_bound(
                        var_id, dead_end_value_);
                    this->generate_all_successors(state_id, aops, transitions);
                    for (unsigned j = 0; j < transitions.size(); ++j) {
                        Distribution<StateID>& transition = transitions[j];
                        transition.make_unique();
                        if (transition.size() == 1
                            && transition.begin()->first == state_id) {
                            continue;
                        }

                        lp::LPConstraint c(-inf, inf);

                        double base_val = (value_type::value_t)rew
                            + this->get_action_reward(state_id, aops[j]);
                        StateID next_prev_state = prev_state;
                        double w = 1.0;

                        for (auto it = transition.begin();
                             it != transition.end();
                             ++it) {
                            const StateID succ_id = it->first;
                            PerStateInfo& succ_info = state_infos_[succ_id];
                            if (succ_id == state_id) {
                                w -= it->second;
                            } else if (succ_id > prev_state) {
                                assert(
                                    state_infos_[succ_id].idx == (unsigned)-1);
                                State succ_state = this->lookup_state(succ_id);
                                auto eval =
                                    value_initializer_->operator()(succ_state);
                                if (eval) {
                                    succ_info.status = PerStateInfo::TERMINAL;
                                    succ_info.idx = terminals_.get_id(
                                        (value_type::value_t)eval);
                                    base_val += it->second
                                        * ((value_type::value_t)eval);
                                } else {
                                    lp_solver_.add_variable(
                                        lp::LPVariable(
                                            (value_type::value_t)eval,
                                            inf,
                                            0.0),
                                        std::vector<int>(),
                                        std::vector<double>());
                                    succ_info.idx = next_var_id;
                                    succ_info.status = PerStateInfo::OPEN;
                                    c.insert(next_var_id, -it->second);
                                    open_states[succ_id].incoming.push_back(
                                        next_constraint_id);
                                    ++next_var_id;
                                }
                                if (succ_id > next_prev_state) {
                                    next_prev_state = succ_id;
                                }
                            } else {
                                assert(succ_info.status != PerStateInfo::NEW);
                                switch (succ_info.status) {
                                case (PerStateInfo::OPEN):
                                    open_states[succ_id].incoming.push_back(
                                        next_constraint_id);
                                    [[fallthrough]];
                                case (PerStateInfo::CLOSED):
                                    c.insert(succ_info.idx, -it->second);
                                    break;
                                default:
                                    base_val +=
                                        it->second * terminals_[succ_info.idx];
                                    break;
                                }
                            }
                        }

                        prev_state = next_prev_state;

                        assert(w > 0);
                        c.insert(var_id, w);
                        c.set_lower_bound(base_val);
                        lp_solver_.add_constraint(c);
                        ++next_constraint_id;
                    }
                    aops.clear();
                    transitions.clear();
                }
            }
            frontier.clear();

            lp_solver_.solve();
            assert(lp_solver_.has_optimal_solution());
            lp_sol = lp_solver_.extract_dual_solution();
            objective_ = lp_solver_.get_objective_value();

            auto it = open_states.begin();
            while (it != open_states.end()) {
                double sum = 0;
                const auto& refs = it->second.incoming;
                for (unsigned i = 0; i < refs.size(); ++i) {
                    sum += lp_sol[refs[i]];
                }
                if (sum > eps) {
                    StateID id(it->first);
                    state_infos_[id].status = PerStateInfo::CLOSED;
                    frontier.emplace_back(id);
                    it = open_states.erase(it);
                } else {
                    ++it;
                }
            }

            report_->operator()();
        } while (!frontier.empty());
        assert(lp_sol.size() > 0);

        statistics_.lp_variables = next_var_id;
        statistics_.lp_constraints = next_constraint_id;
        statistics_.open = open_states.size();
    }

    virtual value_type::value_t get_result(const State& ) override
    {
        return objective_;
    }

private:
    ProgressReport* report_;
    StateEvaluator<State>* value_initializer_;
    const value_type::value_t dead_end_value_;

    lp::LPSolver lp_solver_;
    storage::PerStateStorage<PerStateInfo> state_infos_;
    ValueGroup terminals_;

    Statistics statistics_;

    value_type::value_t objective_;
};

} // namespace idual
} // namespace probabilistic

