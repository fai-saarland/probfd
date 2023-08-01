#include "probfd/heuristics/cartesian/split_selector.h"

#include "probfd/heuristics/cartesian/abstract_state.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "downward/heuristics/additive_heuristic.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/plugins/plugin.h"

#include <cassert>
#include <iostream>
#include <limits>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

SplitSelectorRandom::SplitSelectorRandom(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng(std::move(rng))
{
}

SplitSelectorUnwanted::SplitSelectorUnwanted(int factor)
    : factor(factor)
{
}

SplitSelectorRefinedness::SplitSelectorRefinedness(
    const std::shared_ptr<ProbabilisticTask>& task,
    double factor)
    : task_proxy(*task)
    , factor(factor)
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

    return *rng->choose(splits);
}

double SplitSelectorUnwanted::rate_split(
    const AbstractState& state,
    const Split& split) const
{
    int num_unwanted_values = state.count(split.var_id) - split.values.size();
    assert(num_unwanted_values >= 1);
    return static_cast<double>(factor * num_unwanted_values);
}

double SplitSelectorRefinedness::rate_split(
    const AbstractState& state,
    const Split& split) const
{
    const int var_id = split.var_id;
    double all_values = task_proxy.get_variables()[var_id].get_domain_size();
    assert(all_values >= 2);
    double remaining_values = state.count(var_id);
    assert(2 <= remaining_values && remaining_values <= all_values);
    double refinedness = -(remaining_values / all_values);
    assert(-1.0 <= refinedness && refinedness < 0.0);
    return factor * refinedness;
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
    const plugins::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
{
}

std::unique_ptr<SplitSelector>
SplitSelectorRandomFactory::create_split_selector(
    const std::shared_ptr<ProbabilisticTask>&)
{
    return std::make_unique<SplitSelectorRandom>(rng);
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

static class SplitSelectorFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<SplitSelectorFactory> {
public:
    SplitSelectorFactoryCategoryPlugin()
        : TypedCategoryPlugin("SplitSelectorFactory")
    {
        document_synopsis("Factory for split selection algorithms used in the "
                          "cartesian abstraction refinement loop");
    }
} _category_plugin;

namespace {

class SplitSelectorRandomFactoryFeature
    : public plugins::
          TypedFeature<SplitSelectorFactory, SplitSelectorRandomFactory> {
public:
    SplitSelectorRandomFactoryFeature()
        : TypedFeature("random")
    {
        document_synopsis(
            "select a random variable (among all eligible variables)");

        utils::add_rng_options(*this);
    }
};

class SplitSelectorMinUnwantedFactoryFeature
    : public plugins::
          TypedFeature<SplitSelectorFactory, SplitSelectorMinUnwantedFactory> {
public:
    SplitSelectorMinUnwantedFactoryFeature()
        : TypedFeature("min_unwanted")
    {
        document_synopsis(
            "select an eligible variable which has the least unwanted values "
            "(number of values of v that land in the abstract state whose "
            "h-value will probably be raised) in the flaw state");
    }

    std::shared_ptr<SplitSelectorMinUnwantedFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<SplitSelectorMinUnwantedFactory>();
    }
};

class SplitSelectorMaxUnwantedFactoryFeature
    : public plugins::
          TypedFeature<SplitSelectorFactory, SplitSelectorMaxUnwantedFactory> {
public:
    SplitSelectorMaxUnwantedFactoryFeature()
        : TypedFeature("max_unwanted")
    {
        document_synopsis(
            "select an eligible variable which has the most unwanted values "
            "(number of values of v that land in the abstract state whose "
            "h-value will probably be raised) in the flaw state");
    }

    std::shared_ptr<SplitSelectorMaxUnwantedFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<SplitSelectorMaxUnwantedFactory>();
    }
};

class SplitSelectorMinRefinedFactoryFeature
    : public plugins::
          TypedFeature<SplitSelectorFactory, SplitSelectorMinRefinedFactory> {
public:
    SplitSelectorMinRefinedFactoryFeature()
        : TypedFeature("min_refined")
    {
        document_synopsis(
            "select an eligible variable which is the least refined "
            "(-1 * (remaining_values(v) / original_domain_size(v))) "
            "in the flaw state");
    }

    std::shared_ptr<SplitSelectorMinRefinedFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<SplitSelectorMinRefinedFactory>();
    }
};

class SplitSelectorMaxRefinedFactoryFeature
    : public plugins::
          TypedFeature<SplitSelectorFactory, SplitSelectorMaxRefinedFactory> {
public:
    SplitSelectorMaxRefinedFactoryFeature()
        : TypedFeature("max_refined")
    {
        document_synopsis(
            "select an eligible variable which is the most refined "
            "(-1 * (remaining_values(v) / original_domain_size(v))) "
            "in the flaw state");
    }

    std::shared_ptr<SplitSelectorMaxRefinedFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<SplitSelectorMaxRefinedFactory>();
    }
};

class SplitSelectorMinHAddFactoryFeature
    : public plugins::
          TypedFeature<SplitSelectorFactory, SplitSelectorMinHAddFactory> {
public:
    SplitSelectorMinHAddFactoryFeature()
        : TypedFeature("min_hadd")
    {
        document_synopsis(
            "select an eligible variable with minimal h^add(s_0) value "
            "over all facts that need to be removed from the flaw state");
    }

    std::shared_ptr<SplitSelectorMinHAddFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<SplitSelectorMinHAddFactory>();
    }
};

class SplitSelectorMaxHAddFactoryFeature
    : public plugins::
          TypedFeature<SplitSelectorFactory, SplitSelectorMaxHAddFactory> {
public:
    SplitSelectorMaxHAddFactoryFeature()
        : TypedFeature("max_hadd")
    {
        document_synopsis(
            "Select an eligible variable with maximal h^add(s_0) value "
            "over all facts that need to be removed from the flaw "
            "state");
    }

    std::shared_ptr<SplitSelectorMaxHAddFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<SplitSelectorMaxHAddFactory>();
    }
};

} // namespace

static plugins::FeaturePlugin<SplitSelectorRandomFactoryFeature> _plugin;
static plugins::FeaturePlugin<SplitSelectorMinUnwantedFactoryFeature> _plugin2;
static plugins::FeaturePlugin<SplitSelectorMaxUnwantedFactoryFeature> _plugin3;
static plugins::FeaturePlugin<SplitSelectorMinRefinedFactoryFeature> _plugin4;
static plugins::FeaturePlugin<SplitSelectorMaxRefinedFactoryFeature> _plugin5;
static plugins::FeaturePlugin<SplitSelectorMinHAddFactoryFeature> _plugin6;
static plugins::FeaturePlugin<SplitSelectorMaxHAddFactoryFeature> _plugin7;

} // namespace cartesian
} // namespace heuristics
} // namespace probfd