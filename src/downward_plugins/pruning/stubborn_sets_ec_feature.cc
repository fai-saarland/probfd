#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/pruning_method_options.h"

#include "downward/pruning/stubborn_sets_ec.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;

using namespace downward_plugins;
using namespace downward_plugins::plugins;

namespace stubborn_sets_ec {

class StubbornSetsECFeature
    : public TypedFeature<PruningMethod, StubbornSetsEC> {
public:
    StubbornSetsECFeature()
        : TypedFeature("stubborn_sets_ec")
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

    virtual shared_ptr<StubbornSetsEC>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<StubbornSetsEC>(
            get_pruning_arguments_from_options(opts));
    }
};

FeaturePlugin<StubbornSetsECFeature> _plugin;

} // namespace stubborn_sets_ec
