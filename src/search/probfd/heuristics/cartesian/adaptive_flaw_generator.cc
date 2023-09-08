#include "probfd/heuristics/cartesian/adaptive_flaw_generator.h"

#include "probfd/heuristics/cartesian/evaluators.h"

#include "downward/utils/logging.h"

#include "downward/plugins/plugin.h"

#include <algorithm>

namespace probfd {
namespace heuristics {
namespace cartesian {

AdaptiveFlawGenerator::AdaptiveFlawGenerator(
    std::vector<std::unique_ptr<FlawGenerator>> generators)
    : generators(std::move(generators))
{
}

std::optional<Flaw> AdaptiveFlawGenerator::generate_flaw(
    ProbabilisticTaskProxy task_proxy,
    Abstraction& abstraction,
    const AbstractState* init_id,
    CartesianHeuristic& heuristic,
    utils::LogProxy log,
    utils::CountdownTimer& timer)
{
    while (current_generator != generators.size()) {
        auto& generator = generators[current_generator];
        std::optional<Flaw> flaw = generator->generate_flaw(
            task_proxy,
            abstraction,
            init_id,
            heuristic,
            log,
            timer);

        if (flaw) return flaw;

        log << "Switching to the next flaw generator." << std::endl;
    }

    return std::nullopt;
}

void AdaptiveFlawGenerator::notify_split()
{
    for (size_t i = current_generator; i != generators.size(); ++i) {
        generators[i]->notify_split();
    }
}

void AdaptiveFlawGenerator::print_statistics(utils::LogProxy log)
{
    for (auto& gen : generators) {
        gen->print_statistics(log);
    }
}

AdaptiveFlawGeneratorFactory::AdaptiveFlawGeneratorFactory(
    const plugins::Options& opts)
    : generator_factories(
          opts.get_list<std::shared_ptr<FlawGeneratorFactory>>("generators"))
{
}

std::unique_ptr<FlawGenerator>
AdaptiveFlawGeneratorFactory::create_flaw_generator()
{
    std::vector<std::unique_ptr<FlawGenerator>> generators;

    for (const auto& generator_factory : generator_factories) {
        generators.emplace_back(generator_factory->create_flaw_generator());
    }

    return std::make_unique<AdaptiveFlawGenerator>(std::move(generators));
}

class AdaptiveFlawGeneratorFactoryFeature
    : public plugins::
          TypedFeature<FlawGeneratorFactory, AdaptiveFlawGeneratorFactory> {
public:
    AdaptiveFlawGeneratorFactoryFeature()
        : TypedFeature("flaws_adaptive")
    {
        add_list_option<std::shared_ptr<FlawGeneratorFactory>>(
            "generators",
            "The linear hierachy of flaw generators.",
            "[flaws_astar(), flaws_ilao()]");
    }
};

static plugins::FeaturePlugin<AdaptiveFlawGeneratorFactoryFeature> _plugin;

} // namespace cartesian
} // namespace heuristics
} // namespace probfd
