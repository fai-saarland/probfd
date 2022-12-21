#ifndef LEGACY_MERGE_AND_SHRINK_SHRINK_STRATEGY_H
#define LEGACY_MERGE_AND_SHRINK_SHRINK_STRATEGY_H

#include <list>
#include <string>
#include <vector>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace legacy {
namespace merge_and_shrink {

class Abstraction;

typedef int AbstractStateRef;

class ShrinkStrategy {

    const int max_states;
    const int max_states_before_merge;

    const std::vector<bool> labels_set;

protected:
    // Atomic abstractions is used by the shrinking strategies to know
    // which variables are relevant(atomic_abstractions[v]->size() > 1)
    // and to build the operator signature in bisimulation
    std::vector<Abstraction*> atomic_abstractions;

    /* An equivalence class is a set of abstract states that shall be
       mapped (shrunk) to the same abstract state.

       An equivalence relation is a partitioning of states into
       equivalence classes. It may omit certain states entirely; these
       will be dropped completely and receive an h value of infinity.
       This is used to remove unreachable and irrelevant states.
    */

    typedef std::list<AbstractStateRef> EquivalenceClass;
    typedef std::vector<EquivalenceClass> EquivalenceRelation;

public:
    // HACK/TODO: The following method would usually be protected, but
    // the option parser requires it to be public for the
    // DefaultValueNamer. We need to reconsider the use of the
    // DefaultValueNamer here anyway, along with the way the default
    // strategy is specified. Maybe add a capability to have the default
    // value be a factory function with a string description?
    virtual std::string name() const = 0;

protected:
    virtual void dump_strategy_specific_options() const;

    std::pair<int, int> compute_shrink_sizes(int size1, int size2) const;
    bool must_shrink(const Abstraction& abs, int threshold, bool force) const;
    void apply(
        Abstraction& abs,
        EquivalenceRelation& equivalence_relation,
        int threshold) const;

public:
    ShrinkStrategy(const options::Options& opts);
    virtual ~ShrinkStrategy();

    /* Set this to true to apply label reduction before shrinking, in
       addition to the times when it is usually applied. Some shrink
       strategies may require labels to be reduced (e.g. ones based on
       bisimulations), and others might prefer reduced labels becaues
       it makes their computation more efficient.

       Set this to false if the shrink strategy does not benefit (or
       not much) from label reduction. Label reduction costs quite a
       bit of time and should not be performed without need.
    */
    virtual bool reduce_labels_before_shrinking() const = 0;

    void dump_options() const;

    /* TODO: Make sure that *all* shrink strategies prune irrelevant
       and unreachable states, then update documentation below
       accordingly? Currently the only exception is ShrinkRandom, I
       think. */

    /* Shrink the abstraction to size threshold.

       In most shrink stategies, this also prunes all irrelevant and
       unreachable states, which may cause the resulting size to be
       lower than threshold.

       Does nothing if threshold >= abs.size() unless force is true
       (in which case it only prunes irrelevant and unreachable
       states).
    */

    // TODO: Should all three of these be virtual?
    // TODO: Should all three of these be public?
    //       If not, also modify in derived clases.

    virtual void
    shrink(Abstraction& abs, int threshold, bool force = false) = 0;
    virtual void shrink_atomic(Abstraction& abs1);
    virtual void shrink_before_merge(Abstraction& abs1, Abstraction& abs2);

    static void add_options_to_parser(options::OptionParser& parser);
    static void handle_option_defaults(options::Options& opts);

    void set_atomic_abstractions(
        const std::vector<Abstraction*> atomic_abstractions_)
    {
        atomic_abstractions = atomic_abstractions_;
    }

    virtual bool is_goal_leading() const { return false; }
    virtual bool is_backward_pruned() const { return false; }
    virtual bool require_distances() const { return false; }

    virtual const std::vector<bool>& get_caught_labels_set() const
    {
        return labels_set;
    }
};

} // namespace merge_and_shrink
} // namespace legacy

#endif
