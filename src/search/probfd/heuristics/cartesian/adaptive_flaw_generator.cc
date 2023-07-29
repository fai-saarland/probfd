#include "probfd/heuristics/cartesian/adaptive_flaw_generator.h"

#include "probfd/heuristics/cartesian/engine_interfaces.h"

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
    const ProbabilisticTaskProxy& task_proxy,
    const std::vector<int>& domain_sizes,
    Abstraction& abstraction,
    CartesianCostFunction& cost_function,
    const AbstractState* init_id,
    utils::LogProxy& log,
    utils::CountdownTimer& timer)
{
    while (current_generator != generators.size()) {
        auto& generator = generators[current_generator];
        std::optional<Flaw> flaw = generator->generate_flaw(
            task_proxy,
            domain_sizes,
            abstraction,
            cost_function,
            init_id,
            log,
            timer);

        if (flaw) return flaw;

        log << "Switching to the next flaw generator." << std::endl;

        // Copy the heuristic to the next flaw generator
        CartesianHeuristic& heuristic_prev = generator->get_heuristic();
        CartesianHeuristic& heuristic_next =
            generators[++current_generator]->get_heuristic();

        for (int v = 0; v != abstraction.get_num_states(); ++v) {
            heuristic_next.set_h_value(v, heuristic_prev.get_h_value(v));
        }
    }

    return std::nullopt;
}

void AdaptiveFlawGenerator::notify_split(int v)
{
    for (size_t i = current_generator; i != generators.size(); ++i) {
        generators[i]->notify_split(v);
    }
}

CartesianHeuristic& AdaptiveFlawGenerator::get_heuristic()
{
    return generators[current_generator]->get_heuristic();
}

void AdaptiveFlawGenerator::print_statistics(utils::LogProxy& log)
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
AdaptiveFlawGeneratorFactory::create_flaw_generator() const
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
