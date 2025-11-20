#include "downward/cli/operator_counting/pho_constraints_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/operator_counting/pho_constraints.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::pdbs;
using namespace downward::utils;
using namespace downward::operator_counting;

using namespace downward::cli::plugins;

namespace {
class PhOConstraintsFeature
    : public SharedTypedFeature<
          ConstraintGenerator,
          const std::shared_ptr<downward::pdbs::PatternCollectionGenerator>&> {
public:
    PhOConstraintsFeature()
        : TypedFeature("pho_constraints", &PhOConstraintsFeature::func)
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

        make_optional_argument_with_default(
            0,
            "patterns",
            "systematic(2)",
            "pattern generation method");
    }

    static shared_ptr<ConstraintGenerator> func(
        const std::shared_ptr<downward::pdbs::PatternCollectionGenerator>&
            patterns)
    {
        return make_shared_from_arg_tuples<PhOConstraints>(patterns);
    }
};
} // namespace

namespace downward::cli::operator_counting {

void add_pho_constraints_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<PhOConstraintsFeature>();
}

} // namespace downward::cli::operator_counting
