#include "probfd/cartesian_abstractions/adaptive_flaw_generator.h"

#include "downward/utils/logging.h"

#include <ostream>
#include <utility>

namespace probfd {
class ProbabilisticTaskProxy;

namespace cartesian_abstractions {
class AbstractState;
class CartesianAbstraction;
class CartesianHeuristic;
} // namespace cartesian_abstractions
} // namespace probfd

namespace utils {
class CountdownTimer;
}

namespace probfd::cartesian_abstractions {

AdaptiveFlawGenerator::AdaptiveFlawGenerator(
    std::vector<std::unique_ptr<FlawGenerator>> generators)
    : generators_(std::move(generators))
{
}

std::optional<Flaw> AdaptiveFlawGenerator::generate_flaw(
    const ProbabilisticTaskProxy& task_proxy,
    const std::vector<int>& domain_sizes,
    CartesianAbstraction& abstraction,
    const AbstractState* init_id,
    CartesianHeuristic& heuristic,
    utils::LogProxy& log,
    utils::CountdownTimer& timer)
{
    while (current_generator_ != generators_.size()) {
        auto& generator = generators_[current_generator_];
        std::optional<Flaw> flaw = generator->generate_flaw(
            task_proxy,
            domain_sizes,
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
    for (size_t i = current_generator_; i != generators_.size(); ++i) {
        generators_[i]->notify_split();
    }
}

void AdaptiveFlawGenerator::print_statistics(utils::LogProxy& log)
{
    for (auto& gen : generators_) {
        gen->print_statistics(log);
    }
}

AdaptiveFlawGeneratorFactory::AdaptiveFlawGeneratorFactory(
    std::vector<std::shared_ptr<FlawGeneratorFactory>> generators)
    : generator_factories_(std::move(generators))
{
}

std::unique_ptr<FlawGenerator>
AdaptiveFlawGeneratorFactory::create_flaw_generator()
{
    std::vector<std::unique_ptr<FlawGenerator>> generators;

    for (const auto& generator_factory : generator_factories_) {
        generators.emplace_back(generator_factory->create_flaw_generator());
    }

    return std::make_unique<AdaptiveFlawGenerator>(std::move(generators));
}

} // namespace probfd::cartesian_abstractions
