#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/pdbs/cegar_options.h"
#include "downward_plugins/pdbs/pattern_collection_generator_multiple_options.h"
#include "downward_plugins/pdbs/utils.h"

#include "downward/pdbs/pattern_collection_generator_multiple_cegar.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace utils;
using namespace pdbs;

using namespace downward_plugins::pdbs;
using namespace downward_plugins::plugins;

namespace {

class PatternCollectionGeneratorMultipleCegarFeature
    : public TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorMultipleCegar> {
public:
    PatternCollectionGeneratorMultipleCegarFeature()
        : TypedFeature("multiple_cegar")
    {
        document_title("Multiple CEGAR");
        document_synopsis(
            "This pattern collection generator implements the multiple CEGAR "
            "algorithm described in the paper" +
            get_rovner_et_al_reference() +
            "It is an instantiation of the 'multiple algorithm framework'. "
            "To compute a pattern in each iteration, it uses the CEGAR "
            "algorithm "
            "restricted to a single goal variable. See below for descriptions "
            "of "
            "the algorithms.");

        add_cegar_wildcard_option_to_feature(*this);
        add_multiple_options_to_feature(*this);

        add_cegar_implementation_notes_to_feature(*this);
        add_multiple_algorithm_implementation_notes_to_feature(*this);
    }

    virtual shared_ptr<PatternCollectionGeneratorMultipleCegar>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<
            PatternCollectionGeneratorMultipleCegar>(
            get_cegar_wildcard_arguments_from_options(opts),
            get_multiple_arguments_from_options(opts));
    }
};

FeaturePlugin<PatternCollectionGeneratorMultipleCegarFeature> _plugin;

} // namespace
