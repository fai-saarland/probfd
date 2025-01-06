#include "probfd/cartesian_abstractions/split_selector.h"

#include "probfd/cartesian_abstractions/abstract_state.h"
#include "probfd/cartesian_abstractions/utils.h"

#include "probfd/probabilistic_task.h"

#include "downward/heuristics/additive_heuristic.h"

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include <cassert>
#include <limits>
#include <utility>

using namespace std;

namespace utils {
class Context;
}

namespace probfd::cartesian_abstractions {

SplitSelectorRandom::SplitSelectorRandom(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng_(std::move(rng))
{
}

SplitSelectorUnwanted::SplitSelectorUnwanted(int factor)
    : factor_(factor)
{
}

SplitSelectorRefinedness::SplitSelectorRefinedness(
    const std::shared_ptr<ProbabilisticTask>& task,
    double factor)
    : task_proxy_(*task)
    , factor_(factor)
{
}

SplitSelectorHAdd::SplitSelectorHAdd(
    const std::shared_ptr<ProbabilisticTask>& task)
    : task_(task)
    , task_proxy_(*task)
    , additive_heuristic_(create_additive_heuristic(task))
{
    additive_heuristic_->compute_heuristic_for_cegar(
        task_proxy_.get_initial_state());
}

SplitSelectorHAdd::~SplitSelectorHAdd() = default;

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

const Split& SplitSelectorRandom::pick_split(
    const AbstractState&,
    const std::vector<Split>& splits)
{
    assert(!splits.empty());

    if (splits.size() == 1) {
        return splits[0];
    }

    return *rng_->choose(splits);
}

double SplitSelectorUnwanted::rate_split(
    const AbstractState& state,
    const Split& split) const
{
    int num_unwanted_values =
        state.count(split.var_id) - static_cast<int>(split.values.size());
    assert(num_unwanted_values >= 1);
    return static_cast<double>(factor_ * num_unwanted_values);
}

double SplitSelectorRefinedness::rate_split(
    const AbstractState& state,
    const Split& split) const
{
    const int var_id = split.var_id;
    double all_values = task_proxy_.get_variables()[var_id].get_domain_size();
    assert(all_values >= 2);
    double remaining_values = state.count(var_id);
    assert(2 <= remaining_values && remaining_values <= all_values);
    double refinedness = -(remaining_values / all_values);
    assert(-1.0 <= refinedness && refinedness < 0.0);
    return factor_ * refinedness;
}

int SplitSelectorHAdd::get_hadd_value(int var_id, int value) const
{
    assert(additive_heuristic_);
    int hadd = additive_heuristic_->get_cost_for_cegar(var_id, value);
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

SplitSelectorRandomFactory::SplitSelectorRandomFactory(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng_(std::move(rng))
{
}

std::unique_ptr<SplitSelector>
SplitSelectorRandomFactory::create_split_selector(
    const std::shared_ptr<ProbabilisticTask>&)
{
    return std::make_unique<SplitSelectorRandom>(rng_);
}

std::unique_ptr<SplitSelector>
SplitSelectorMinUnwantedFactory::create_split_selector(
    const std::shared_ptr<ProbabilisticTask>&)
{
    return std::make_unique<SplitSelectorUnwanted>(-1);
}

std::unique_ptr<SplitSelector>
SplitSelectorMaxUnwantedFactory::create_split_selector(
    const std::shared_ptr<ProbabilisticTask>&)
{
    return std::make_unique<SplitSelectorUnwanted>(1);
}

std::unique_ptr<SplitSelector>
SplitSelectorMinRefinedFactory::create_split_selector(
    const std::shared_ptr<ProbabilisticTask>& task)
{
    return std::make_unique<SplitSelectorRefinedness>(task, -1);
}

std::unique_ptr<SplitSelector>
SplitSelectorMaxRefinedFactory::create_split_selector(
    const std::shared_ptr<ProbabilisticTask>& task)
{
    return std::make_unique<SplitSelectorRefinedness>(task, 1);
}

std::unique_ptr<SplitSelector>
SplitSelectorMinHAddFactory::create_split_selector(
    const std::shared_ptr<ProbabilisticTask>& task)
{
    return std::make_unique<SplitSelectorMinHAdd>(task);
}

std::unique_ptr<SplitSelector>
SplitSelectorMaxHAddFactory::create_split_selector(
    const std::shared_ptr<ProbabilisticTask>& task)
{
    return std::make_unique<SplitSelectorMaxHAdd>(task);
}

} // namespace probfd::cartesian_abstractions
