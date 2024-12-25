#include "downward/cli/plugins/plugin.h"

#include "downward/cli/open_list_options.h"

#include "downward/open_lists/best_first_open_list.h"

#include "downward/utils/memory.h"

using namespace std;
using namespace standard_scalar_open_list;
using namespace utils;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;
using downward::cli::get_open_list_arguments_from_options;

namespace {

class BestFirstOpenListFeature
    : public TypedFeature<OpenListFactory, BestFirstOpenListFactory> {
public:
    BestFirstOpenListFeature()
        : TypedFeature("single")
    {
        document_title("Best-first open list");
        document_synopsis(
            "Open list that uses a single evaluator and FIFO tiebreaking.");

        add_option<shared_ptr<Evaluator>>("eval", "evaluator");
        add_open_list_options_to_feature(*this);

        document_note(
            "Implementation Notes",
            "Elements with the same evaluator value are stored in double-ended "
            "queues, called \"buckets\". The open list stores a map from "
            "evaluator "
            "values to buckets. Pushing and popping from a bucket runs in "
            "constant "
            "time. Therefore, inserting and removing an entry from the open "
            "list "
            "takes time O(log(n)), where n is the number of buckets.");
    }

    virtual shared_ptr<BestFirstOpenListFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<BestFirstOpenListFactory>(
            opts.get<shared_ptr<Evaluator>>("eval"),
            get_open_list_arguments_from_options(opts));
    }
};

FeaturePlugin<BestFirstOpenListFeature> _plugin;

} // namespace
