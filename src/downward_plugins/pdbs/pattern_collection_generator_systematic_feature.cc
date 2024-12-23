#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/pdbs/pattern_generator_options.h"

#include "downward/pdbs/pattern_collection_generator_systematic.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace utils;
using namespace pdbs;

using namespace downward_plugins::pdbs;
using namespace downward_plugins::plugins;

namespace {

class PatternCollectionGeneratorSystematicFeature
    : public TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorSystematic> {
public:
    PatternCollectionGeneratorSystematicFeature()
        : TypedFeature("systematic")
    {
        document_title("Systematically generated patterns");
        document_synopsis(
            "Generates all (interesting) patterns with up to pattern_max_size "
            "variables. "
            "For details, see" +
            utils::format_conference_reference(
                {"Florian Pommerening", "Gabriele Roeger", "Malte Helmert"},
                "Getting the Most Out of Pattern Databases for Classical "
                "Planning",
                "https://ai.dmi.unibas.ch/papers/"
                "pommerening-et-al-ijcai2013.pdf",
                "Proceedings of the Twenty-Third International Joint"
                " Conference on Artificial Intelligence (IJCAI 2013)",
                "2357-2364",
                "AAAI Press",
                "2013"));

        add_option<int>(
            "pattern_max_size",
            "max number of variables per pattern",
            "1",
            Bounds("1", "infinity"));
        add_option<bool>(
            "only_interesting_patterns",
            "Only consider the union of two disjoint patterns if the union has "
            "more information than the individual patterns.",
            "true");
        add_generator_options_to_feature(*this);
    }

    virtual shared_ptr<PatternCollectionGeneratorSystematic>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<
            PatternCollectionGeneratorSystematic>(
            opts.get<int>("pattern_max_size"),
            opts.get<bool>("only_interesting_patterns"),
            get_generator_arguments_from_options(opts));
    }
};

FeaturePlugin<PatternCollectionGeneratorSystematicFeature> _plugin;

} // namespace
