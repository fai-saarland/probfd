#include "downward/cli/operator_counting/lm_cut_constraints_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/operator_counting/lm_cut_constraints.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::operator_counting;

using namespace downward::cli::plugins;

namespace {

InternalFunctionDefinitionBase& add_delete_relaxation_rr_constraints_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "lmcut_constraints",
        &downward::cli::plugins::
            construct_shared<ConstraintGenerator, LMCutConstraints>);
    f.document_title("LM-cut landmark constraints");
    f.document_synopsis(
        "Computes a set of landmarks in each state using the LM-cut "
        "method. "
        "For each landmark L the constraint sum_{o in L} Count_o >= 1 is "
        "added "
        "to the operator-counting LP temporarily. After the heuristic "
        "value "
        "for the state is computed, all temporary constraints are removed "
        "again. For details, see" +
        format_conference_reference(
            {"Florian Pommerening",
             "Gabriele Roeger",
             "Malte Helmert",
             "Blai Bonet"},
            "LP-based Heuristics for Cost-optimal Planning",
            "http://www.aaai.org/ocs/index.php/ICAPS/ICAPS14/paper/view/"
            "7892/8031",
            "Proceedings of the Twenty-Fourth International Conference"
            " on Automated Planning and Scheduling (ICAPS 2014)",
            "226-234",
            "AAAI Press",
            "2014") +
        format_conference_reference(
            {"Blai Bonet"},
            "An admissible heuristic for SAS+ planning obtained from the"
            " state equation",
            "https://ijcai.org/papers13/Papers/IJCAI13-335.pdf",
            "Proceedings of the Twenty-Third International Joint"
            " Conference on Artificial Intelligence (IJCAI 2013)",
            "2268-2274",
            "AAAI Press",
            "2013"));

    return f;
}

} // namespace

namespace downward::cli::operator_counting {

void add_lm_cut_constraints_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_delete_relaxation_rr_constraints_to_namespace(n);
}

} // namespace downward::cli::operator_counting
