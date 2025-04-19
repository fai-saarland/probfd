#include "downward/cli/plugins/plugin.h"

#include "downward/cli/open_list_options.h"

#include "downward/open_lists/best_first_open_list.h"

#include "downward/utils/memory.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward::standard_scalar_open_list;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;
using downward::cli::get_open_list_arguments_from_options;

namespace {

template <typename T>
class BestFirstOpenListFeature
    : public TypedFeature<
          downward::TaskDependentFactory<downward::OpenList<T>>,
          BestFirstOpenListFactory<T>> {
public:
    BestFirstOpenListFeature()
        requires(std::same_as<T, downward::StateOpenListEntry>)
        : BestFirstOpenListFeature::TypedFeature("state_single")
    {
        this->document_title("Best-first state open list");
        this->document_synopsis(
            "Open list that uses a single evaluator and FIFO tiebreaking.");

        this->template add_option<
            shared_ptr<downward::TaskDependentFactory<downward::Evaluator>>>(
            "eval",
            "evaluator");
        add_open_list_options_to_feature(*this);

        this->document_note(
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

    BestFirstOpenListFeature()
        requires(std::same_as<T, downward::EdgeOpenListEntry>)
        : BestFirstOpenListFeature::TypedFeature("edge_single")
    {
        this->document_title("Best-first edge open list");
        this->document_synopsis(
            "Open list that uses a single evaluator and FIFO tiebreaking.");

        this->template add_option<
            shared_ptr<downward::TaskDependentFactory<downward::Evaluator>>>(
            "eval",
            "evaluator");
        add_open_list_options_to_feature(*this);

        this->document_note(
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

    shared_ptr<BestFirstOpenListFactory<T>>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<BestFirstOpenListFactory<T>>(
            opts.get<shared_ptr<
                downward::TaskDependentFactory<downward::Evaluator>>>("eval"),
            get_open_list_arguments_from_options(opts));
    }
};

FeaturePlugin<BestFirstOpenListFeature<downward::StateOpenListEntry>> _plugin;
FeaturePlugin<BestFirstOpenListFeature<downward::EdgeOpenListEntry>> _plugin2;

} // namespace
