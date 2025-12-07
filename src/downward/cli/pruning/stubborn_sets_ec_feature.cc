#include "downward/cli/pruning/stubborn_sets_ec_feature.h"

#include "downward/cli/pruning/pruning_method_options.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/pruning/stubborn_sets_ec.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;

using namespace downward::cli;
using namespace downward::cli::plugins;

using namespace downward;
using namespace downward::stubborn_sets_ec;

namespace {

InternalFunctionDefinitionBase& add_stubborn_sets_ec_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "stubborn_sets_ec",
        &downward::cli::plugins::construct_shared<
            PruningMethod,
            StubbornSetsEC,
            utils::Verbosity>);
    f.document_title("StubbornSetsEC");
    f.document_synopsis(
        "Stubborn sets represent a state pruning method which computes a "
        "subset "
        "of applicable operators in each state such that completeness and "
        "optimality of the overall search is preserved. As stubborn sets "
        "rely "
        "on several design choices, there are different variants thereof. "
        "The variant 'StubbornSetsEC' resolves the design choices such "
        "that "
        "the resulting pruning method is guaranteed to strictly dominate "
        "the "
        "Expansion Core pruning method. For details, see" +
        utils::format_conference_reference(
            {"Martin Wehrle",
             "Malte Helmert",
             "Yusra Alkhazraji",
             "Robert Mattmueller"},
            "The Relative Pruning Power of Strong Stubborn Sets and "
            "Expansion Core",
            "http://www.aaai.org/ocs/index.php/ICAPS/ICAPS13/paper/view/"
            "6053/6185",
            "Proceedings of the 23rd International Conference on Automated "
            "Planning "
            "and Scheduling (ICAPS 2013)",
            "251-259",
            "AAAI Press",
            "2013"));
    add_pruning_options_to_feature(f, 0);

    return f;
}

} // namespace

namespace downward::cli::pruning {

void add_stubborn_sets_ec_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_stubborn_sets_ec_to_namespace(n);
}

} // namespace downward::cli::pruning
