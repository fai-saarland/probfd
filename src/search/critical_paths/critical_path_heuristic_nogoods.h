#pragma once

#include "../fact_set.h"
#include "../heuristic.h"
#include "../utils/timer.h"

#include <memory>

namespace strips_utils {
class STRIPS;
}

namespace critical_path_heuristic {

class CriticalPathHeuristic;

class CriticalPathHeuristicNoGoods : public Heuristic {
public:
    explicit CriticalPathHeuristicNoGoods(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

    virtual bool supports_partial_state_evaluation() const override;

    std::shared_ptr<CriticalPathHeuristic> get_underlying_heuristic() const;
    void update_nogoods(const GlobalState& state);
    void print_statistics() const;

protected:
    virtual int compute_heuristic(const GlobalState& state) override;
    virtual int compute_heuristic(const PartialState& partial_state) override;

    void update_nogoods(const GlobalState& state, strips_utils::FactSet& state_facts);

    std::shared_ptr<CriticalPathHeuristic> hc_;
    std::shared_ptr<strips_utils::STRIPS> task_;

    strips_utils::FactSetSet nogoods_;
    ::utils::Timer refinement_time_;
    ::utils::Timer evaluation_time_;
    unsigned evaluations_;
    unsigned saved_heuristic_computations_;
    unsigned refinements_;
};

} // namespace critical_path_heuristic
