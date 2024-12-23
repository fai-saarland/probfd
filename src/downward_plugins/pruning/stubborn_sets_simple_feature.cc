#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/pruning_method_options.h"

#include "downward/pruning/stubborn_sets_simple.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace stubborn_sets_simple;

using namespace downward_plugins;
using namespace downward_plugins::plugins;

namespace {

class StubbornSetsSimpleFeature
    : public TypedFeature<PruningMethod, StubbornSetsSimple> {
public:
    StubbornSetsSimpleFeature()
        : TypedFeature("stubborn_sets_simple")
    {
        document_title("Stubborn sets simple");
        document_synopsis(
            "Stubborn sets represent a state pruning method which computes a "
            "subset "
            "of applicable operators in each state such that completeness and "
            "optimality of the overall search is preserved. As stubborn sets "
            "rely "
            "on several design choices, there are different variants thereof. "
            "This stubborn set variant resolves the design choices in a "
            "straight-forward way. For details, see the following papers: " +
            utils::format_conference_reference(
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
            utils::format_conference_reference(
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
        add_pruning_options_to_feature(*this);
    }

    virtual shared_ptr<StubbornSetsSimple>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<StubbornSetsSimple>(
            get_pruning_arguments_from_options(opts));
    }
};

FeaturePlugin<StubbornSetsSimpleFeature> _plugin;

} // namespace
