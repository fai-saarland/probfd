#include "strips.h"

#include "global_operator.h"
#include "global_state.h"
#include "globals.h"
#include "partial_state.h"

#include <algorithm>
#include <sstream>

namespace strips_utils {

std::shared_ptr<STRIPS> STRIPS::task_instance_ = nullptr;

STRIPS::Action::Action(unsigned id, unsigned op)
    : id(id)
    , original_operator(op)
{
}

STRIPS::STRIPS()
{
    ::verify_no_axioms_no_conditional_effects();

    facts_ = 0;
    variable_offset_.resize(g_variable_domain.size() + 1, 0);
    for (unsigned var = 0; var < variable_offset_.size() - 1; var++) {
        variable_offset_[var] = facts_;
        facts_ += g_variable_domain[var];
    }
    variable_offset_.back() = facts_;

    for (int i = g_goal.size() - 1; i >= 0; i--) {
        const auto& p = g_goal[i];
        goal_facts_.push_back(variable_offset_[p.first] + p.second);
    }
    std::sort(goal_facts_.begin(), goal_facts_.end());

    mutexes_.resize(facts_, std::vector<bool>(facts_, false));
    std::vector<std::vector<unsigned>> mutex_with(facts_);
    for (int var = g_variable_domain.size() - 1; var >= 0; var--) {
        for (int val1 = 0; val1 < g_variable_domain[var] - 1; val1++) {
            for (int val2 = val1 + 1; val2 < g_variable_domain[var]; val2++) {
                const Fact p = variable_offset_[var] + val1;
                const Fact q = variable_offset_[var] + val2;
                mutexes_[p][q] = true;
                mutexes_[q][p] = true;
                mutex_with[p].push_back(q);
                mutex_with[q].push_back(p);
            }
        }
        for (int var = g_inconsistent_facts.size() - 1; var >= 0; var--) {
            for (int val = g_inconsistent_facts[var].size() - 1; val >= 0;
                 val--) {
                const Fact p = variable_offset_[var] + val;
                for (auto it = g_inconsistent_facts[var][val].begin();
                     it != g_inconsistent_facts[var][val].end();
                     ++it) {
                    const Fact q = variable_offset_[it->first] + it->second;
                    mutexes_[p][q] = true;
                    mutexes_[q][p] = true;
                    mutex_with[p].push_back(q);
                    mutex_with[q].push_back(p);
                }
            }
        }
    }

    actions_.reserve(g_operators.size());
    actions_that_add_.resize(facts_);
    actions_that_del_.resize(facts_);
    actions_with_mutex_regression_.resize(facts_);
    actions_with_mutex_progression_.resize(facts_);

    std::vector<bool> regression_is_mutex_with(facts_);
    std::vector<bool> progression_is_mutex_with(facts_);
    std::vector<int> precondition(g_variable_domain.size(), -1);
    unsigned action_index = 0;
    for (unsigned opid = 0; opid < g_operators.size(); opid++) {
        const GlobalOperator& g_op = g_operators[opid];
        if (g_op.get_effects().empty()) {
            continue;
        }

        std::fill(precondition.begin(), precondition.end(), -1);
        std::fill(
            regression_is_mutex_with.begin(),
            regression_is_mutex_with.end(),
            false);
        std::fill(
            progression_is_mutex_with.begin(),
            progression_is_mutex_with.end(),
            false);

        actions_.emplace_back(action_index, opid);
        STRIPS::Action& s_op = actions_[action_index];

        s_op.cost = g_op.get_cost();

        {
            const auto& pres = g_op.get_preconditions();
            for (int i = pres.size() - 1; i >= 0; i--) {
                const auto& pre = pres[i];
                precondition[pre.var] = pre.val;
                const Fact p = variable_offset_[pre.var] + pre.val;
                s_op.pre.push_back(p);
                for (const auto& q : mutex_with[p]) {
                    regression_is_mutex_with[q] = true;
                    progression_is_mutex_with[q] = true;
                }
            }
        }

        {
            const auto& effs = g_op.get_effects();
            for (int i = effs.size() - 1; i >= 0; --i) {
                const auto& eff = effs[i];
                const Fact p = variable_offset_[eff.var] + eff.val;
                s_op.add.push_back(p);
                actions_that_add_[p].push_back(action_index);
                regression_is_mutex_with[p] = false;
                if (precondition[eff.var] != -1) {
                    const Fact q =
                        variable_offset_[eff.var] + precondition[eff.var];
                    s_op.del.push_back(q);
                    actions_that_del_[q].push_back(action_index);
                } else {
                    for (int val = 0; val < g_variable_domain[eff.var]; ++val) {
                        if (val != eff.val) {
                            const Fact q = variable_offset_[eff.var] + val;
                            s_op.del.push_back(q);
                            actions_that_del_[q].push_back(action_index);
                        }
                    }
                }
            }
        }

        std::sort(s_op.pre.begin(), s_op.pre.end());
        std::sort(s_op.add.begin(), s_op.add.end());
        std::sort(s_op.del.begin(), s_op.del.end());

        for (Fact p = 0; p < regression_is_mutex_with.size(); p++) {
            if (regression_is_mutex_with[p]) {
                actions_with_mutex_regression_[p].push_back(action_index);
            }
        }

        for (const Fact& p : s_op.add) {
            for (const Fact& q : mutex_with[p]) {
                progression_is_mutex_with[q] = true;
            }
        }

        for (const Fact& p : s_op.del) {
            progression_is_mutex_with[p] = false;
        }

        for (Fact p = 0; p < progression_is_mutex_with.size(); p++) {
            if (progression_is_mutex_with[p]) {
                actions_with_mutex_progression_[p].push_back(action_index);
            }
        }

        action_index++;
    }
}

std::shared_ptr<STRIPS>
STRIPS::get_task()
{
    if (task_instance_ == nullptr) {
        task_instance_ = std::shared_ptr<STRIPS>(new STRIPS());
    }
    return task_instance_;
}

unsigned
STRIPS::num_facts() const
{
    return facts_;
}

const FactSet&
STRIPS::get_goal_facts() const
{
    return goal_facts_;
}

Fact
STRIPS::get_fact(const std::pair<int, int>& x) const
{
    return variable_offset_[x.first] + x.second;
}

Fact
STRIPS::get_fact(int var, int val) const
{
    return variable_offset_[var] + val;
}

std::pair<int, int>
STRIPS::get_variable_assignment(Fact p) const
{
    auto it =
        std::upper_bound(variable_offset_.begin(), variable_offset_.end(), p);
    int var = std::distance(variable_offset_.begin(), it) - 1;
    return std::pair<int, int>(var, p - variable_offset_[var]);
}

void
STRIPS::copy_to_partial_state(const FactSet& fact_set, PartialState& result)
    const
{
    result.clear();
    if (fact_set.empty()) {
        return;
    }
    auto it = variable_offset_.begin();
    for (unsigned i = 0; i < fact_set.size(); ++i) {
        it = std::upper_bound(it, variable_offset_.end(), fact_set[i]);
        int var = std::distance(variable_offset_.begin(), it) - 1;
        result[var] = fact_set[i] - variable_offset_[var];
    }
}

bool
STRIPS::contains_mutex(const FactSet& subgoal) const
{
    for (unsigned i = 0; i < subgoal.size() - 1; i++) {
        assert(subgoal[i] < num_facts());
        const std::vector<bool>& mutexes = mutexes_[subgoal[i]];
        for (unsigned j = i + 1; j < subgoal.size(); j++) {
            assert(subgoal[j] < num_facts());
            if (mutexes[subgoal[j]]) {
                return true;
            }
        }
    }
    return false;
}

bool
STRIPS::get_mutex(const FactSet& subgoal, FactSet& conflict) const
{
    for (unsigned i = 0; i < subgoal.size() - 1; i++) {
        const std::vector<bool>& mutexes = mutexes_[subgoal[i]];
        for (unsigned j = i + 1; j < subgoal.size(); j++) {
            if (mutexes[subgoal[j]]) {
                conflict.push_back(subgoal[i]);
                conflict.push_back(subgoal[j]);
                return true;
            }
        }
    }
    return false;
}

bool
STRIPS::are_mutex(Fact p, Fact q) const
{
    return mutexes_[p][q];
}

bool
STRIPS::are_mutex(const FactSet& x, const FactSet& y) const
{
    for (unsigned i = 0; i < x.size(); i++) {
        const auto& mutexes = mutexes_[x[i]];
        for (unsigned j = 0; j < y.size(); j++) {
            if (mutexes[y[j]]) {
                return true;
            }
        }
    }
    return false;
}

bool
STRIPS::are_mutex(const Fact& x, const FactSet& y) const
{
    for (const Fact& z : y) {
        if (mutexes_[x][z]) {
            return true;
        }
    }
    return false;
}

unsigned
STRIPS::num_actions() const
{
    return actions_.size();
}

const std::vector<STRIPS::Action>&
STRIPS::get_actions() const
{
    return actions_;
}

const STRIPS::Action&
STRIPS::get_operator(unsigned idx) const
{
    return actions_[idx];
}

const std::vector<unsigned>&
STRIPS::get_actions_that_delete(Fact p) const
{
    return actions_that_del_[p];
}

const std::vector<unsigned>&
STRIPS::get_actions_that_add(Fact p) const
{
    return actions_that_add_[p];
}

const std::vector<unsigned>&
STRIPS::get_mutex_actions(Fact p) const
{
    return actions_with_mutex_regression_[p];
}

const std::vector<unsigned>&
STRIPS::get_actions_with_mutex_progression(Fact p) const
{
    return actions_with_mutex_progression_[p];
}

void
STRIPS::compute_is_regressable(const FactSet& conj, std::vector<int>& achievers)
    const
{
    assert(achievers.size() == num_actions());
    for (const Fact& p : conj) {
        for (const unsigned& op : actions_with_mutex_regression_[p]) {
            achievers[op] = -1;
        }
        for (const unsigned& op : actions_that_del_[p]) {
            achievers[op] = -1;
        }
        for (const unsigned& op : actions_that_add_[p]) {
            if (achievers[op] == 0) {
                achievers[op] = 1;
            }
        }
    }
#if !defined(NDEBUG) && !defined(NDEBUG_EXPENSIVE)
    for (unsigned i = 0; i < num_actions(); i++) {
        const Action& action = actions_[i];
        FactSet aux;
        std::set_intersection(
            conj.begin(),
            conj.end(),
            action.add.begin(),
            action.add.end(),
            std::back_inserter(aux));
        bool adds = !aux.empty();
        aux.clear();
        assert(adds || achievers[i] != 1);
        if (adds) {
            std::set_intersection(
                conj.begin(),
                conj.end(),
                action.del.begin(),
                action.del.end(),
                std::back_inserter(aux));
            bool dels = !aux.empty();
            aux.clear();
            assert(!dels || achievers[i] != 1);
            if (!dels) {
                std::set_difference(
                    conj.begin(),
                    conj.end(),
                    action.add.begin(),
                    action.add.end(),
                    std::back_inserter(aux));
                aux.insert(aux.end(), action.pre.begin(), action.pre.end());
                std::sort(aux.begin(), aux.end());
                aux.erase(std::unique(aux.begin(), aux.end()), aux.end());
                bool mutex = contains_mutex(aux);
                assert(mutex || achievers[i] == 1);
            }
        }
    }
#endif
}

void
STRIPS::compute_is_progressable(const FactSet& conj, std::vector<int>& result)
    const
{
    assert(result.size() == num_actions());
    std::fill(result.begin(), result.end(), 0);
    for (const Fact& p : conj) {
        for (const unsigned& op : actions_with_mutex_progression_[p]) {
            result[op] = -1;
        }
        for (const unsigned& op : actions_that_del_[p]) {
            if (result[op] != -1) {
                result[op] = 1;
            }
        }
    }
}

FactSet
STRIPS::get_fact_set(const GlobalState& state) const
{
    FactSet fact_set;
    for (unsigned var = 0; var < variable_offset_.size() - 1; var++) {
        fact_set.push_back(variable_offset_[var] + state[var]);
    }
    return fact_set;
}

FactSet
STRIPS::get_fact_set(const PartialState& state) const
{
    FactSet fact_set;
    for (unsigned var = 0; var < variable_offset_.size() - 1; var++) {
        if (state.is_defined(var)) {
            fact_set.push_back(variable_offset_[var] + state[var]);
        } else {
            for (int val = 0; val < g_variable_domain[var]; val++) {
                fact_set.push_back(variable_offset_[var] + val);
            }
        }
    }
    return fact_set;
}

FactSet
STRIPS::Action::regress(const FactSet& fact_set) const
{
    FactSet result;
    std::set_difference(
        fact_set.begin(),
        fact_set.end(),
        add.begin(),
        add.end(),
        std::back_inserter(result));
    unsigned middle = result.size();
    std::set_difference(
        pre.begin(),
        pre.end(),
        fact_set.begin(),
        fact_set.end(),
        std::back_inserter(result));
    std::inplace_merge(result.begin(), result.begin() + middle, result.end());
    assert(std::is_sorted(result.begin(), result.end()));
    assert(std::unique(result.begin(), result.end()) == result.end());
    return result;
}

FactSet
STRIPS::Action::progress(const FactSet& fact_set) const
{
    FactSet result;
    std::set_difference(
        fact_set.begin(),
        fact_set.end(),
        del.begin(),
        del.end(),
        std::back_inserter(result));
    unsigned middle = result.size();
    std::set_difference(
        add.begin(),
        add.end(),
        fact_set.begin(),
        fact_set.end(),
        std::back_inserter(result));
    std::inplace_merge(result.begin(), result.begin() + middle, result.end());
    assert(std::is_sorted(result.begin(), result.end()));
    assert(std::unique(result.begin(), result.end()) == result.end());
    return result;
}

std::string
STRIPS::to_string(const Fact& p) const
{
    auto x = get_variable_assignment(p);
    return g_fact_names[x.first][x.second];
}

std::string
STRIPS::to_string(const FactSet& fact_set) const
{
    std::ostringstream out;
    out << "[";
    for (unsigned i = 0; i < fact_set.size(); i++) {
        out << (i > 0 ? " | " : "") << to_string(fact_set[i]);
    }
    out << "]";
    return out.str();
}

Fact
STRIPS::parse_fact_string(const std::string& fact) const
{
    for (int var = g_variable_domain.size() - 1; var >= 0; var--) {
        for (int val = g_variable_domain[var] - 1; val >= 0; val--) {
            if (g_fact_names[var][val] == fact) {
                return get_fact(var, val);
            }
        }
    }
    assert(false);
    return -1;
}

FactSet
STRIPS::parse_fact_set_string(const std::string& fact_set) const
{
    FactSet result;
    for (unsigned var = 0; var < g_variable_domain.size(); ++var) {
        for (int val = 0; val < g_variable_domain[var]; ++val) {
            if (fact_set.find(g_fact_names[var][val]) != std::string::npos) {
                result.push_back(get_fact(var, val));
            }
        }
    }
    return result;
}

} // namespace strips_utils
