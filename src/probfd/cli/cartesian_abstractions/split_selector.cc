#include "probfd/cli/cartesian_abstractions/split_selector.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cartesian_abstractions/split_selector.h"

#include "downward/utils/rng_options.h"

using namespace downward;
using namespace downward::utils;

using namespace downward::cli::plugins;

using namespace probfd::cartesian_abstractions;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

std::shared_ptr<SplitSelectorFactory>
create_split_selector_random(int random_seed)
{
    return make_shared_from_arg_tuples<SplitSelectorRandomFactory>(
        get_rng(random_seed));
}

Feature& add_split_selector_random_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "random",
        create_split_selector_random);

    f.document_synopsis(
        "select a random variable (among all eligible variables)");

    add_rng_options_to_feature(f, 0);

    return f;
}

Feature& add_split_selector_min_unwanted_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "min_unwanted",
        &cli::plugins::
            make_shared<SplitSelectorFactory, SplitSelectorMinUnwantedFactory>);

    f.document_synopsis(
        "select an eligible variable which has the least unwanted values "
        "(number of values of v that land in the abstract state whose "
        "h-value will probably be raised) in the flaw state");

    return f;
}

Feature& add_split_selector_max_unwanted_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "max_unwanted",
        &cli::plugins::
            make_shared<SplitSelectorFactory, SplitSelectorMaxUnwantedFactory>);

    f.document_synopsis(
        "select an eligible variable which has the most unwanted values "
        "(number of values of v that land in the abstract state whose "
        "h-value will probably be raised) in the flaw state");

    return f;
}

Feature& add_split_selector_min_refined_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "min_refined",
        &cli::plugins::
            make_shared<SplitSelectorFactory, SplitSelectorMinRefinedFactory>);

    f.document_synopsis(
        "select an eligible variable which is the least refined "
        "(-1 * (remaining_values(v) / original_domain_size(v))) "
        "in the flaw state");

    return f;
}

Feature& add_split_selector_max_refined_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "max_refined",
        &cli::plugins::
            make_shared<SplitSelectorFactory, SplitSelectorMaxRefinedFactory>);

    f.document_synopsis(
        "select an eligible variable which is the most refined "
        "(-1 * (remaining_values(v) / original_domain_size(v))) "
        "in the flaw state");

    return f;
}

Feature& add_split_selector_min_hadd_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "min_hadd",
        &cli::plugins::
            make_shared<SplitSelectorFactory, SplitSelectorMinHAddFactory>);

    f.document_synopsis(
        "select an eligible variable with minimal h^add(s_0) value "
        "over all facts that need to be removed from the flaw state");

    return f;
}

Feature& add_split_selector_max_hadd_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "max_hadd",
        &cli::plugins::
            make_shared<SplitSelectorFactory, SplitSelectorMaxHAddFactory>);

    f.document_synopsis(
        "Select an eligible variable with maximal h^add(s_0) value "
        "over all facts that need to be removed from the flaw "
        "state");

    return f;
}

} // namespace

namespace probfd::cli::cartesian_abstractions {

void add_split_selector_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugin<SplitSelectorFactory>(
        "SplitSelectorFactory",
        "Factory for split selection algorithms used in the "
        "cartesian abstraction refinement loop");
}

void add_split_selector_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_split_selector_random_to_namespace(n);
    add_split_selector_min_refined_to_namespace(n);
    add_split_selector_max_refined_to_namespace(n);
    add_split_selector_min_unwanted_to_namespace(n);
    add_split_selector_max_unwanted_to_namespace(n);
    add_split_selector_min_hadd_to_namespace(n);
    add_split_selector_max_hadd_to_namespace(n);
}

} // namespace probfd::cli::cartesian_abstractions
