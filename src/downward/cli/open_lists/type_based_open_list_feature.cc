#include "downward/cli/open_lists/type_based_open_list_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/open_lists/open_list_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/open_lists/type_based_open_list.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::type_based_open_list;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;

using downward::cli::utils::add_rng_options_to_feature;

namespace {
template <typename T>
class TypeBasedOpenListFeature
    : public SharedTypedFeature<
          downward::TaskDependentFactory<downward::OpenList<T>>,
          const std::vector<std::shared_ptr<
              downward::TaskDependentFactory<downward::Evaluator>>>&,
          int> {
public:
    TypeBasedOpenListFeature()
        requires(std::same_as<T, downward::StateOpenListEntry>)
        : TypeBasedOpenListFeature::TypedFeature(
              "state_type_based",
              &TypeBasedOpenListFeature::func)
    {
        this->document_title("Type-based state open list");
        this->document_synopsis(
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

        this->make_required_argument(
            0,
            "evaluators",
            "Evaluators used to determine the bucket for each entry.");
        add_rng_options_to_feature(*this, 1);
    }

    TypeBasedOpenListFeature()
        requires(std::same_as<T, downward::EdgeOpenListEntry>)
        : TypeBasedOpenListFeature::TypedFeature(
              "edge_type_based",
              &TypeBasedOpenListFeature::func)
    {
        this->document_title("Type-based edge open list");
        this->document_synopsis(
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

        this->make_required_argument(
            0,
            "evaluators",
            "Evaluators used to determine the bucket for each entry.");
        add_rng_options_to_feature(*this, 1);
    }

    static shared_ptr<downward::TaskDependentFactory<downward::OpenList<T>>>
    func(
        const std::vector<std::shared_ptr<
            downward::TaskDependentFactory<downward::Evaluator>>>& factories,
        int random_seed)
    {
        return make_shared_from_arg_tuples<TypeBasedOpenListFactory<T>>(
            factories,
            random_seed);
    }
};
} // namespace

namespace downward::cli::open_lists {

void add_type_based_open_list_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<
        TypeBasedOpenListFeature<downward::StateOpenListEntry>>();
    n.insert_feature_plugin<
        TypeBasedOpenListFeature<downward::EdgeOpenListEntry>>();
}

} // namespace downward::cli::open_lists
