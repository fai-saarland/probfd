#ifndef PROBFD_HEURISTICS_CARTESIAN_SPLIT_SELECTOR_H
#define PROBFD_HEURISTICS_CARTESIAN_SPLIT_SELECTOR_H

#include "probfd/task_proxy.h"

#include "probfd/heuristics/cartesian/types.h"

#include <limits>
#include <memory>
#include <vector>

namespace additive_heuristic {
class AdditiveHeuristic;
}

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace heuristics {
namespace cartesian {

// Strategies for selecting a split in case there are multiple possibilities.
enum class PickSplit {
    RANDOM,
    // Number of values that land in the state whose h-value is probably raised.
    MIN_UNWANTED,
    MAX_UNWANTED,
    // Refinement: - (remaining_values / original_domain_size)
    MIN_REFINED,
    MAX_REFINED,
    // Compare the h^add(s_0) values of the facts.
    MIN_HADD,
    MAX_HADD
};

struct Split {
    int var_id;
    std::vector<int> values;
};

/*
  Select split in case there are multiple possible splits.
*/
class SplitSelector {
public:
    virtual ~SplitSelector() = default;

    virtual const Split& pick_split(
        const AbstractState& state,
        const std::vector<Split>& splits) = 0;
};

class SplitSelectorRandom : public SplitSelector {
    utils::RandomNumberGenerator& rng;

public:
    explicit SplitSelectorRandom(utils::RandomNumberGenerator& rng);

    const Split&
    pick_split(const AbstractState& state, const std::vector<Split>& splits)
        override;
};

template <class Derived>
class RateBasedSplitSelector : public SplitSelector {
public:
    const Split&
    pick_split(const AbstractState& state, const std::vector<Split>& splits)
        override
    {

        assert(!splits.empty());

        if (splits.size() == 1) {
            return splits[0];
        }

        double max_rating = std::numeric_limits<double>::lowest();
        const Split* selected_split = nullptr;
        for (const Split& split : splits) {
            double rating =
                static_cast<const Derived*>(this)->rate_split(state, split);
            if (rating > max_rating) {
                selected_split = &split;
                max_rating = rating;
            }
        }

        assert(selected_split);
        return *selected_split;
    }
};

// Number of values that land in the state whose h-value is probably raised.
class SplitSelectorMinUnwanted
    : public RateBasedSplitSelector<SplitSelectorMinUnwanted> {
public:
    double rate_split(const AbstractState& state, const Split& split) const;
};

// Number of values that land in the state whose h-value is probably raised.
class SplitSelectorMaxUnwanted
    : public RateBasedSplitSelector<SplitSelectorMaxUnwanted> {
public:
    double rate_split(const AbstractState& state, const Split& split) const;
};

// Refinement: - (remaining_values / original_domain_size)
class SplitSelectorRefinedness {
    const ProbabilisticTaskProxy task_proxy;

public:
    explicit SplitSelectorRefinedness(
        const std::shared_ptr<ProbabilisticTask>& task);

protected:
    double get_refinedness(const AbstractState& state, int var_id) const;
};

class SplitSelectorMinRefined
    : public RateBasedSplitSelector<SplitSelectorMinRefined>
    , public SplitSelectorRefinedness {
public:
    explicit SplitSelectorMinRefined(
        const std::shared_ptr<ProbabilisticTask>& task);

    double rate_split(const AbstractState& state, const Split& split) const;
};

class SplitSelectorMaxRefined
    : public RateBasedSplitSelector<SplitSelectorMaxRefined>
    , public SplitSelectorRefinedness {
public:
    explicit SplitSelectorMaxRefined(
        const std::shared_ptr<ProbabilisticTask>& task);

    double rate_split(const AbstractState& state, const Split& split) const;
};

// Compare the h^add(s_0) values of the facts.
class SplitSelectorHAdd {
    const std::shared_ptr<AbstractTaskBase> task;
    const ProbabilisticTaskProxy task_proxy;
    std::unique_ptr<additive_heuristic::AdditiveHeuristic> additive_heuristic;

public:
    explicit SplitSelectorHAdd(const std::shared_ptr<ProbabilisticTask>& task);

protected:
    int get_hadd_value(int var_id, int value) const;
};

class SplitSelectorMinHAdd
    : public RateBasedSplitSelector<SplitSelectorMinHAdd>
    , public SplitSelectorHAdd {
public:
    explicit SplitSelectorMinHAdd(
        const std::shared_ptr<ProbabilisticTask>& task);

    double rate_split(const AbstractState& state, const Split& split) const;

private:
    int get_min_hadd_value(int var_id, const std::vector<int>& values) const;
};

class SplitSelectorMaxHAdd
    : public RateBasedSplitSelector<SplitSelectorMaxHAdd>
    , public SplitSelectorHAdd {
public:
    explicit SplitSelectorMaxHAdd(
        const std::shared_ptr<ProbabilisticTask>& task);

    double rate_split(const AbstractState& state, const Split& split) const;

private:
    int get_max_hadd_value(int var_id, const std::vector<int>& values) const;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
