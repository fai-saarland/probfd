#include "probfd/heuristics/pdbs/pattern_collection_generator_classical.h"

#include "probfd/tasks/all_outcomes_determinization.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

PatternCollectionGeneratorClassical::PatternCollectionGeneratorClassical(
    const plugins::Options& opts)
    : PatternCollectionGeneratorClassical(
          utils::get_log_from_options(opts),
          opts.get<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
              "generator"))
{
}

PatternCollectionGeneratorClassical::PatternCollectionGeneratorClassical(
    utils::LogProxy log,
    std::shared_ptr<::pdbs::PatternCollectionGenerator> gen)
    : PatternCollectionGenerator(std::move(log))
    , gen(std::move(gen))
{
}

std::shared_ptr<PatternCollection>
PatternCollectionGeneratorClassical::generate_pattern_collection(
    const std::shared_ptr<ProbabilisticTask>& task,
    const std::shared_ptr<FDRCostFunction>& task_cost_function)
{
    auto determinization =
        std::make_shared<tasks::AODDeterminizationTask>(task.get());

    return gen->generate(determinization).get_patterns();
}

namespace {

class PatternCollectionGeneratorClassicalFeature
    : public plugins::TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorClassical> {
public:
    PatternCollectionGeneratorClassicalFeature()
        : TypedFeature("classical_pattern_generator")
    {
        document_title("Classical Pattern Generation Adapter");
        document_synopsis(
            "Uses a classical pattern generation method on the determinization "
            "of the input task. If classical PDBs are constructed by the "
            "generation algorithm, they are used as a heuristic to compute the "
            "corresponding probability-aware PDBs.");

        add_pattern_collection_generator_options_to_feature(*this);

        add_option<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
            "generator",
            "The classical pattern collection generator.",
            "systematic()");
    }
};

static plugins::FeaturePlugin<PatternCollectionGeneratorClassicalFeature> _;

} // namespace

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
