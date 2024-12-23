#include "downward_plugins/plugins/plugin.h"

#include "downward/operator_counting/pho_constraints.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace utils;
using namespace operator_counting;

using namespace downward_plugins::plugins;

namespace {

class PhOConstraintsFeature
    : public TypedFeature<ConstraintGenerator, PhOConstraints> {
public:
    PhOConstraintsFeature()
        : TypedFeature("pho_constraints")
    {
        document_title("Posthoc optimization constraints");
        document_synopsis(
            "The generator will compute a PDB for each pattern and add the"
            " constraint h(s) <= sum_{o in relevant(h)} Count_o. For details,"
            " see" +
            utils::format_conference_reference(
                {"Florian Pommerening", "Gabriele Roeger", "Malte Helmert"},
                "Getting the Most Out of Pattern Databases for Classical "
                "Planning",
                "http://ijcai.org/papers13/Papers/IJCAI13-347.pdf",
                "Proceedings of the Twenty-Third International Joint"
                " Conference on Artificial Intelligence (IJCAI 2013)",
                "2357-2364",
                "AAAI Press",
                "2013"));

        add_option<shared_ptr<pdbs::PatternCollectionGenerator>>(
            "patterns",
            "pattern generation method",
            "systematic(2)");
    }

    virtual shared_ptr<PhOConstraints>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PhOConstraints>(
            opts.get<shared_ptr<pdbs::PatternCollectionGenerator>>("patterns"));
    }
};

FeaturePlugin<PhOConstraintsFeature> _plugin;

} // namespace
