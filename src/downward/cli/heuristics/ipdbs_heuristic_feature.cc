#include "downward/cli/heuristics/ipdbs_heuristic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/cli/pdbs/canonical_pdbs_heuristic_options.h"
#include "downward/cli/pdbs/pattern_collection_generator_hillclimbing_options.h"

#include "downward/pdbs/canonical_pdbs_heuristic.h"

#include "downward/cli/pdbs/pattern_generator_options.h"
#include "downward/cli/utils/rng_options.h"
#include "downward/pdbs/pattern_collection_generator_hillclimbing.h"
#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::plugins;
using namespace downward::cli::pdbs;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

namespace {
std::string paper_references()
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

class IPDBsHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    int pdb_max_size;
    int collection_max_size;
    int num_samples;
    int min_improvement;
    Duration max_time;
    int random_seed;
    Duration max_time_dominance_pruning;

public:
    IPDBsHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        int pdb_max_size,
        int collection_max_size,
        int num_samples,
        int min_improvement,
        Duration max_time,
        int random_seed,
        Duration max_time_dominance_pruning)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , pdb_max_size(pdb_max_size)
        , collection_max_size(collection_max_size)
        , num_samples(num_samples)
        , min_improvement(min_improvement)
        , max_time(max_time)
        , random_seed(random_seed)
        , max_time_dominance_pruning(max_time_dominance_pruning)
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);

        auto pgh = std::make_shared<PatternCollectionGeneratorHillclimbing>(
            pdb_max_size,
            collection_max_size,
            num_samples,
            min_improvement,
            max_time,
            random_seed,
            verbosity);

        return std::make_unique<CanonicalPDBsHeuristic>(
            std::move(pgh),
            max_time_dominance_pruning,
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class IPDBFeature : public SharedTypedFeature<TaskDependentFactory<Evaluator>> {
public:
    IPDBFeature()
        : SharedTypedFeature("ipdb")
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

    shared_ptr<TaskDependentFactory<Evaluator>>
    create_component(const Options& opts, const Context& context) const override
    {
        if (opts.get<int>("min_improvement") > opts.get<int>("num_samples")) {
            context.error(
                "Minimum improvement must not be higher than number "
                "of samples");
        }

        return make_shared_from_arg_tuples<IPDBsHeuristicFactory>(
            get_heuristic_arguments_from_options(opts),
            opts.get<int>("pdb_max_size"),
            opts.get<int>("collection_max_size"),
            opts.get<int>("num_samples"),
            opts.get<int>("min_improvement"),
            opts.get<Duration>("max_time"),
            cli::utils::get_rng_arguments_from_options(opts),
            opts.get<Duration>("max_time_dominance_pruning"));
    }
};
} // namespace

namespace downward::cli::heuristics {

void add_ipdbs_heuristic_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<IPDBFeature>();
}

} // namespace downward::cli::heuristics
