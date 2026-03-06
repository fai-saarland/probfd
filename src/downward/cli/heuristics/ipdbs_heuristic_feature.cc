#include "downward/cli/heuristics/ipdbs_heuristic_feature.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

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

using namespace language::parser;
using namespace downward::cli::pdbs;

using downward::cli::add_heuristic_options_to_feature;

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
    Verbosity verbosity;
    int pdb_max_size;
    int collection_max_size;
    int num_samples;
    int min_improvement;
    FSeconds max_time;
    std::shared_ptr<RandomNumberGenerator> rng;
    FSeconds max_time_dominance_pruning;

public:
    IPDBsHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        Verbosity verbosity,
        int pdb_max_size,
        int collection_max_size,
        int num_samples,
        int min_improvement,
        FSeconds max_time,
        std::shared_ptr<RandomNumberGenerator> rng,
        FSeconds max_time_dominance_pruning)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , pdb_max_size(pdb_max_size)
        , collection_max_size(collection_max_size)
        , num_samples(num_samples)
        , min_improvement(min_improvement)
        , max_time(max_time)
        , rng(std::move(rng))
        , max_time_dominance_pruning(max_time_dominance_pruning)
    {
        if (min_improvement > num_samples) {
            throw std::domain_error(
                "Minimum improvement must not be higher than number "
                "of samples");
        }
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
            rng,
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

} // namespace

namespace downward::cli::heuristics {

InternalFunctionDefinitionBase&
add_ipdbs_heuristic_features(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskDependentFactory<Evaluator>,
        IPDBsHeuristicFactory,
        shared_ptr<TaskTransformation>,
        bool,
        string,
        Verbosity,
        int,
        int,
        int,
        int,
        FSeconds,
        std::shared_ptr<RandomNumberGenerator>,
        FSeconds>>(nspace, "ipdb");

    f.document_title("iPDB");
    f.document_synopsis(
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

    f.document_language_support("action costs", "supported");
    f.document_language_support("conditional effects", "not supported");
    f.document_language_support("axioms", "not supported");

    f.document_property("admissible", "yes");
    f.document_property("consistent", "yes");
    f.document_property("safe", "yes");
    f.document_property("preferred operators", "no");

    const auto n = add_hillclimbing_options_to_feature(f, 0);
    /*
      Add, possibly among others, the options for dominance pruning.
      Note that using dominance pruning during hill climbing could lead to
      fewer discovered patterns and pattern collections. A dominated pattern
      (or pattern collection) might no longer be dominated after more
      patterns are added. We thus only use dominance pruning on the
      resulting collection.
    */
    const auto n2 = add_canonical_pdbs_options_to_feature(f, n);
    add_heuristic_options_to_feature(f, "cpdbs", n + n2);

    return f;
}

} // namespace downward::cli::heuristics
