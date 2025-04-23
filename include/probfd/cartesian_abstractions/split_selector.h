#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_SPLIT_SELECTOR_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_SPLIT_SELECTOR_H

#include <cassert>
#include <limits>
#include <memory>
#include <vector>

// Forward Declarations
namespace downward {
class PlanningTask;
}

namespace downward::additive_heuristic {
class AdditiveHeuristic;
}

namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd {
class ProbabilisticTask;
}

namespace probfd::cartesian_abstractions {
class AbstractState;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

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
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;

public:
    explicit SplitSelectorRandom(
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng);

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

        if (splits.size() == 1) { return splits[0]; }

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
class SplitSelectorUnwanted
    : public RateBasedSplitSelector<SplitSelectorUnwanted> {
    const int factor_;

public:
    explicit SplitSelectorUnwanted(int factor);

    [[nodiscard]]
    double rate_split(const AbstractState& state, const Split& split) const;
};

// Refinement: - (remaining_values / original_domain_size)
class SplitSelectorRefinedness
    : public RateBasedSplitSelector<SplitSelectorRefinedness> {
    const std::shared_ptr<ProbabilisticTask> task_;
    const double factor_;

public:
    SplitSelectorRefinedness(
        std::shared_ptr<ProbabilisticTask> task,
        double factor);

    [[nodiscard]]
    double rate_split(const AbstractState& state, const Split& split) const;
};

// Compare the h^add(s_0) values of the facts.
class SplitSelectorHAdd {
    const std::shared_ptr<downward::PlanningTask> task_;
    std::unique_ptr<downward::additive_heuristic::AdditiveHeuristic>
        additive_heuristic_;

public:
    explicit SplitSelectorHAdd(const std::shared_ptr<ProbabilisticTask>& task);
    ~SplitSelectorHAdd();

protected:
    [[nodiscard]]
    int get_hadd_value(int var_id, int value) const;
};

class SplitSelectorMinHAdd
    : public RateBasedSplitSelector<SplitSelectorMinHAdd>
    , public SplitSelectorHAdd {
public:
    explicit SplitSelectorMinHAdd(
        const std::shared_ptr<ProbabilisticTask>& task);

    [[nodiscard]]
    double rate_split(const AbstractState& state, const Split& split) const;

private:
    [[nodiscard]]
    int get_min_hadd_value(int var_id, const std::vector<int>& values) const;
};

class SplitSelectorMaxHAdd
    : public RateBasedSplitSelector<SplitSelectorMaxHAdd>
    , public SplitSelectorHAdd {
public:
    explicit SplitSelectorMaxHAdd(
        const std::shared_ptr<ProbabilisticTask>& task);

    [[nodiscard]]
    double rate_split(const AbstractState& state, const Split& split) const;

private:
    [[nodiscard]]
    int get_max_hadd_value(int var_id, const std::vector<int>& values) const;
};

/*
  Select split in case there are multiple possible splits.
*/
class SplitSelectorFactory {
public:
    virtual ~SplitSelectorFactory() = default;

    virtual std::unique_ptr<SplitSelector>
    create_split_selector(const std::shared_ptr<ProbabilisticTask>& task) = 0;
};

/*
  Select split in case there are multiple possible splits.
*/
class SplitSelectorRandomFactory : public SplitSelectorFactory {
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;

public:
    explicit SplitSelectorRandomFactory(
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng);

    std::unique_ptr<SplitSelector> create_split_selector(
        const std::shared_ptr<ProbabilisticTask>& task) override;
};

/*
  Select split in case there are multiple possible splits.
*/
class SplitSelectorMinUnwantedFactory : public SplitSelectorFactory {
public:
    std::unique_ptr<SplitSelector> create_split_selector(
        const std::shared_ptr<ProbabilisticTask>& task) override;
};

/*
  Select split in case there are multiple possible splits.
*/
class SplitSelectorMaxUnwantedFactory : public SplitSelectorFactory {
public:
    std::unique_ptr<SplitSelector> create_split_selector(
        const std::shared_ptr<ProbabilisticTask>& task) override;
};

/*
  Select split in case there are multiple possible splits.
*/
class SplitSelectorMinRefinedFactory : public SplitSelectorFactory {
public:
    std::unique_ptr<SplitSelector> create_split_selector(
        const std::shared_ptr<ProbabilisticTask>& task) override;
};

/*
  Select split in case there are multiple possible splits.
*/
class SplitSelectorMaxRefinedFactory : public SplitSelectorFactory {
public:
    std::unique_ptr<SplitSelector> create_split_selector(
        const std::shared_ptr<ProbabilisticTask>& task) override;
};

/*
  Select split in case there are multiple possible splits.
*/
class SplitSelectorMinHAddFactory : public SplitSelectorFactory {
public:
    std::unique_ptr<SplitSelector> create_split_selector(
        const std::shared_ptr<ProbabilisticTask>& task) override;
};

/*
  Select split in case there are multiple possible splits.
*/
class SplitSelectorMaxHAddFactory : public SplitSelectorFactory {
public:
    std::unique_ptr<SplitSelector> create_split_selector(
        const std::shared_ptr<ProbabilisticTask>& task) override;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_SPLIT_SELECTOR_H
