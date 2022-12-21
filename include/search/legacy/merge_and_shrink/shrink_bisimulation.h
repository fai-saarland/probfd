#ifndef LEGACY_MERGE_AND_SHRINK_SHRINK_BISIMULATION_H
#define LEGACY_MERGE_AND_SHRINK_SHRINK_BISIMULATION_H

#include "legacy/merge_and_shrink/shrink_strategy.h"

#include <memory>

namespace legacy {
namespace merge_and_shrink {

struct Signature;

class ShrinkBisimulation : public ShrinkStrategy {
    enum Greediness { NOT_GREEDY, SOMEWHAT_GREEDY, GREEDY };

    enum AtLimit { RETURN, USE_UP };

    /*
      greediness: Select between exact, "somewhat greedy" or "greedy"
      bisimulation.

      threshold: Shrink the abstraction iff it is larger than this
      size. Note that this is set independently from max_states, which
      is the number of states to which the abstraction is shrunk.
    */

    const Greediness greediness;
    const int threshold;
    const bool initialize_by_h;
    const bool group_by_h;
    const AtLimit at_limit;

    std::vector<bool> labels;

    void compute_abstraction(
        Abstraction& abs,
        int target_size,
        EquivalenceRelation& equivalence_relation);

    int
    initialize_groups(const Abstraction& abs, std::vector<int>& state_to_group);
    void compute_signatures(
        const Abstraction& abs,
        std::vector<Signature>& signatures,
        std::vector<int>& state_to_group);

public:
    ShrinkBisimulation(const options::Options& opts);
    virtual ~ShrinkBisimulation();

    virtual std::string name() const;
    virtual void dump_strategy_specific_options() const;

    virtual bool reduce_labels_before_shrinking() const;

    virtual void shrink(Abstraction& abs, int target, bool force = false);
    virtual void shrink_atomic(Abstraction& abs);
    virtual void shrink_before_merge(Abstraction& abs1, Abstraction& abs2);

    static std::shared_ptr<ShrinkStrategy> create_default();

    virtual bool is_backward_pruned() const { return false; }
    virtual bool require_distances() const { return true; }
    void set_all_labels();
    void set_labels_from_file();
    void set_labels_from_abstraction(
        Abstraction& abs,
        bool backward_pruning = false,
        int find_goal_leading_actions = 0);
    void set_labels(std::vector<bool>& labels_);

    static void add_options_to_parser(options::OptionParser& parser);
    static void handle_option_defaults(options::Options& opts);

    virtual const std::vector<bool>& get_caught_labels_set() const
    {
        return labels;
    }
};

} // namespace merge_and_shrink
} // namespace legacy

#endif
