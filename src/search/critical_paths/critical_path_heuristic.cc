#include "critical_path_heuristic.h"

#include "../algorithms/priority_queues.h"
#include "../global_state.h"
#include "../globals.h"
#include "../operator_cost.h"
#include "../option_parser.h"
#include "../partial_state.h"
#include "../plugin.h"
#include "../strips.h"
#include "../utils/hash.h"
#include "../utils/timer.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <unordered_map>

namespace critical_path_heuristic {

ConjunctionInfo::ConjunctionInfo()
    : id(-1)
    , cost(UNACHIEVED)
{
}

CounterInfo::CounterInfo()
    : id(-1)
    , unsat(0)
{
}

CriticalPathHeuristic::CriticalPathHeuristic(const options::Options& opts)
    : Heuristic(opts)
    , prune_dominated_preconditions_(
          opts.get<bool>("prune_dominated_preconditions"))
    , prune_dominated_achievers_(opts.get<bool>("prune_dominated_achievers"))
    , is_unit_cost_(cost_type == ONE || ::is_unit_cost())
    , early_termination_(true)
    , store_conjunctions_(
          opts.contains("out") ? opts.get<std::string>("out") : "")
    , task_(strips_utils::STRIPS::get_task())
    , conjunctions_(task_->num_facts())
    , num_removed_precondition_relations_(0)
    , num_removed_effect_relations_(0)
    , num_removed_counters_(0)
{
    ::utils::Timer t_init;
    std::cout << "Initializaing critical-path heuristic ..." << std::endl;

    ::verify_no_axioms_no_conditional_effects();

    std::cout << "Cost variant: " << (is_unit_cost_ ? "unit" : "general")
              << std::endl;
    std::cout << "Pruning dominated preconditions: "
              << prune_dominated_preconditions_ << std::endl;
    std::cout << "Pruning dominated achievers: " << prune_dominated_achievers_
              << std::endl;

    initialize_with_unit_conjunctions();
    num_unit_conjunctions_ = conjunctions_.size();
    num_fact_counters_ = num_counters();
    num_fact_counters_effects_ = num_counter_effects();
    for (int m = 2;
         m <= std::min(
             opts.get<int>("m"), static_cast<int>(g_variable_domain.size()));
         ++m) {
        add_m_conjunctions(m);
    }
    if (opts.contains("in")) {
        std::ifstream inf(opts.get<std::string>("in"));
        if (inf.good()) {
            std::string line;
            while (std::getline(inf, line)) {
                add_conjunction(task_->parse_fact_set_string(line));
            }
        }
    }
    print_construction_statistics();
    std::cout << "initialized critical-path heuristic in " << t_init
              << " [t=" << ::utils::g_timer << "]" << std::endl;
}

void
CriticalPathHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<int>("m", "", "1");
    parser.add_option<bool>("prune_dominated_preconditions", "", "true");
    parser.add_option<bool>("prune_dominated_achievers", "", "true");
    parser.add_option<std::string>("out", "", options::OptionParser::NONE);
    parser.add_option<std::string>("in", "", options::OptionParser::NONE);
    Heuristic::add_options_to_parser(parser);
}

bool
CriticalPathHeuristic::supports_partial_state_evaluation() const
{
    return true;
}

double
CriticalPathHeuristic::counter_ratio() const
{
    return static_cast<double>(counter_infos_.size()) / num_fact_counters_;
}

double
CriticalPathHeuristic::counter_effect_ratio() const
{
    return static_cast<double>(num_counter_effects())
        / num_fact_counters_effects_;
}

double
CriticalPathHeuristic::conjunction_ratio() const
{
    return static_cast<double>(conjunction_infos_.size())
        / num_unit_conjunctions_;
}

unsigned
CriticalPathHeuristic::num_conjunctions() const
{
    return conjunction_infos_.size();
}

unsigned
CriticalPathHeuristic::num_counters() const
{
    return counter_infos_.size();
}

unsigned
CriticalPathHeuristic::num_base_counters() const
{
    return num_fact_counters_;
}

std::shared_ptr<strips_utils::STRIPS>
CriticalPathHeuristic::get_task() const
{
    return task_;
}

unsigned
CriticalPathHeuristic::num_counter_effects() const
{
    unsigned result = 0;
    for (int i = counter_infos_.size() - 1; i >= 0; i--) {
        result += counter_infos_[i].achieving.size();
    }
    return result;
}

unsigned
CriticalPathHeuristic::num_no_effect_counters() const
{
    unsigned res = 0;
    for (int i = counter_infos_.size() - 1; i >= 0; i--) {
        if (i != static_cast<int>(goal_counter_)
            && counter_infos_[i].achieving.empty()) {
            res++;
        }
    }
    return res;
}

CounterInfo*
CriticalPathHeuristic::get_counter_info(unsigned counter_id)
{
    return &counter_infos_[counter_id];
}

const CounterInfo*
CriticalPathHeuristic::get_counter_info(unsigned counter_id) const
{
    return &counter_infos_[counter_id];
}

ConjunctionInfo*
CriticalPathHeuristic::get_conjunction_info(unsigned conjunction_id)
{
    return &conjunction_infos_[conjunction_id];
}

const ConjunctionInfo*
CriticalPathHeuristic::get_conjunction_info(unsigned conjunction_id) const
{
    return &conjunction_infos_[conjunction_id];
}

void
CriticalPathHeuristic::print_size_info(std::ostream& out) const
{
    out << "hC has " << num_conjunctions() << " (" << conjunction_ratio()
        << ") conjunctions and " << num_counters() << " (" << counter_ratio()
        << ") counter with " << num_counter_effects() << " ("
        << counter_effect_ratio() << ") effect relations." << std::endl;
}

void
CriticalPathHeuristic::print_construction_statistics(std::ostream& out) const
{
    // out << "**** hC ****" << std::endl;
    out << "Facts: " << task_->num_facts() << std::endl;
    out << "Conjunctions: " << num_conjunctions() << " (" << conjunction_ratio()
        << ")" << std::endl;
    out << "Counters: " << num_counters() << " (" << counter_ratio() << ")"
        << std::endl;
    out << "Counter effects: " << num_counter_effects() << " ("
        << counter_effect_ratio() << ")" << std::endl;
    out << "Counters without effects: " << num_no_effect_counters()
        << std::endl;
    out << "Simplified counter preconditions: "
        << num_removed_precondition_relations_ << std::endl;
    out << "Simplified counter effects: " << num_removed_effect_relations_
        << std::endl;
    out << "Skipped counters: " << num_removed_counters_ << std::endl;
}

void
CriticalPathHeuristic::print_statistics() const
{
    std::cout << "**** Critical-Path Heuristic ****" << std::endl;
    print_construction_statistics(std::cout);
    if (store_conjunctions_ != "") {
        std::ofstream out;
        out.open(store_conjunctions_);
        for (unsigned conj_id = num_unit_conjunctions_;
             conj_id < num_conjunctions();
             conj_id++) {
            out << task_->to_string(conjunctions_[conj_id]) << std::endl;
        }
        out.close();
    }
}

void
CriticalPathHeuristic::initialize_with_unit_conjunctions()
{
    strips_utils::FactIndexedVector<unsigned> fact_to_id(task_->num_facts());
    {
        conjunction_infos_.resize(task_->num_facts() + 1);
        conjunctions_.insert({});
        true_conjunction_ = 0;
        conjunction_infos_[0].id = 0;
        unsigned id = 1;
        for (auto p = task_->fact_begin(); p != task_->fact_end(); ++p) {
#if !defined(NDEBUG)
            assert(id == conjunctions_.insert({ *p }).first);
#else
            conjunctions_.insert({ *p });
#endif
            conjunction_infos_[id].id = id;
            fact_to_id[*p] = id++;
        }
    }

    auto conj_ptr_action_pair_less = [this](const auto& x, const auto& y) {
        if (x.first->id < y.first->id) {
            return true;
        } else if (x.first->id == y.first->id) {
            if (action_costs_[x.second] < action_costs_[y.second]) {
                return true;
            } else {
                return action_costs_[x.second] == action_costs_[y.second]
                    && x.second < y.second;
            }
        }
        return false;
    };

    {
        unsigned id = 0;
        const auto& actions = task_->get_actions();
        action_costs_.resize(actions.size() + 1, 0);
        for (unsigned i = 0; i < actions.size(); ++i) {
            const auto& a = actions[i];
            action_costs_[a.id] = ::get_adjusted_action_cost(a.cost, cost_type);
            CounterInfo& counter = counter_infos_[a.pre];
            const bool is_new = counter.id >= id;
            // #if !defined(NDEBUG)
            //             if (!is_new && counter.regression != a.pre) {
            //                 std::cout << "~~~~~~ ERROR ~~~~~~ " <<
            //                 std::endl; std::cout << "action = " <<
            //                 g_operators[actions[i].original_operator].get_name()
            //                           << std::endl;
            //                 std::cout << "precondition = " <<
            //                 task_->to_string(a.pre)
            //                           << std::endl;
            //                 std::cout << "counter.regression = "
            //                           <<
            //                           task_->to_string(counter.regression)
            //                           << std::endl;
            //             }
            // #endif
            assert(is_new || counter.regression == a.pre);
            if (is_new) {
                counter.id = id++;
                counter.regression = a.pre;
                for (const auto& p : a.pre) {
                    const unsigned conj = fact_to_id[p];
                    conjunction_infos_[conj].in_regression_of.push_back(
                        &counter);
                    counter.regression_conjunction_ids.push_back(conj);
                }
                if (counter.regression_conjunction_ids.empty()) {
                    counter.regression_conjunction_ids.push_back(
                        true_conjunction_);
                    conjunction_infos_[true_conjunction_]
                        .in_regression_of.push_back(&counter);
                }
            }
#if !defined(NDEBUG)
            else {
                assert(!counter.achieving.empty());
            }
#endif
            for (const auto& p : a.add) {
                counter.achieving.emplace_back(
                    &conjunction_infos_[fact_to_id[p]], a.id);
            }
        }
        for (unsigned i = 0; i < id; i++) {
            CounterInfo& info = counter_infos_[i];
            std::sort(
                info.achieving.begin(),
                info.achieving.end(),
                conj_ptr_action_pair_less);
            unsigned prev_size = info.achieving.size();
            info.achieving.erase(
                std::unique(
                    info.achieving.begin(),
                    info.achieving.end(),
                    [](const auto& x, const auto& y) {
                        return x.first == y.first;
                    }),
                info.achieving.end());
            num_removed_effect_relations_ +=
                (prev_size - info.achieving.size());
            for (int j = info.achieving.size() - 1; j >= 0; j--) {
                const auto& x = info.achieving[j];
                ConjunctionInfo& conj = *x.first;
                conj.achievers.emplace_back(&info, x.second);
            }
        }
    }
    if (prune_dominated_achievers_) {
        for (int i = conjunction_infos_.size() - 1; i >= 0; --i) {
            auto& info = conjunction_infos_[i];
            auto it = info.achievers.begin();
            unsigned j = 0;
            while (j < info.achievers.size()) {
                bool dominated = false;
                for (int k = j - 1; k >= 0; k--) {
                    auto& other = info.achievers[k];
                    CounterInfo& counter_j = *it->first;
                    CounterInfo& counter_k = *other.first;
                    const auto& rj = counter_j.regression_conjunction_ids;
                    const auto& rk = counter_k.regression_conjunction_ids;
                    if (rj.size() < rk.size()) {
                        if (action_costs_[it->second]
                                <= action_costs_[other.second]
                            && std::includes(
                                rk.begin(), rk.end(), rj.begin(), rj.end())) {
                            auto k_it = std::lower_bound(
                                counter_k.achieving.begin(),
                                counter_k.achieving.end(),
                                std::pair<ConjunctionInfo*, unsigned>(
                                    &info, other.second),
                                conj_ptr_action_pair_less);
                            assert(k_it != counter_k.achieving.end());
                            assert(
                                k_it->first == &info
                                && k_it->second == other.second);
                            counter_k.achieving.erase(k_it);
                            info.achievers.erase(info.achievers.begin() + k);
                            j--;
                            it = info.achievers.begin() + j;
                            num_removed_effect_relations_++;
                        }
                    } else if (rj.size() > rj.size()) {
                        if (action_costs_[other.second]
                                <= action_costs_[it->second]
                            && std::includes(
                                rj.begin(), rj.end(), rk.begin(), rk.end())) {
                            dominated = true;
                            break;
                        }
                    }
                }
                if (dominated) {
                    CounterInfo& counter_j = *it->first;
                    auto j_it = std::lower_bound(
                        counter_j.achieving.begin(),
                        counter_j.achieving.end(),
                        std::pair<ConjunctionInfo*, unsigned>(
                            &info, it->second),
                        conj_ptr_action_pair_less);
                    assert(j_it != counter_j.achieving.end());
                    assert(j_it->first == &info && j_it->second == it->second);
                    counter_j.achieving.erase(j_it);
                    it = info.achievers.erase(it);
                    num_removed_effect_relations_++;
                } else {
                    it++;
                    j++;
                }
            }
        }
    }
    {
        const auto& goal_facts = task_->get_goal_facts();
        goal_counter_ = counter_infos_.size();
        CounterInfo& counter = counter_infos_[goal_facts];
        if (counter.id >= goal_counter_) {
            counter.id = goal_counter_;
            counter.regression = goal_facts;
            if (goal_facts.empty()) {
                counter.regression_conjunction_ids.push_back(true_conjunction_);
                conjunction_infos_[true_conjunction_]
                    .in_regression_of.push_back(&counter);
            } else {
                for (const auto& p : goal_facts) {
                    const unsigned conj = fact_to_id[p];
                    counter.regression_conjunction_ids.push_back(conj);
                    conjunction_infos_[conj].in_regression_of.push_back(
                        &counter);
                }
            }
        } else {
            goal_counter_ = counter.id;
        }
    }
}

void
CriticalPathHeuristic::add_m_conjunctions(int m)
{
    strips_utils::FactSet fact_set;
    for (unsigned var = 0; var <= g_variable_domain.size() - m; ++var) {
        add_m_conjunctions(var, fact_set, m);
    }
}

void
CriticalPathHeuristic::add_m_conjunctions(
    int var,
    strips_utils::FactSet& fact_set,
    int m)
{
    for (int val = g_variable_domain[var] - 1; val >= 0; val--) {
        strips_utils::Fact p = task_->get_fact(var, val);
        if (!task_->are_mutex(p, fact_set)) {
            fact_set.push_back(p);
            if (m == 1) {
#if !defined(NDEBUG) && !defined(NDEBUG_EXPENSIVE)
                assert(std::is_sorted(fact_set.begin(), fact_set.end()));
                assert(
                    std::unique(fact_set.begin(), fact_set.end())
                    == fact_set.end());
#endif
                add_conjunction(fact_set);
            } else {
                for (unsigned var2 = var + 1;
                     var2 <= g_variable_domain.size() - m + 1;
                     var2++) {
                    add_m_conjunctions(var2, fact_set, m - 1);
                }
            }
            fact_set.pop_back();
        }
    }
}

void
CriticalPathHeuristic::reset_exploration_cache(
    const std::vector<unsigned>& conjunctions)
{
    for (int i = conjunctions.size() - 1; i >= 0; i--) {
        ConjunctionInfo& conj = conjunction_infos_[conjunctions[i]];
        conj.cost = ConjunctionInfo::UNACHIEVED;
        for (int j = conj.in_regression_of.size() - 1; j >= 0; j--) {
            conj.in_regression_of[j]->unsat++;
        }
    }
}

void
CriticalPathHeuristic::reset_exploration_cache()
{
    for (int i = conjunction_infos_.size() - 1; i >= 0; i--) {
        conjunction_infos_[i].cost = ConjunctionInfo::UNACHIEVED;
    }
    for (int i = counter_infos_.size() - 1; i >= 0; i--) {
        counter_infos_[i].unsat =
            counter_infos_[i].regression_conjunction_ids.size();
    }
}

int
CriticalPathHeuristic::exploration_unit_cost(
    const std::vector<unsigned>& satisfied_conjunctions)
{
    std::vector<ConjunctionInfo*> current;
    std::vector<ConjunctionInfo*> next;
    next.push_back(&conjunction_infos_[true_conjunction_]);
    next.back()->cost = 0;
    for (int i = satisfied_conjunctions.size() - 1; i >= 0; i--) {
        ConjunctionInfo* conj = &conjunction_infos_[satisfied_conjunctions[i]];
        if (conj->cost
            == ConjunctionInfo::UNACHIEVED) { // incremental computation
            next.push_back(conj);
            next.back()->cost = 0;
        }
    }
    int layer = 0;
    int result = DEAD_END;
    while (!next.empty()) {
        next.swap(current);
        next.clear();
        for (int i = current.size() - 1; i >= 0; i--) {
            ConjunctionInfo* conj = current[i];
            for (int j = conj->in_regression_of.size() - 1; j >= 0; j--) {
                CounterInfo* counter = conj->in_regression_of[j];
                if (--(counter->unsat) == 0) {
                    if (counter->id == goal_counter_) {
                        if (early_termination_) {
                            return layer;
                        } else {
                            result = layer;
                        }
                    }
                    for (int k = counter->achieving.size() - 1; k >= 0; k--) {
                        ConjunctionInfo* achieved = counter->achieving[k].first;
                        if (achieved->cost == ConjunctionInfo::UNACHIEVED) {
                            achieved->cost = layer + 1;
                            next.push_back(achieved);
                        }
                    }
                }
            }
        }
        layer++;
    }
    return result;
}

int
CriticalPathHeuristic::incremental_exploration_unit_cost(
    std::vector<unsigned>& newly_achieved_conjunctions)
{
    std::vector<ConjunctionInfo*> next;
    int layer = 0;
    auto process_counters =
        [this, &next, &layer](std::vector<CounterInfo*>& counters) {
            bool goal_reached = false;
            for (int j = counters.size() - 1; j >= 0; j--) {
                CounterInfo* counter = counters[j];
                if (--(counter->unsat) == 0) {
                    if (counter->id == goal_counter_) {
                        goal_reached = true;
                    }
                    for (int k = counter->achieving.size() - 1; k >= 0; k--) {
                        ConjunctionInfo* achieved = counter->achieving[k].first;
                        if (achieved->cost == ConjunctionInfo::UNACHIEVED) {
                            achieved->cost = layer + 1;
                            next.push_back(achieved);
                        }
                    }
                }
            }
            return goal_reached;
        };

    for (int i = newly_achieved_conjunctions.size() - 1; i >= 0; i--) {
        ConjunctionInfo* conj =
            &conjunction_infos_[newly_achieved_conjunctions[i]];
        assert(conj->cost == ConjunctionInfo::UNACHIEVED);
        conj->cost = 0;
    }

    int result = DEAD_END;
    for (int i = newly_achieved_conjunctions.size() - 1; i >= 0; i--) {
        ConjunctionInfo* conj =
            &conjunction_infos_[newly_achieved_conjunctions[i]];
        if (process_counters(conj->in_regression_of)) {
            result = 0;
        }
    }

    if (!early_termination_ || result != 0) {
        layer++;
        std::vector<ConjunctionInfo*> current;
        while (!next.empty()) {
            next.swap(current);
            next.clear();
            for (int i = current.size() - 1; i >= 0; i--) {
                ConjunctionInfo* conj = current[i];
                newly_achieved_conjunctions.push_back(current[i]->id);
                if (process_counters(conj->in_regression_of)) {
                    result = layer;
                    if (early_termination_) {
                        while (--i >= 0) {
                            current[i]->cost = ConjunctionInfo::UNACHIEVED;
                        }
                        goto incremental_end;
                    }
                }
            }
            layer++;
        }
    }

incremental_end:
    for (int i = next.size() - 1; i >= 0; i--) {
        next[i]->cost = ConjunctionInfo::UNACHIEVED;
    }

    return result;
}

int
CriticalPathHeuristic::exploration_general_cost(
    const std::vector<unsigned>& satisfied_conjunctions)
{
    priority_queues::AdaptiveQueue<ConjunctionInfo*> queue;
    queue.push(0, &conjunction_infos_[true_conjunction_]);
    conjunction_infos_[true_conjunction_].cost = 0;
    for (int i = satisfied_conjunctions.size() - 1; i >= 0; i--) {
        ConjunctionInfo* conj = &conjunction_infos_[satisfied_conjunctions[i]];
        assert(conj->cost == ConjunctionInfo::UNACHIEVED);
        conj->cost = 0;
        queue.push(0, conj);
    }
    int result = DEAD_END;
    while (!queue.empty()) {
        auto x = queue.pop();
        if (x.second->cost < x.first) {
            continue;
        }
        for (int j = x.second->in_regression_of.size() - 1; j >= 0; j--) {
            CounterInfo* counter = x.second->in_regression_of[j];
            if (--(counter->unsat) == 0) {
                if (counter->id == goal_counter_) {
                    if (early_termination_) {
                        return x.first;
                    } else {
                        result = x.first;
                    }
                }
                for (int k = counter->achieving.size() - 1; k >= 0; k--) {
                    ConjunctionInfo* achieved = counter->achieving[k].first;
                    int cost = action_costs_[counter->achieving[k].second];
                    if (achieved->cost == ConjunctionInfo::UNACHIEVED
                        || achieved->cost > x.first + cost) {
                        achieved->cost = x.first + cost;
                        queue.push(achieved->cost, achieved);
                    }
                }
            }
        }
    }
    return result;
}

int
CriticalPathHeuristic::compute_heuristic(const GlobalState& state)
{
    strips_utils::FactSet facts = task_->get_fact_set(state);
    std::vector<unsigned> conjunctions = conjunctions_.get_subsets(facts);
    reset_exploration_cache();
    return is_unit_cost_ ? exploration_unit_cost(conjunctions)
                         : exploration_general_cost(conjunctions);
}

int
CriticalPathHeuristic::compute_heuristic(const PartialState& state)
{
    strips_utils::FactSet facts = task_->get_fact_set(state);
    std::vector<unsigned> conjunctions = conjunctions_.get_subsets(facts);
    reset_exploration_cache();
    return is_unit_cost_ ? exploration_unit_cost(conjunctions)
                         : exploration_general_cost(conjunctions);
}

bool
CriticalPathHeuristic::is_conjunction_reachable(unsigned conjunction_id) const
{
    return conjunction_infos_[conjunction_id].cost
        != ConjunctionInfo::UNACHIEVED;
}

int
CriticalPathHeuristic::get_cost(unsigned conjunction_id) const
{
    return conjunction_infos_[conjunction_id].cost;
}

const strips_utils::FactSet&
CriticalPathHeuristic::get_conjunction_fact_set(unsigned conjunction_id) const
{
    return conjunctions_[conjunction_id];
}

const strips_utils::FactSetSet&
CriticalPathHeuristic::get_conjunction_fact_sets() const
{
    return conjunctions_;
}

bool
CriticalPathHeuristic::set_exploration_early_termination(bool term)
{
    bool was = early_termination_;
    early_termination_ = term;
    return was;
}

std::pair<unsigned, bool>
CriticalPathHeuristic::add_conjunction(const strips_utils::FactSet& fact_set)
{
    assert(!task_->contains_mutex(fact_set));
    // std::cout << "adding " << task_->to_string(fact_set) << std::endl;

    auto conj_id = conjunctions_.insert(fact_set);
    if (!conj_id.second) {
        // std::cout << " -> exists (" << conj_id.first << ")" << std::endl;
        return conj_id;
    }

    // create new info
    conjunction_infos_.push_back(ConjunctionInfo());
    ConjunctionInfo& conj_info = conjunction_infos_[conj_id.first];
    conj_info.id = conj_id.first;

    if (prune_dominated_preconditions_) {
        std::vector<int> relationships(num_conjunctions(), 0);
        conjunctions_.apply_to_subsets(
            fact_set, [&relationships](int idx) { relationships[idx] = -1; });
        conjunctions_.apply_to_supersets(
            fact_set, [&relationships](int idx) { relationships[idx] = 1; });
        auto update_regression = [this, &conj_info, &relationships](
                                     CounterInfo& counter) {
            bool is_dominated = false;
            unsigned i = 0;
            for (unsigned j = 0; j < counter.regression_conjunction_ids.size();
                 ++j) {
                const unsigned other = counter.regression_conjunction_ids[j];
                if (relationships[other] > 0) {
                    is_dominated = true;
                    break;
                } else if (relationships[other] == 0) {
                    if (i != j) {
                        counter.regression_conjunction_ids[i] = other;
                    }
                    i++;
                } else {
                    ConjunctionInfo& cinfo = conjunction_infos_[other];
                    cinfo.in_regression_of.erase(std::lower_bound(
                        cinfo.in_regression_of.begin(),
                        cinfo.in_regression_of.end(),
                        &counter,
                        [](const auto& ptr0, const auto& ptr1) {
                            return ptr0->id < ptr1->id;
                        }));
                    num_removed_precondition_relations_++;
                    if (cinfo.cost == ConjunctionInfo::UNACHIEVED) {
                        counter.unsat--;
                    }
                }
            }
            if (!is_dominated) {
                if (i < counter.regression_conjunction_ids.size()) {
                    counter.regression_conjunction_ids.resize(i);
                }
                conj_info.in_regression_of.push_back(&counter);
                counter.regression_conjunction_ids.push_back(conj_info.id);
                num_removed_precondition_relations_++;
            }
        };
        counter_infos_.apply_to_supersets(fact_set, update_regression);
    } else {
        auto update_regression = [this, &conj_info](CounterInfo& counter) {
            conj_info.in_regression_of.push_back(&counter);
            counter.regression_conjunction_ids.push_back(conj_info.id);
        };
        counter_infos_.apply_to_supersets(fact_set, update_regression);
    }
    std::sort(
        conj_info.in_regression_of.begin(),
        conj_info.in_regression_of.end(),
        [](const auto& ptr0, const auto& ptr1) { return ptr0->id < ptr1->id; });

#if !defined(NDEBUG) && !defined(NDEBUG_EXPENSIVE)
    for (unsigned i = 0; i < counter_infos_.size(); i++) {
        const CounterInfo& counter = counter_infos_[i];
        bool in_regr = std::count(
                           counter.regression_conjunction_ids.begin(),
                           counter.regression_conjunction_ids.end(),
                           conj_info.id)
            != 0;
        assert(
            in_regr
            == (std::count(
                    conj_info.in_regression_of.begin(),
                    conj_info.in_regression_of.end(),
                    &counter)
                != 0));
        if (std::includes(
                counter.regression.begin(),
                counter.regression.end(),
                fact_set.begin(),
                fact_set.end())) {
            bool is_dominated = false;
            if (!in_regr) {
                for (unsigned j = 0;
                     j < counter.regression_conjunction_ids.size();
                     j++) {
                    const auto& other =
                        conjunctions_[counter.regression_conjunction_ids[j]];
                    if (std::includes(
                            other.begin(),
                            other.end(),
                            fact_set.begin(),
                            fact_set.end())) {
                        is_dominated = true;
                        break;
                    }
                }
            }
            assert(in_regr || (prune_dominated_preconditions_ && is_dominated));
        } else {
            assert(!in_regr);
        }
    }
#endif

    std::vector<int> is_regressable(task_->num_actions(), 0);
    task_->compute_is_regressable(fact_set, is_regressable);
    unsigned counter_id = counter_infos_.size();
    if (prune_dominated_achievers_) {
        struct Achiever {
            Achiever()
                : action(-1)
                , cost(0)
            {
            }
            int action;
            int cost;
        };
        using AchieverMap = std::unordered_map<
            std::vector<unsigned>,
            Achiever,
            ::utils::Hash<std::vector<unsigned>>>;
        AchieverMap achievers;
        for (int i = is_regressable.size() - 1; i >= 0; i--) {
            if (is_regressable[i] != 1) {
                continue;
            }
            strips_utils::FactSet regression =
                task_->get_actions()[i].regress(fact_set);
            Achiever& achiever = achievers[std::move(regression)];
            num_removed_effect_relations_ += achiever.action != -1;
            const int cost = action_costs_[i];
            if (achiever.action == -1 || cost <= achiever.cost) {
                achiever.action = i;
                achiever.cost = cost;
            }
        }
        if (achievers.size() > 0) {
            auto it = achievers.begin();
            std::vector<AchieverMap::iterator> non_dominated({ it++ });
            for (; it != achievers.end(); ++it) {
                const std::vector<unsigned>& a_regression = it->first;
                const Achiever& a = it->second;
                bool is_dominated = false;
                for (int j = non_dominated.size() - 1; j >= 0; j--) {
                    const std::vector<unsigned>& b_regression =
                        non_dominated[j]->first;
                    const Achiever& b = non_dominated[j]->second;
                    if (a_regression.size() < b_regression.size()) {
                        if (a.cost <= b.cost
                            && std::includes(
                                b_regression.begin(),
                                b_regression.end(),
                                a_regression.begin(),
                                a_regression.end())) {
                            non_dominated.erase(non_dominated.begin() + j);
                            num_removed_effect_relations_++;
                        }
                    } else if (a_regression.size() > b_regression.size()) {
                        if (a.cost >= b.cost
                            && std::includes(
                                a_regression.begin(),
                                a_regression.end(),
                                b_regression.begin(),
                                b_regression.end())) {
                            is_dominated = true;
                            num_removed_effect_relations_++;
                            break;
                        }
                    }
                }
                if (!is_dominated) {
                    non_dominated.push_back(it);
                }
            }
            for (int i = non_dominated.size() - 1; i >= 0; --i) {
                // Achiever* achiever = non_dominated[i];
                auto it = non_dominated[i];
                CounterInfo& counter = counter_infos_[it->first];
                if (counter.id >= counter_id) {
                    counter.id = counter_id++;
                    counter.regression = std::move(it->first);
                    if (prune_dominated_preconditions_) {
                        counter.regression_conjunction_ids =
                            conjunctions_.get_non_dominated_subsets(
                                counter.regression);
                    } else {
                        counter.regression_conjunction_ids =
                            conjunctions_.get_subsets(counter.regression);
                    }
                    if (counter.regression_conjunction_ids.empty()) {
                        counter.regression_conjunction_ids.push_back(
                            true_conjunction_);
                    }
                    for (int j = counter.regression_conjunction_ids.size() - 1;
                         j >= 0;
                         j--) {
                        ConjunctionInfo& pre_conj = conjunction_infos_
                            [counter.regression_conjunction_ids[j]];
                        pre_conj.in_regression_of.push_back(&counter);
                    }
                }
                counter.achieving.emplace_back(&conj_info, it->second.action);
                conj_info.achievers.emplace_back(&counter, it->second.action);
            }
            std::sort(
                conj_info.achievers.begin(),
                conj_info.achievers.end(),
                [](const auto& x, const auto& y) {
                    return x.second < y.second;
                });
            num_removed_counters_ += (achievers.size() - non_dominated.size());
        }
    } else {
        std::vector<CounterInfo*> achievers;
        for (int i = is_regressable.size() - 1; i >= 0; i--) {
            if (is_regressable[i] != 1) {
                continue;
            }
            strips_utils::FactSet regression =
                task_->get_actions()[i].regress(fact_set);
            CounterInfo& counter = counter_infos_[regression];
            if (counter.id >= counter_id) {
                counter.id = counter_id++;
                counter.regression = std::move(regression);
                if (prune_dominated_preconditions_) {
                    counter.regression_conjunction_ids =
                        conjunctions_.get_non_dominated_subsets(
                            counter.regression);
                } else {
                    counter.regression_conjunction_ids =
                        conjunctions_.get_subsets(counter.regression);
                }
                if (counter.regression_conjunction_ids.empty()) {
                    counter.regression_conjunction_ids.push_back(
                        true_conjunction_);
                }
                for (int j = counter.regression_conjunction_ids.size() - 1;
                     j >= 0;
                     j--) {
                    ConjunctionInfo& pre_conj =
                        conjunction_infos_[counter
                                               .regression_conjunction_ids[j]];
                    pre_conj.in_regression_of.push_back(&counter);
                }
            }
#if !defined(NDEBUG) && !defined(NDEBUG_EXPENSIVE)
            else {
                assert(counter.regression == regression);
            }
#endif
            if (counter.achieving.empty()
                || counter.achieving.back().first != &conj_info) {
                counter.achieving.emplace_back(&conj_info, i);
                achievers.push_back(&counter);
            } else {
                num_removed_effect_relations_++;
                std::pair<ConjunctionInfo*, unsigned>& x =
                    counter.achieving.back();
                if (action_costs_[i] <= action_costs_[x.second]) {
                    x.second = i;
                }
            }
        }

        std::sort(
            achievers.begin(),
            achievers.end(),
            [](const auto& x, const auto& y) {
                return x->achieving.back().second < y->achieving.back().second;
            });
        for (unsigned i = 0; i < achievers.size(); i++) {
            conj_info.achievers.emplace_back(
                achievers[i], achievers[i]->achieving.back().second);
        }
    }

#if !defined(NDEBUG) && !defined(NDEBUG_EXPENSIVE)
    for (unsigned i = 0; i < conjunctions_.size(); ++i) {
        const ConjunctionInfo& conj_info = conjunction_infos_[i];
        assert(conj_info.id == i);
        assert(std::is_sorted(
            conj_info.in_regression_of.begin(),
            conj_info.in_regression_of.end(),
            [](const auto& x, const auto& y) { return x->id < y->id; }));
        for (unsigned j = 0; j < conj_info.in_regression_of.size(); j++) {
            const CounterInfo& counter_info = *conj_info.in_regression_of[j];
            assert(std::count(
                counter_info.regression_conjunction_ids.begin(),
                counter_info.regression_conjunction_ids.end(),
                conj_info.id));
            assert(std::includes(
                counter_info.regression.begin(),
                counter_info.regression.end(),
                conjunctions_[i].begin(),
                conjunctions_[i].end()));
        }
    }
    for (unsigned i = 0; i < counter_infos_.size(); i++) {
        const CounterInfo& counter_info = counter_infos_[i];
        assert(counter_info.id == i);
        assert(std::is_sorted(
            counter_info.regression.begin(), counter_info.regression.end()));
        assert(std::is_sorted(
            counter_info.regression_conjunction_ids.begin(),
            counter_info.regression_conjunction_ids.end()));
        assert(std::is_sorted(
            counter_info.achieving.begin(),
            counter_info.achieving.end(),
            [](const auto& x, const auto& y) {
                return x.first->id < y.first->id;
            }));
        for (unsigned j = 0; j < counter_info.regression_conjunction_ids.size();
             j++) {
            const ConjunctionInfo& conj_info =
                conjunction_infos_[counter_info.regression_conjunction_ids[j]];
            assert(std::count(
                conj_info.in_regression_of.begin(),
                conj_info.in_regression_of.end(),
                &counter_info));
        }
    }
#endif

    return conj_id;
}

std::vector<unsigned>
CriticalPathHeuristic::get_satisfied_conjunctions(
    const GlobalState& state) const
{
    strips_utils::FactSet facts = task_->get_fact_set(state);
    return conjunctions_.get_subsets(facts);
}

std::vector<unsigned>
CriticalPathHeuristic::get_satisfied_conjunctions(
    const PartialState& state) const
{
    strips_utils::FactSet facts = task_->get_fact_set(state);
    return conjunctions_.get_subsets(facts);
}

std::vector<unsigned>
CriticalPathHeuristic::get_satisfied_conjunctions(
    const strips_utils::FactSet& facts) const
{
    return conjunctions_.get_subsets(facts);
}

const std::vector<unsigned>&
CriticalPathHeuristic::get_goal_conjunctions() const
{
    return counter_infos_[goal_counter_].regression_conjunction_ids;
}

CounterInfo*
CriticalPathHeuristic::get_goal_counter()
{
    return &counter_infos_[goal_counter_];
}

static Plugin<Heuristic>
    _plugin("hc", options::parse<Heuristic, CriticalPathHeuristic>);

} // namespace critical_path_heuristic
