#include "downward/cli/plugins/plugin.h"

#include "downward/cli/pdbs/pattern_collection_generator_hillclimbing_options.h"

#include "downward/cli/pdbs/canonical_pdbs_heuristic_options.h"
#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/cli/heuristic_options.h"

#include "downward/pdbs/canonical_pdbs_heuristic.h"
#include "downward/pdbs/pattern_collection_generator_hillclimbing.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace utils;
using namespace pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

using downward::cli::pdbs::add_generator_options_to_feature;
using downward::cli::pdbs::get_generator_arguments_from_options;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

namespace {

static basic_string<char> paper_references()
{
    return format_conference_reference(
               {"Patrik Haslum",
                "Adi Botea",
                "Malte Helmert",
                "Blai Bonet",
                "Sven Koenig"},
               "Domain-Independent Construction of Pattern Database Heuristics "
               "for"
               " Cost-Optimal Planning",
               "https://ai.dmi.unibas.ch/papers/haslum-et-al-aaai07.pdf",
               "Proceedings of the 22nd AAAI Conference on Artificial"
               " Intelligence (AAAI 2007)",
               "1007-1012",
               "AAAI Press",
               "2007") +
           "For implementation notes, see:" +
           format_conference_reference(
               {"Silvan Sievers", "Manuela Ortlieb", "Malte Helmert"},
               "Efficient Implementation of Pattern Database Heuristics for"
               " Classical Planning",
               "https://ai.dmi.unibas.ch/papers/sievers-et-al-socs2012.pdf",
               "Proceedings of the Fifth Annual Symposium on Combinatorial"
               " Search (SoCS 2012)",
               "105-111",
               "AAAI Press",
               "2012");
}

class PatternCollectionGeneratorHillclimbingFeature
    : public TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorHillclimbing> {
public:
    PatternCollectionGeneratorHillclimbingFeature()
        : TypedFeature("hillclimbing")
    {
        document_title("Hill climbing");
        document_synopsis(
            "This algorithm uses hill climbing to generate patterns "
            "optimized for the Heuristic#Canonical_PDB heuristic. It it "
            "described "
            "in the following paper:" +
            paper_references());
        add_hillclimbing_options_to_feature(*this);
        add_generator_options_to_feature(*this);
    }

    virtual shared_ptr<PatternCollectionGeneratorHillclimbing>
    create_component(const Options& opts, const Context& context) const override
    {
        if (opts.get<int>("min_improvement") > opts.get<int>("num_samples")) {
            context.error("Minimum improvement must not be higher than number "
                          "of samples");
        }

        return make_shared_from_arg_tuples<
            PatternCollectionGeneratorHillclimbing>(
            get_hillclimbing_arguments_from_options(opts),
            get_generator_arguments_from_options(opts));
    }
};

class IPDBFeature : public TypedFeature<Evaluator, CanonicalPDBsHeuristic> {
public:
    IPDBFeature()
        : TypedFeature("ipdb")
    {
        document_subcategory("heuristics_pdb");
        document_title("iPDB");
        document_synopsis(
            "This approach is a combination of using the "
            "Heuristic#Canonical_PDB "
            "heuristic over patterns computed with the "
            "PatternCollectionGenerator#hillclimbing algorithm for pattern "
            "generation. It is a short-hand for the command-line option "
            "{{{cpdbs(hillclimbing())}}}. "
            "Both the heuristic and the pattern generation algorithm are "
            "described "
            "in the following paper:" +
            paper_references() +
            "See also Heuristic#Canonical_PDB and "
            "PatternCollectionGenerator#Hill_climbing for more details.");

        add_hillclimbing_options_to_feature(*this);
        /*
          Add, possibly among others, the options for dominance pruning.
          Note that using dominance pruning during hill climbing could lead to
          fewer discovered patterns and pattern collections. A dominated pattern
          (or pattern collection) might no longer be dominated after more
          patterns are added. We thus only use dominance pruning on the
          resulting collection.
        */
        add_canonical_pdbs_options_to_feature(*this);
        add_heuristic_options_to_feature(*this, "cpdbs");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<CanonicalPDBsHeuristic>
    create_component(const Options& opts, const Context& context) const override
    {
        if (opts.get<int>("min_improvement") > opts.get<int>("num_samples")) {
            context.error("Minimum improvement must not be higher than number "
                          "of samples");
        }

        shared_ptr<PatternCollectionGeneratorHillclimbing> pgh =
            make_shared_from_arg_tuples<PatternCollectionGeneratorHillclimbing>(
                get_hillclimbing_arguments_from_options(opts),
                get_generator_arguments_from_options(opts));

        return make_shared_from_arg_tuples<CanonicalPDBsHeuristic>(
            pgh,
            opts.get<double>("max_time_dominance_pruning"),
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<PatternCollectionGeneratorHillclimbingFeature> _plugin;
FeaturePlugin<IPDBFeature> _plugin_ipdb;

} // namespace
