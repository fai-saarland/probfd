#include "probfd/cli/cartesian_abstractions/split_selector.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cartesian_abstractions/split_selector.h"

#include "downward/utils/rng_options.h"

using namespace downward;
using namespace downward::utils;

using namespace downward::cli::plugins;

using namespace probfd::cartesian_abstractions;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {

class SplitSelectorRandomFactoryFeature
    : public SharedTypedFeature<SplitSelectorFactory> {
public:
    SplitSelectorRandomFactoryFeature()
        : SharedTypedFeature("random")
    {
        document_synopsis(
            "select a random variable (among all eligible variables)");

        add_rng_options_to_feature(*this);
    }

    std::shared_ptr<SplitSelectorFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<SplitSelectorRandomFactory>(
            get_rng(std::get<0>(get_rng_arguments_from_options(opts))));
    }
};

class SplitSelectorMinUnwantedFactoryFeature
    : public SharedTypedFeature<SplitSelectorFactory> {
public:
    SplitSelectorMinUnwantedFactoryFeature()
        : SharedTypedFeature("min_unwanted")
    {
        document_synopsis(
            "select an eligible variable which has the least unwanted values "
            "(number of values of v that land in the abstract state whose "
            "h-value will probably be raised) in the flaw state");
    }

    std::shared_ptr<SplitSelectorFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<SplitSelectorMinUnwantedFactory>();
    }
};

class SplitSelectorMaxUnwantedFactoryFeature
    : public SharedTypedFeature<SplitSelectorFactory> {
public:
    SplitSelectorMaxUnwantedFactoryFeature()
        : SharedTypedFeature("max_unwanted")
    {
        document_synopsis(
            "select an eligible variable which has the most unwanted values "
            "(number of values of v that land in the abstract state whose "
            "h-value will probably be raised) in the flaw state");
    }

    std::shared_ptr<SplitSelectorFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<SplitSelectorMaxUnwantedFactory>();
    }
};

class SplitSelectorMinRefinedFactoryFeature
    : public SharedTypedFeature<SplitSelectorFactory> {
public:
    SplitSelectorMinRefinedFactoryFeature()
        : SharedTypedFeature("min_refined")
    {
        document_synopsis(
            "select an eligible variable which is the least refined "
            "(-1 * (remaining_values(v) / original_domain_size(v))) "
            "in the flaw state");
    }

    std::shared_ptr<SplitSelectorFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<SplitSelectorMinRefinedFactory>();
    }
};

class SplitSelectorMaxRefinedFactoryFeature
    : public SharedTypedFeature<SplitSelectorFactory> {
public:
    SplitSelectorMaxRefinedFactoryFeature()
        : SharedTypedFeature("max_refined")
    {
        document_synopsis(
            "select an eligible variable which is the most refined "
            "(-1 * (remaining_values(v) / original_domain_size(v))) "
            "in the flaw state");
    }

    std::shared_ptr<SplitSelectorFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<SplitSelectorMaxRefinedFactory>();
    }
};

class SplitSelectorMinHAddFactoryFeature
    : public SharedTypedFeature<SplitSelectorFactory> {
public:
    SplitSelectorMinHAddFactoryFeature()
        : SharedTypedFeature("min_hadd")
    {
        document_synopsis(
            "select an eligible variable with minimal h^add(s_0) value "
            "over all facts that need to be removed from the flaw state");
    }

    std::shared_ptr<SplitSelectorFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<SplitSelectorMinHAddFactory>();
    }
};

class SplitSelectorMaxHAddFactoryFeature
    : public SharedTypedFeature<SplitSelectorFactory> {
public:
    SplitSelectorMaxHAddFactoryFeature()
        : SharedTypedFeature("max_hadd")
    {
        document_synopsis(
            "Select an eligible variable with maximal h^add(s_0) value "
            "over all facts that need to be removed from the flaw "
            "state");
    }

    std::shared_ptr<SplitSelectorFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<SplitSelectorMaxHAddFactory>();
    }
};
} // namespace

namespace probfd::cli::cartesian_abstractions {

void add_split_selector_features(RawRegistry& raw_registry)
{
    auto& category = raw_registry.insert_category_plugin<SplitSelectorFactory>(
        "SplitSelectorFactory");
    category.document_synopsis(
        "Factory for split selection algorithms used in the "
        "cartesian abstraction refinement loop");

    raw_registry.insert_feature_plugin<SplitSelectorRandomFactoryFeature>();
    raw_registry
        .insert_feature_plugin<SplitSelectorMinUnwantedFactoryFeature>();
    raw_registry
        .insert_feature_plugin<SplitSelectorMaxUnwantedFactoryFeature>();
    raw_registry.insert_feature_plugin<SplitSelectorMinRefinedFactoryFeature>();
    raw_registry.insert_feature_plugin<SplitSelectorMaxRefinedFactoryFeature>();
    raw_registry.insert_feature_plugin<SplitSelectorMinHAddFactoryFeature>();
    raw_registry.insert_feature_plugin<SplitSelectorMaxHAddFactoryFeature>();
}

} // namespace probfd::cli::cartesian_abstractions
