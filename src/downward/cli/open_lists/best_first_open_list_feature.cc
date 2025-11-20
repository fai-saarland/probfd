#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/open_lists/open_list_options.h"

#include "downward/open_lists/best_first_open_list.h"

#include "downward/utils/memory.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward::standard_scalar_open_list;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;

namespace {
template <typename T>
class BestFirstOpenListFeature
    : public SharedTypedFeature<
          downward::TaskDependentFactory<downward::OpenList<T>>,
          bool> {
public:
    BestFirstOpenListFeature()
        requires(std::same_as<T, downward::StateOpenListEntry>)
        : BestFirstOpenListFeature::TypedFeature(
              "state_single",
              &BestFirstOpenListFeature::func)
    {
        this->document_title("Best-first state open list");
        this->document_synopsis(
            "Open list that uses a single evaluator and FIFO tiebreaking.");

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

        add_open_list_options_to_feature(*this, 0);
    }

    BestFirstOpenListFeature()
        requires(std::same_as<T, downward::EdgeOpenListEntry>)
        : BestFirstOpenListFeature::TypedFeature(
              "edge_single",
              &BestFirstOpenListFeature::func)
    {
        this->document_title("Best-first edge open list");
        this->document_synopsis(
            "Open list that uses a single evaluator and FIFO tiebreaking.");

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

        add_open_list_options_to_feature(*this, 0);
    }

    static shared_ptr<downward::TaskDependentFactory<downward::OpenList<T>>>
    func(bool pref_only)
    {
        return make_shared<BestFirstOpenListFactory<T>>(nullptr, pref_only);
    }
};

template <typename T>
class BestFirstOpenListFeatureOverload
    : public SharedTypedFeature<
          downward::TaskDependentFactory<downward::OpenList<T>>,
          shared_ptr<downward::TaskDependentFactory<downward::Evaluator>>,
          bool> {
public:
    BestFirstOpenListFeatureOverload()
        requires(std::same_as<T, downward::StateOpenListEntry>)
        : BestFirstOpenListFeatureOverload::TypedFeature(
              "state_single_with_eval",
              &BestFirstOpenListFeatureOverload::func)
    {
        this->document_title("Best-first state open list");
        this->document_synopsis(
            "Open list that uses a single evaluator and FIFO tiebreaking.");

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

        this->make_required_argument(0, "eval", "evaluator");
        add_open_list_options_to_feature(*this, 1);
    }

    BestFirstOpenListFeatureOverload()
        requires(std::same_as<T, downward::EdgeOpenListEntry>)
        : BestFirstOpenListFeatureOverload::TypedFeature(
              "edge_single_with_eval",
              &BestFirstOpenListFeatureOverload::func)
    {
        this->document_title("Best-first edge open list");
        this->document_synopsis(
            "Open list that uses a single evaluator and FIFO tiebreaking.");

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

        this->make_required_argument(0, "eval", "evaluator");
        add_open_list_options_to_feature(*this, 1);
    }

    static shared_ptr<downward::TaskDependentFactory<downward::OpenList<T>>>
    func(
        shared_ptr<downward::TaskDependentFactory<downward::Evaluator>>
            evaluator,
        bool pref_only)
    {
        return make_shared<BestFirstOpenListFactory<T>>(
            std::move(evaluator),
            pref_only);
    }
};

} // namespace

namespace downward::cli::open_lists {

void add_best_first_open_list_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<
        BestFirstOpenListFeature<downward::StateOpenListEntry>>();
    n.insert_feature_plugin<
        BestFirstOpenListFeature<downward::EdgeOpenListEntry>>();

    n.insert_feature_plugin<
        BestFirstOpenListFeatureOverload<downward::StateOpenListEntry>>();
    n.insert_feature_plugin<
        BestFirstOpenListFeatureOverload<downward::EdgeOpenListEntry>>();
}

} // namespace downward::cli::open_lists
