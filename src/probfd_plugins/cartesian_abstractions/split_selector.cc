#include "probfd/cartesian_abstractions/split_selector.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/rng_options.h"

using namespace probfd::cartesian_abstractions;

namespace {

class SplitSelectorFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<SplitSelectorFactory> {
public:
    SplitSelectorFactoryCategoryPlugin()
        : TypedCategoryPlugin("SplitSelectorFactory")
    {
        document_synopsis("Factory for split selection algorithms used in the "
                          "cartesian abstraction refinement loop");
    }
} _category_plugin;

class SplitSelectorRandomFactoryFeature
    : public plugins::
          TypedFeature<SplitSelectorFactory, SplitSelectorRandomFactory> {
public:
    SplitSelectorRandomFactoryFeature()
        : TypedFeature("random")
    {
        document_synopsis(
            "select a random variable (among all eligible variables)");

        utils::add_rng_options_to_feature(*this);
    }

    std::shared_ptr<SplitSelectorRandomFactory>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<SplitSelectorRandomFactory>(
            utils::get_rng(
                std::get<0>(utils::get_rng_arguments_from_options(opts))));
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

plugins::FeaturePlugin<SplitSelectorRandomFactoryFeature> _plugin;
plugins::FeaturePlugin<SplitSelectorMinUnwantedFactoryFeature> _plugin2;
plugins::FeaturePlugin<SplitSelectorMaxUnwantedFactoryFeature> _plugin3;
plugins::FeaturePlugin<SplitSelectorMinRefinedFactoryFeature> _plugin4;
plugins::FeaturePlugin<SplitSelectorMaxRefinedFactoryFeature> _plugin5;
plugins::FeaturePlugin<SplitSelectorMinHAddFactoryFeature> _plugin6;
plugins::FeaturePlugin<SplitSelectorMaxHAddFactoryFeature> _plugin7;

} // namespace
