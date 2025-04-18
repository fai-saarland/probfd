#include "downward/cli/potentials/potential_options.h"

#include "downward/cli/heuristic_options.h"

#include "downward/cli/lp/lp_solver_options.h"
#include "downward/cli/plugins/plugin.h"

#include "downward/utils/markup.h"

using namespace std;

namespace downward::cli::potentials {

string get_admissible_potentials_reference()
{
    return "The algorithm is based on" +
           utils::format_conference_reference(
               {"Jendrik Seipp", "Florian Pommerening", "Malte Helmert"},
               "New Optimization Functions for Potential Heuristics",
               "https://ai.dmi.unibas.ch/papers/seipp-et-al-icaps2015.pdf",
               "Proceedings of the 25th International Conference on"
               " Automated Planning and Scheduling (ICAPS 2015)",
               "193-201",
               "AAAI Press",
               "2015");
}

void add_admissible_potentials_options_to_feature(
    plugins::Feature& feature,
    const string& description)
{
    feature.document_language_support("action costs", "supported");
    feature.document_language_support("conditional effects", "not supported");
    feature.document_language_support("axioms", "not supported");
    feature.document_property("admissible", "yes");
    feature.document_property("consistent", "yes");
    feature.document_property("safe", "yes");
    feature.document_property("preferred operators", "no");
    feature.add_option<double>(
        "max_potential",
        "Bound potentials by this number. Using the bound {{{infinity}}} "
        "disables the bounds. In some domains this makes the computation of "
        "weights unbounded in which case no weights can be extracted. Using "
        "very high weights can cause numerical instability in the LP solver, "
        "while using very low weights limits the choice of potential "
        "heuristics. For details, see the ICAPS paper cited above.",
        "1e8",
        plugins::Bounds("0.0", "infinity"));
    downward::cli::lp::add_lp_solver_option_to_feature(feature);
    add_heuristic_options_to_feature(feature, description);
}

tuple<
    double,
    downward::lp::LPSolverType,
    shared_ptr<TaskTransformation>,
    bool,
    string,
    utils::Verbosity>
get_admissible_potential_arguments_from_options(const plugins::Options& opts)
{
    return tuple_cat(
        make_tuple(opts.get<double>("max_potential")),
        lp::get_lp_solver_arguments_from_options(opts),
        get_heuristic_arguments_from_options(opts));
}

} // namespace downward::cli::potentials
