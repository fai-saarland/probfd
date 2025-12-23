#include "downward/cli/landmarks/landmark_factory_merged_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_merged.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace language::plugins;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;

namespace downward::cli::landmarks {

InternalFunctionDefinitionBase&
add_landmark_factory_merged_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "lm_merged",
        &language::plugins::construct_shared<
            LandmarkFactory,
            LandmarkFactoryMerged,
            std::vector<std::shared_ptr<LandmarkFactory>>,
            downward::utils::Verbosity>);
    f.document_title("Merged Landmarks");
    f.document_synopsis(
        "Merges the landmarks and orderings from the parameter landmarks");

    f.document_note(
        "Precedence",
        "Fact landmarks take precedence over disjunctive landmarks, "
        "orderings take precedence in the usual manner "
        "(gn > nat > reas > o_reas). ");
    f.document_note("Note", "Does not currently support conjunctive landmarks");

    f.document_language_support(
        "conditional_effects",
        "supported if all components support them");

    f.make_required_argument(0, "lm_factories");
    add_landmark_factory_options_to_feature(f, 1);

    return f;
}

} // namespace downward::cli::landmarks
