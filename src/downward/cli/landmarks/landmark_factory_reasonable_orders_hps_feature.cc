#include "downward/cli/landmarks/landmark_factory_reasonable_orders_hps_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/task_dependent_factory.h"

#include "downward/landmarks/landmark_factory_reasonable_orders_hps.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace downward;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;

namespace {
class LandmarkFactoryReasonableOrdersHPSFeature
    : public SharedTypedFeature<
          LandmarkFactory,
          const std::shared_ptr<LandmarkFactory>&,
          std::shared_ptr<TaskDependentFactory<MutexInformation>>,
          utils::Verbosity> {
public:
    LandmarkFactoryReasonableOrdersHPSFeature()
        : TypedFeature(
              "lm_reasonable_orders_hps",
              &LandmarkFactoryReasonableOrdersHPSFeature::func)
    {
        document_title("HPS Orders");
        document_synopsis(
            "Adds reasonable orders described in the following paper" +
            format_journal_reference(
                {"Jörg Hoffmann", "Julie Porteous", "Laura Sebastia"},
                "Ordered Landmarks in Planning",
                "https://jair.org/index.php/jair/article/view/10390/24882",
                "Journal of Artificial Intelligence Research",
                "22",
                "215-278",
                "2004"));

        document_note(
            "Obedient-reasonable orders",
            "Hoffmann et al. (2004) suggest obedient-reasonable orders in "
            "addition to reasonable orders. Obedient-reasonable orders were "
            "later also used by the LAMA planner (Richter and Westphal, 2010). "
            "They are \"reasonable orders\" under the assumption that all "
            "(non-obedient) reasonable orders are actually \"natural\", i.e., "
            "every plan obeys the reasonable orders. We observed "
            "experimentally that obedient-reasonable orders have minimal "
            "effect on the performance of LAMA (Büchner et al., 2023) and "
            "decided to remove them in issue1089.");

        // TODO: correct?
        document_language_support(
            "conditional_effects",
            "supported if subcomponent supports them");

        make_required_argument(0, "lm_factory");
        make_required_argument(1, "mutexes", "factory for mutexes");

        add_landmark_factory_options_to_feature(*this, 2);
    }

    static shared_ptr<LandmarkFactory> func(
        const Context&,
        const std::shared_ptr<LandmarkFactory>& lm_factory,
        std::shared_ptr<TaskDependentFactory<MutexInformation>> mutex_factory,
        utils::Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<LandmarkFactoryReasonableOrdersHPS>(
            lm_factory,
            std::move(mutex_factory),
            verbosity);
    }
};
} // namespace

namespace downward::cli::landmarks {

void add_landmark_factory_reasonable_orders_hps_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<LandmarkFactoryReasonableOrdersHPSFeature>();
}

} // namespace downward::cli::landmarks
