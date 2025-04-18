#include "downward/cli/landmarks/landmark_heuristic_options.h"

#include "downward/cli/heuristic_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/landmarks/landmark_factory.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::landmarks;

namespace downward::cli::landmarks {

void add_landmark_heuristic_options_to_feature(
    plugins::Feature& feature,
    const string& description)
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

    feature.add_option<shared_ptr<LandmarkFactory>>(
        "lm_factory",
        "the set of landmarks to use for this heuristic. "
        "The set of landmarks can be specified here, "
        "or predefined (see LandmarkFactory).");
    feature.add_option<bool>(
        "pref",
        "enable preferred operators (see note below)",
        "false");
    /* TODO: Do we really want these options or should we just always progress
        everything we can? */
    feature.add_option<bool>("prog_goal", "Use goal progression.", "true");
    feature.add_option<bool>(
        "prog_gn",
        "Use greedy-necessary ordering progression.",
        "true");
    feature.add_option<bool>(
        "prog_r",
        "Use reasonable ordering progression.",
        "true");
    add_heuristic_options_to_feature(feature, description);

    feature.document_property(
        "preferred operators",
        "yes (if enabled; see ``pref`` option)");
}

tuple<
    shared_ptr<TaskTransformation>,
    bool,
    string,
    utils::Verbosity,
    shared_ptr<LandmarkFactory>,
    bool,
    bool,
    bool,
    bool>
get_landmark_heuristic_arguments_from_options(const plugins::Options& opts)
{
    return tuple_cat(
        get_heuristic_arguments_from_options(opts),
        make_tuple(
            opts.get<shared_ptr<LandmarkFactory>>("lm_factory"),
            opts.get<bool>("pref"),
            opts.get<bool>("prog_goal"),
            opts.get<bool>("prog_gn"),
            opts.get<bool>("prog_r")));
}

} // namespace downward::cli::landmarks
