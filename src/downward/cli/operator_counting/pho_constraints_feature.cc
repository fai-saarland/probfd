#include "downward/cli/operator_counting/pho_constraints_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/operator_counting/pho_constraints.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::pdbs;
using namespace downward::utils;
using namespace downward::operator_counting;

using namespace downward::cli::plugins;

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

        add_option<shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "pattern generation method",
            "systematic(2)");
    }

    virtual shared_ptr<PhOConstraints>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PhOConstraints>(
            opts.get<shared_ptr<PatternCollectionGenerator>>("patterns"));
    }
};
}

namespace downward::cli::operator_counting {

void add_pho_constraints_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<PhOConstraintsFeature>();
}

} // namespace
