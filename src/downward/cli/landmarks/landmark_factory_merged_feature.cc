#include "downward/cli/landmarks/landmark_factory_merged_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_merged.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;

namespace {
class LandmarkFactoryMergedFeature
    : public SharedTypedFeature<
          LandmarkFactory,
          const std::vector<std::shared_ptr<LandmarkFactory>>&,
          downward::utils::Verbosity> {
public:
    LandmarkFactoryMergedFeature()
        : TypedFeature("lm_merged", &LandmarkFactoryMergedFeature::func)
    {
        document_title("Merged Landmarks");
        document_synopsis(
            "Merges the landmarks and orderings from the parameter landmarks");

        document_note(
            "Precedence",
            "Fact landmarks take precedence over disjunctive landmarks, "
            "orderings take precedence in the usual manner "
            "(gn > nat > reas > o_reas). ");
        document_note(
            "Note",
            "Does not currently support conjunctive landmarks");

        document_language_support(
            "conditional_effects",
            "supported if all components support them");

        make_required_argument(0, "lm_factories");
        add_landmark_factory_options_to_feature(*this, 1);
    }

    static shared_ptr<LandmarkFactory> func(
        const std::vector<std::shared_ptr<LandmarkFactory>>& lm_factories,
        downward::utils::Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<LandmarkFactoryMerged>(
            lm_factories,
            verbosity);
    }
};
} // namespace

namespace downward::cli::landmarks {

void add_landmark_factory_merged_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<LandmarkFactoryMergedFeature>();
}

} // namespace downward::cli::landmarks
