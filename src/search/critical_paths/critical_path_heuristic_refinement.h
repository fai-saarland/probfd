#pragma once

#include "../state_component_listener.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace strips_utils {
class STRIPS;
}

namespace critical_path_heuristic {

class CriticalPathHeuristic;
class CriticalPathHeuristicNoGoods;
class CriticalPathOnlineCARTHeuristic;

class CriticalPathHeuristicRefinement
    : public state_component::StateComponentListener {
public:
    CriticalPathHeuristicRefinement(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual bool refine_conjunction_set(
        state_component::StateComponent&,
        state_component::StateComponent&) = 0;
    virtual Status on_new_component(
        state_component::StateComponent&,
        state_component::StateComponent&) override;
    virtual void statistics() const override;
    virtual std::string name() const = 0;

    bool is_size_limit_reached() const;

    std::shared_ptr<CriticalPathHeuristicNoGoods> nogoods_;
    std::shared_ptr<CriticalPathOnlineCARTHeuristic> carts_;

    std::shared_ptr<CriticalPathHeuristic> hc_;
    std::shared_ptr<strips_utils::STRIPS> task_;

    const unsigned overall_counter_limit_;
    const unsigned overall_conjunction_limit_;
    const bool stop_listening_on_failure_;

    unsigned num_refinements_;
    unsigned num_refinements_wo_changes_;
};

} // namespace critical_path_heuristic
