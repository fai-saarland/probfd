#include "downward/cli/landmarks/landmark_heuristic_options.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "language/plugins/plugin.h"

#include "downward/landmarks/landmark_factory.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::landmarks;

namespace downward::cli::landmarks {

std::size_t add_landmark_heuristic_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    const string& description,
    std::size_t start_index)
{
    feature.document_synopsis(
        "Landmark progression is implemented according to the following "
        "paper:" +
        utils::format_conference_reference(
            {"Clemens Büchner",
             "Thomas Keller",
             "Salomé Eriksson",
             "Malte Helmert"},
            "Landmarks Progression in Heuristic Search",
            "https://ai.dmi.unibas.ch/papers/buechner-et-al-icaps2023.pdf",
            "Proceedings of the Thirty-Third International Conference on "
            "Automated Planning and Scheduling (ICAPS 2023)",
            "70-79",
            "AAAI Press",
            "2023"));

    feature.document_property(
        "preferred operators",
        "yes (if enabled; see ``pref`` option)");

    feature.make_required_argument(
        start_index,
        "lm_factory",
        "the set of landmarks to use for this heuristic. "
        "The set of landmarks can be specified here, "
        "or predefined (see LandmarkFactory).");
    feature.make_optional_argument_with_default(
        start_index + 1,
        "pref",
        "false",
        "enable preferred operators (see note below)");
    /* TODO: Do we really want these options or should we just always progress
        everything we can? */
    feature.make_optional_argument_with_default(
        start_index + 2,
        "prog_goal",
        "true",
        "Use goal progression.");
    feature.make_optional_argument_with_default(
        start_index + 3,
        "prog_gn",
        "true",
        "Use greedy-necessary ordering progression.");
    feature.make_optional_argument_with_default(
        start_index + 4,
        "prog_r",
        "true",
        "Use reasonable ordering progression.");

    const auto n =
        add_heuristic_options_to_feature(feature, description, start_index + 5);

    return n + 5;
}

} // namespace downward::cli::landmarks
