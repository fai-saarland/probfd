#include "downward/cli/operator_counting/pho_constraints_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/operator_counting/pho_constraints.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::pdbs;
using namespace downward::utils;
using namespace downward::operator_counting;

using namespace language::plugins;

namespace downward::cli::operator_counting {

InternalFunctionDefinitionBase& add_pho_constraints_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pho_constraints",
        &language::plugins::construct_shared<
            ConstraintGenerator,
            PhOConstraints,
            std::shared_ptr<PatternCollectionGenerator>>);
    f.document_title("Posthoc optimization constraints");
    f.document_synopsis(
        "The generator will compute a PDB for each pattern and add the"
        " constraint h(s) <= sum_{o in relevant(h)} Count_o. For details,"
        " see" +
        format_conference_reference(
            {"Florian Pommerening", "Gabriele Roeger", "Malte Helmert"},
            "Getting the Most Out of Pattern Databases for Classical "
            "Planning",
            "https://ijcai.org/papers13/Papers/IJCAI13-347.pdf",
            "Proceedings of the Twenty-Third International Joint"
            " Conference on Artificial Intelligence (IJCAI 2013)",
            "2357-2364",
            "AAAI Press",
            "2013"));

    f.make_optional_argument_with_default(
        0,
        "patterns",
        "systematic(2)",
        "pattern generation method");

    return f;
}

} // namespace downward::cli::operator_counting
