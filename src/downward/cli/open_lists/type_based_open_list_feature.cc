#include "downward/cli/plugins/plugin.h"

#include "downward/cli/open_list_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/open_lists/type_based_open_list.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::type_based_open_list;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;
using downward::cli::get_open_list_arguments_from_options;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {

class TypeBasedOpenListFeature
    : public TypedFeature<downward::OpenListFactory, TypeBasedOpenListFactory> {
public:
    TypeBasedOpenListFeature()
        : TypedFeature("type_based")
    {
        document_title("Type-based open list");
        document_synopsis(
            "Uses multiple evaluators to assign entries to buckets. "
            "All entries in a bucket have the same evaluator values. "
            "When retrieving an entry, a bucket is chosen uniformly at "
            "random and one of the contained entries is selected "
            "uniformly randomly. "
            "The algorithm is based on" +
            format_conference_reference(
                {"Fan Xie", "Martin Mueller", "Robert Holte", "Tatsuya Imai"},
                "Type-Based Exploration with Multiple Search Queues for"
                " Satisficing Planning",
                "http://www.aaai.org/ocs/index.php/AAAI/AAAI14/paper/view/8472/"
                "8705",
                "Proceedings of the Twenty-Eigth AAAI Conference Conference"
                " on Artificial Intelligence (AAAI 2014)",
                "2395-2401",
                "AAAI Press",
                "2014"));

        add_list_option<shared_ptr<downward::Evaluator>>(
            "evaluators",
            "Evaluators used to determine the bucket for each entry.");
        add_rng_options_to_feature(*this);
    }

    virtual shared_ptr<TypeBasedOpenListFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        verify_list_non_empty<shared_ptr<downward::Evaluator>>(
            context,
            opts,
            "evaluators");
        return make_shared_from_arg_tuples<TypeBasedOpenListFactory>(
            opts.get_list<shared_ptr<downward::Evaluator>>("evaluators"),
            get_rng_arguments_from_options(opts));
    }
};

FeaturePlugin<TypeBasedOpenListFeature> _plugin;

} // namespace
