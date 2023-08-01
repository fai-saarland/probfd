#include "probfd/heuristics/cartesian/split_selector.h"

#include "probfd/heuristics/cartesian/abstract_state.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "downward/heuristics/additive_heuristic.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"

#include <cassert>
#include <iostream>
#include <limits>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

SplitSelectorRandom::SplitSelectorRandom(utils::RandomNumberGenerator& rng)
    : rng(rng)
{
}

SplitSelectorRefinedness::SplitSelectorRefinedness(
    const std::shared_ptr<ProbabilisticTask>& task)
    : task_proxy(*task)
{
}

SplitSelectorMinRefined::SplitSelectorMinRefined(
    const std::shared_ptr<ProbabilisticTask>& task)
    : SplitSelectorRefinedness(task)
{
}

SplitSelectorMaxRefined::SplitSelectorMaxRefined(
    const std::shared_ptr<ProbabilisticTask>& task)
    : SplitSelectorRefinedness(task)
{
}

SplitSelectorHAdd::SplitSelectorHAdd(
    const std::shared_ptr<ProbabilisticTask>& task)
    : task(task)
    , task_proxy(*task)
    , additive_heuristic(create_additive_heuristic(task))
{
    additive_heuristic->compute_heuristic_for_cegar(
        task_proxy.get_initial_state());
}

SplitSelectorMinHAdd::SplitSelectorMinHAdd(
    const std::shared_ptr<ProbabilisticTask>& task)
    : SplitSelectorHAdd(task)
{
}

SplitSelectorMaxHAdd::SplitSelectorMaxHAdd(
    const std::shared_ptr<ProbabilisticTask>& task)
    : SplitSelectorHAdd(task)
{
}

SplitSelectorMinHAdd::~SplitSelectorMinHAdd() = default;

SplitSelectorMaxHAdd::~SplitSelectorMaxHAdd() = default;

static int
get_num_unwanted_values(const AbstractState& state, const Split& split)
{
    int num_unwanted_values = state.count(split.var_id) - split.values.size();
    assert(num_unwanted_values >= 1);
    return num_unwanted_values;
}

double SplitSelectorRefinedness::get_refinedness(
    const AbstractState& state,
    int var_id) const
{
    double all_values = task_proxy.get_variables()[var_id].get_domain_size();
    assert(all_values >= 2);
    double remaining_values = state.count(var_id);
    assert(2 <= remaining_values && remaining_values <= all_values);
    double refinedness = -(remaining_values / all_values);
    assert(-1.0 <= refinedness && refinedness < 0.0);
    return refinedness;
}

int SplitSelectorHAdd::get_hadd_value(int var_id, int value) const
{
    assert(additive_heuristic);
    int hadd = additive_heuristic->get_cost_for_cegar(var_id, value);
    assert(hadd != -1);
    return hadd;
}

int SplitSelectorMinHAdd::get_min_hadd_value(
    int var_id,
    const vector<int>& values) const
{
    int min_hadd = numeric_limits<int>::max();
    for (int value : values) {
        const int hadd = get_hadd_value(var_id, value);
        if (hadd < min_hadd) {
            min_hadd = hadd;
        }
    }
    return min_hadd;
}

int SplitSelectorMaxHAdd::get_max_hadd_value(
    int var_id,
    const vector<int>& values) const
{
    int max_hadd = -1;
    for (int value : values) {
        const int hadd = get_hadd_value(var_id, value);
        if (hadd > max_hadd) {
            max_hadd = hadd;
        }
    }
    return max_hadd;
}

double SplitSelectorMinUnwanted::rate_split(
    const AbstractState& state,
    const Split& split) const
{
    return -get_num_unwanted_values(state, split);
}

double SplitSelectorMaxUnwanted::rate_split(
    const AbstractState& state,
    const Split& split) const
{
    return get_num_unwanted_values(state, split);
}

double SplitSelectorMinRefined::rate_split(
    const AbstractState& state,
    const Split& split) const
{
    return -get_refinedness(state, split.var_id);
}

double SplitSelectorMaxRefined::rate_split(
    const AbstractState& state,
    const Split& split) const
{
    return get_refinedness(state, split.var_id);
}

double
SplitSelectorMinHAdd::rate_split(const AbstractState&, const Split& split) const
{
    return -get_min_hadd_value(split.var_id, split.values);
}

double
SplitSelectorMaxHAdd::rate_split(const AbstractState&, const Split& split) const
{
    return get_max_hadd_value(split.var_id, split.values);
}

const Split& SplitSelectorRandom::pick_split(
    const AbstractState&,
    const std::vector<Split>& splits)
{
    assert(!splits.empty());

    if (splits.size() == 1) {
        return splits[0];
    }

    return *rng.choose(splits);
}

} // namespace cartesian
} // namespace heuristics
} // namespace probfd