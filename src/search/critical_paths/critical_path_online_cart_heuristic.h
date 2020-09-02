#pragma once

#include "../algorithms/ub_tree.h"
#include "../heuristic.h"
#include "../utils/timer.h"

#include <memory>

namespace critical_path_heuristic {

class CriticalPathHeuristic;
class ConjunctionInfo;
class CounterInfo;

class CriticalPathOnlineCARTHeuristic : public Heuristic {
public:
    CriticalPathOnlineCARTHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);
    virtual bool supports_partial_state_evaluation() const override;
    void extract_and_insert_new_trace();
    void print_statistics() const;
    std::shared_ptr<CriticalPathHeuristic> get_underlying_heuristic() const;

protected:
    virtual int compute_heuristic(const GlobalState& state);
    virtual int compute_heuristic(const PartialState& partial_state) override;
    ConjunctionInfo* select_unsatisfied_conjunction(const CounterInfo& counter);
    bool evaluate_unsorted(std::vector<unsigned>& conjunctions) const;

    std::shared_ptr<CriticalPathHeuristic> hc_;
    ub_tree::UBTree<unsigned, bool> traces_;

    unsigned evaluations_;
    unsigned true_evaluations_;
    unsigned refinements_;
    ::utils::Timer evaluation_time_;
    ::utils::Timer refinement_time_;
};

} // namespace critical_path_heuristic
