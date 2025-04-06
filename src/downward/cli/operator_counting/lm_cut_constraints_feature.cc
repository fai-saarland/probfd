#include "downward/cli/plugins/plugin.h"

#include "downward/operator_counting/lm_cut_constraints.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::operator_counting;

using namespace downward::cli::plugins;

namespace {

class LMCutConstraintsFeature
    : public TypedFeature<ConstraintGenerator, LMCutConstraints> {
public:
    LMCutConstraintsFeature()
        : TypedFeature("lmcut_constraints")
    {
        document_title("LM-cut landmark constraints");
        document_synopsis(
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
                "http://ijcai.org/papers13/Papers/IJCAI13-335.pdf",
                "Proceedings of the Twenty-Third International Joint"
                " Conference on Artificial Intelligence (IJCAI 2013)",
                "2268-2274",
                "AAAI Press",
                "2013"));
    }

    virtual shared_ptr<LMCutConstraints>
    create_component(const Options&, const Context&) const override
    {
        return make_shared<LMCutConstraints>();
    }
};

FeaturePlugin<LMCutConstraintsFeature> _plugin;

} // namespace
