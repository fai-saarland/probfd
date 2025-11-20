#include "downward/cli/landmarks/landmark_factory_h_m_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/task_dependent_factory.h"

#include "downward/landmarks/landmark_factory_h_m.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::landmarks::add_use_orders_option_to_feature;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;

namespace {
class LandmarkFactoryHMFeature
    : public SharedTypedFeature<
          LandmarkFactory,
          std::shared_ptr<TaskDependentFactory<MutexInformation>>,
          int,
          bool,
          bool,
          utils::Verbosity> {
public:
    LandmarkFactoryHMFeature()
        : TypedFeature("lm_hm", &LandmarkFactoryHMFeature::func)
    {
        // document_group("");
        document_title("h^m Landmarks");
        document_synopsis(
            "The landmark generation method introduced by "
            "Keyder, Richter & Helmert (ECAI 2010).");

        document_language_support(
            "conditional_effects",
            "ignored, i.e. not supported");

        make_optional_argument_with_default(
            0,
            "m",
            "2",
            "subset size (if unsure, use the default of 2)");
        make_required_argument(1, "mutexes", "factory for mutexes");
        make_optional_argument_with_default(
            2,
            "conjunctive_landmarks",
            "true",
            "keep conjunctive landmarks");
        const auto n = add_use_orders_option_to_feature(*this, 3);
        add_landmark_factory_options_to_feature(*this, n + 3);
    }

    static shared_ptr<LandmarkFactory> func(
        std::shared_ptr<TaskDependentFactory<MutexInformation>> mutex_factory,
        int m,
        bool conjunctive_landmarks,
        bool use_orders,
        utils::Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<LandmarkFactoryHM>(
            std::move(mutex_factory),
            m,
            conjunctive_landmarks,
            use_orders,
            verbosity);
    }
};
} // namespace

namespace downward::cli::landmarks {

void add_landmark_factory_hm_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<LandmarkFactoryHMFeature>();
}

} // namespace downward::cli::landmarks
