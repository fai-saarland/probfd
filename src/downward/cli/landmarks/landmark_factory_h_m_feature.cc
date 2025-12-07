#include "downward/cli/landmarks/landmark_factory_h_m_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

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

InternalFunctionDefinitionBase& add_landmark_factory_hm_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "lm_hm",
        &downward::cli::plugins::construct_shared<
            LandmarkFactory,
            LandmarkFactoryHM,
            std::shared_ptr<TaskDependentFactory<MutexInformation>>,
            int,
            bool,
            bool,
            utils::Verbosity>);
    // document_group("");
    f.document_title("h^m Landmarks");
    f.document_synopsis(
        "The landmark generation method introduced by "
        "Keyder, Richter & Helmert (ECAI 2010).");

    f.document_language_support(
        "conditional_effects",
        "ignored, i.e. not supported");

    f.make_optional_argument_with_default(
        0,
        "m",
        "2",
        "subset size (if unsure, use the default of 2)");
    f.make_required_argument(1, "mutexes", "factory for mutexes");
    f.make_optional_argument_with_default(
        2,
        "conjunctive_landmarks",
        "true",
        "keep conjunctive landmarks");
    const auto n = add_use_orders_option_to_feature(f, 3);
    add_landmark_factory_options_to_feature(f, n + 3);

    return f;
}

} // namespace

namespace downward::cli::landmarks {

void add_landmark_factory_hm_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_landmark_factory_hm_to_namespace(n);
}

} // namespace downward::cli::landmarks
