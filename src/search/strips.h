#pragma once

#include "fact_set.h"

#include <iterator>
#include <memory>
#include <string>
#include <vector>

class GlobalState;
class PartialState;

namespace strips_utils {

class STRIPS {
public:
    struct Action {
        unsigned id;
        unsigned original_operator;
        int cost;
        FactSet pre;
        FactSet add;
        FactSet del;

        explicit Action(unsigned id, unsigned op);

        FactSet regress(const FactSet& fact_set) const;
        FactSet progress(const FactSet& fact_set) const;
    };

    class fact_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Fact;
        using difference_type = int;
        using pointer = Fact*;
        using reference = Fact;

        ~fact_iterator() = default;
        fact_iterator(const fact_iterator&) = default;
        fact_iterator(fact_iterator&&) = default;
        fact_iterator& operator=(const fact_iterator&) = default;
        fact_iterator& operator=(fact_iterator&&) = default;

        fact_iterator& operator--()
        {
            f_--;
            return *this;
        }

        fact_iterator operator--(int d)
        {
            fact_iterator result(*this);
            f_ -= d;
            return result;
        }

        fact_iterator& operator++()
        {
            f_++;
            return *this;
        }

        fact_iterator operator++(int d)
        {
            fact_iterator result(*this);
            f_ += d;
            return result;
        }

        Fact operator*() const { return f_; }

        bool operator==(const fact_iterator& other) const
        {
            return f_ == other.f_;
        }

        bool operator!=(const fact_iterator& other) const
        {
            return f_ != other.f_;
        }

    private:
        friend class STRIPS;
        fact_iterator(Fact f)
            : f_(f)
        {
        }
        Fact f_;
    };

    STRIPS(const STRIPS&) = default;
    STRIPS(STRIPS&&) = default;
    ~STRIPS() = default;
    STRIPS& operator=(const STRIPS&) = default;
    STRIPS& operator=(STRIPS&&) = default;

    static std::shared_ptr<STRIPS> get_task();

    unsigned num_facts() const;
    const FactSet& get_goal_facts() const;
    Fact get_fact(const std::pair<int, int>& x) const;
    Fact get_fact(int var, int val) const;
    FactSet get_fact_set(const GlobalState& state) const;
    FactSet get_fact_set(const PartialState& state) const;
    std::pair<int, int> get_variable_assignment(Fact p) const;
    void
    copy_to_partial_state(const FactSet& fact_set, PartialState& result) const;

    bool contains_mutex(const FactSet& facts) const;
    bool get_mutex(const FactSet& subgoal, FactSet& conflict) const;
    bool are_mutex(Fact p, Fact q) const;
    bool are_mutex(const FactSet& x, const FactSet& y) const;
    bool are_mutex(const Fact& x, const FactSet& y) const;

    unsigned num_actions() const;
    const std::vector<Action>& get_actions() const;
    const Action& get_operator(unsigned idx) const;
    const std::vector<unsigned>& get_actions_that_delete(Fact p) const;
    const std::vector<unsigned>& get_actions_that_add(Fact p) const;
    const std::vector<unsigned>& get_mutex_actions(Fact p) const;
    const std::vector<unsigned>&
    get_actions_with_mutex_progression(Fact p) const;

    void compute_is_regressable(
        const FactSet& conj,
        std::vector<int>& achievers) const;

    void compute_is_progressable(
        const FactSet& conj,
        std::vector<int>& achievers) const;

    fact_iterator fact_begin() const { return fact_iterator(0); }
    fact_iterator fact_end() const { return fact_iterator(facts_); }

    std::string to_string(const Fact& p) const;
    std::string to_string(const FactSet& fact_set) const;

    Fact parse_fact_string(const std::string& fact) const;
    FactSet parse_fact_set_string(const std::string& fact_set) const;

private:
    explicit STRIPS();

    static std::shared_ptr<STRIPS> task_instance_;

    unsigned facts_;
    std::vector<unsigned> variable_offset_;
    FactSet goal_facts_;
    std::vector<std::vector<bool>> mutexes_;

    std::vector<Action> actions_;
    std::vector<std::vector<unsigned>> actions_that_add_;
    std::vector<std::vector<unsigned>> actions_that_del_;
    std::vector<std::vector<unsigned>> actions_with_mutex_regression_;
    std::vector<std::vector<unsigned>> actions_with_mutex_progression_;
};

} // namespace strips_utils
