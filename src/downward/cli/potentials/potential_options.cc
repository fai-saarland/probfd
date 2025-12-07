#include "downward/cli/potentials/potential_options.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/cli/lp/lp_solver_options.h"
#include "language/plugins/plugin.h"

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

std::size_t add_admissible_potentials_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    const string& description,
    std::size_t start_index)
{
    feature.document_language_support("action costs", "supported");
    feature.document_language_support("conditional effects", "not supported");
    feature.document_language_support("axioms", "not supported");
    feature.document_property("admissible", "yes");
    feature.document_property("consistent", "yes");
    feature.document_property("safe", "yes");
    feature.document_property("preferred operators", "no");
    feature.make_optional_argument_with_default(
        start_index,
        "max_potential",
        "1e8",
        "Bound potentials by this number. Using the bound {{{infinity}}} "
        "disables the bounds. In some domains this makes the computation of "
        "weights unbounded in which case no weights can be extracted. Using "
        "very high weights can cause numerical instability in the LP solver, "
        "while using very low weights limits the choice of potential "
        "heuristics. For details, see the ICAPS paper cited above.");
    const auto n = downward::cli::lp::add_lp_solver_option_to_feature(
        feature,
        start_index + 1);
    const auto n2 = add_heuristic_options_to_feature(
        feature,
        description,
        start_index + n + 1);
    return n + n2 + 1;
}

} // namespace downward::cli::potentials
