#include "downward/cli/pruning/stubborn_sets_simple_feature.h"

#include "downward/cli/pruning/pruning_method_options.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/pruning/stubborn_sets_simple.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::stubborn_sets_simple;

using namespace downward::cli;
using namespace language::plugins;

namespace {

} // namespace

namespace downward::cli::pruning {

InternalFunctionDefinitionBase&
add_stubborn_sets_simple_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "stubborn_sets_simple",
        &language::plugins::construct_shared<
            downward::PruningMethod,
            StubbornSetsSimple,
            Verbosity>);
    f.document_title("Stubborn sets simple");
    f.document_synopsis(
        "Stubborn sets represent a state pruning method which computes a "
        "subset "
        "of applicable operators in each state such that completeness and "
        "optimality of the overall search is preserved. As stubborn sets "
        "rely "
        "on several design choices, there are different variants thereof. "
        "This stubborn set variant resolves the design choices in a "
        "straight-forward way. For details, see the following papers: " +
        format_conference_reference(
            {"Yusra Alkhazraji",
             "Martin Wehrle",
             "Robert Mattmueller",
             "Malte Helmert"},
            "A Stubborn Set Algorithm for Optimal Planning",
            "https://ai.dmi.unibas.ch/papers/alkhazraji-et-al-ecai2012.pdf",
            "Proceedings of the 20th European Conference on Artificial "
            "Intelligence "
            "(ECAI 2012)",
            "891-892",
            "IOS Press",
            "2012") +
        format_conference_reference(
            {"Martin Wehrle", "Malte Helmert"},
            "Efficient Stubborn Sets: Generalized Algorithms and Selection "
            "Strategies",
            "http://www.aaai.org/ocs/index.php/ICAPS/ICAPS14/paper/view/"
            "7922/8042",
            "Proceedings of the 24th International Conference on Automated "
            "Planning "
            " and Scheduling (ICAPS 2014)",
            "323-331",
            "AAAI Press",
            "2014"));
    add_pruning_options_to_feature(f, 0);

    return f;
}

} // namespace downward::cli::pruning
