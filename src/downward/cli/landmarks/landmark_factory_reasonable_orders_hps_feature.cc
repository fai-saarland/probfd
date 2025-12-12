#include "downward/cli/landmarks/landmark_factory_reasonable_orders_hps_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/task_dependent_factory.h"

#include "downward/landmarks/landmark_factory_reasonable_orders_hps.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace downward;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace language::plugins;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;

namespace {

InternalFunctionDefinitionBase&
add_landmark_factory_reasonable_orders_hps_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "lm_reasonable_orders_hps",
        &language::plugins::construct_shared<
            LandmarkFactory,
            LandmarkFactoryReasonableOrdersHPS,
            const std::shared_ptr<LandmarkFactory>&,
            std::shared_ptr<TaskDependentFactory<MutexInformation>>,
            utils::Verbosity>);
    f.document_title("HPS Orders");
    f.document_synopsis(
        "Adds reasonable orders described in the following paper" +
        format_journal_reference(
            {"Jörg Hoffmann", "Julie Porteous", "Laura Sebastia"},
            "Ordered Landmarks in Planning",
            "https://jair.org/index.php/jair/article/view/10390/24882",
            "Journal of Artificial Intelligence Research",
            "22",
            "215-278",
            "2004"));

    f.document_note(
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
    f.document_language_support(
        "conditional_effects",
        "supported if subcomponent supports them");

    f.make_required_argument(0, "lm_factory");
    f.make_required_argument(1, "mutexes", "factory for mutexes");

    add_landmark_factory_options_to_feature(f, 2);

    return f;
}

} // namespace

namespace downward::cli::landmarks {

void add_landmark_factory_reasonable_orders_hps_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_landmark_factory_reasonable_orders_hps_to_namespace(n);
}

} // namespace downward::cli::landmarks
