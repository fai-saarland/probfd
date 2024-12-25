#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/logging_options.h"

#include "downward/operator_counting/state_equation_constraints.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace utils;
using namespace operator_counting;

using namespace downward::cli::plugins;

using downward::cli::utils::add_log_options_to_feature;
using downward::cli::utils::get_log_arguments_from_options;

namespace {

class StateEquationConstraintsFeature
    : public TypedFeature<ConstraintGenerator, StateEquationConstraints> {
public:
    StateEquationConstraintsFeature()
        : TypedFeature("state_equation_constraints")
    {
        document_title("State equation constraints");
        document_synopsis(
            "For each fact, a permanent constraint is added that considers the "
            "net "
            "change of the fact, i.e., the total number of times the fact is "
            "added "
            "minus the total number of times is removed. The bounds of each "
            "constraint depend on the current state and the goal state and are "
            "updated in each state. For details, see" +
            utils::format_conference_reference(
                {"Menkes van den Briel",
                 "J. Benton",
                 "Subbarao Kambhampati",
                 "Thomas Vossen"},
                "An LP-based heuristic for optimal planning",
                "http://link.springer.com/chapter/10.1007/978-3-540-74970-7_46",
                "Proceedings of the Thirteenth International Conference on"
                " Principles and Practice of Constraint Programming (CP 2007)",
                "651-665",
                "Springer-Verlag",
                "2007") +
            utils::format_conference_reference(
                {"Blai Bonet"},
                "An admissible heuristic for SAS+ planning obtained from the"
                " state equation",
                "http://ijcai.org/papers13/Papers/IJCAI13-335.pdf",
                "Proceedings of the Twenty-Third International Joint"
                " Conference on Artificial Intelligence (IJCAI 2013)",
                "2268-2274",
                "AAAI Press",
                "2013") +
            utils::format_conference_reference(
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
                "2014"));

        add_log_options_to_feature(*this);
    }

    virtual shared_ptr<StateEquationConstraints>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<StateEquationConstraints>(
            get_log_arguments_from_options(opts));
    }
};

FeaturePlugin<StateEquationConstraintsFeature> _plugin;

} // namespace
