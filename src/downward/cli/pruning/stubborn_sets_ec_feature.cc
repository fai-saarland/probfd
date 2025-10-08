#include "downward/cli/pruning/stubborn_sets_ec_feature.h"

#include "downward/cli/pruning/pruning_method_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/pruning/stubborn_sets_ec.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;

using namespace downward::cli;
using namespace downward::cli::plugins;

using namespace downward;
using namespace downward::stubborn_sets_ec;

namespace {
class StubbornSetsECFeature : public SharedTypedFeature<PruningMethod> {
public:
    StubbornSetsECFeature()
        : SharedTypedFeature("stubborn_sets_ec")
    {
        document_title("StubbornSetsEC");
        document_synopsis(
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
        add_pruning_options_to_feature(*this);
    }

    virtual shared_ptr<PruningMethod>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<StubbornSetsEC>(
            get_pruning_arguments_from_options(opts));
    }
};
} // namespace

namespace downward::cli::pruning {

void add_stubborn_sets_ec_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<StubbornSetsECFeature>();
}

} // namespace downward::cli::pruning
