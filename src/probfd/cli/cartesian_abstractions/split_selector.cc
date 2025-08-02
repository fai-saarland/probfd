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
class SplitSelectorFactoryCategoryPlugin
    : public TypedCategoryPlugin<SplitSelectorFactory> {
public:
    SplitSelectorFactoryCategoryPlugin()
        : TypedCategoryPlugin("SplitSelectorFactory")
    {
        document_synopsis(
            "Factory for split selection algorithms used in the "
            "cartesian abstraction refinement loop");
    }
};

class SplitSelectorRandomFactoryFeature
    : public TypedFeature<SplitSelectorFactory, SplitSelectorRandomFactory> {
public:
    SplitSelectorRandomFactoryFeature()
        : TypedFeature("random")
    {
        document_synopsis(
            "select a random variable (among all eligible variables)");

        add_rng_options_to_feature(*this);
    }

    std::shared_ptr<SplitSelectorRandomFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<SplitSelectorRandomFactory>(
            get_rng(std::get<0>(get_rng_arguments_from_options(opts))));
    }
};

class SplitSelectorMinUnwantedFactoryFeature
    : public TypedFeature<
          SplitSelectorFactory,
          SplitSelectorMinUnwantedFactory> {
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
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<SplitSelectorMinUnwantedFactory>();
    }
};

class SplitSelectorMaxUnwantedFactoryFeature
    : public TypedFeature<
          SplitSelectorFactory,
          SplitSelectorMaxUnwantedFactory> {
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
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<SplitSelectorMaxUnwantedFactory>();
    }
};

class SplitSelectorMinRefinedFactoryFeature
    : public TypedFeature<
          SplitSelectorFactory,
          SplitSelectorMinRefinedFactory> {
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
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<SplitSelectorMinRefinedFactory>();
    }
};

class SplitSelectorMaxRefinedFactoryFeature
    : public TypedFeature<
          SplitSelectorFactory,
          SplitSelectorMaxRefinedFactory> {
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
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<SplitSelectorMaxRefinedFactory>();
    }
};

class SplitSelectorMinHAddFactoryFeature
    : public TypedFeature<SplitSelectorFactory, SplitSelectorMinHAddFactory> {
public:
    SplitSelectorMinHAddFactoryFeature()
        : TypedFeature("min_hadd")
    {
        document_synopsis(
            "select an eligible variable with minimal h^add(s_0) value "
            "over all facts that need to be removed from the flaw state");
    }

    std::shared_ptr<SplitSelectorMinHAddFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<SplitSelectorMinHAddFactory>();
    }
};

class SplitSelectorMaxHAddFactoryFeature
    : public TypedFeature<SplitSelectorFactory, SplitSelectorMaxHAddFactory> {
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
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<SplitSelectorMaxHAddFactory>();
    }
};
}

namespace probfd::cli::cartesian_abstractions {

void add_split_selector_features(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<SplitSelectorFactoryCategoryPlugin>();

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

} // namespace
