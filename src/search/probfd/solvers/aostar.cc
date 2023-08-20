#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/engines/ao_star.h"

#include "probfd/engines/successor_sampler.h"
#include "probfd/successor_samplers/task_successor_sampler_factory.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engines;

template <bool Bisimulation>
class AOStarSolver : public MDPHeuristicSearch<Bisimulation, false> {
    template <typename T>
    using WrappedType =
        typename MDPHeuristicSearch<Bisimulation, false>::template WrappedType<
            T>;

    WrappedType<std::shared_ptr<FDRSuccessorSampler>> successor_sampler_;

public:
    explicit AOStarSolver(const plugins::Options& opts)
        : MDPHeuristicSearch<Bisimulation, false>(opts)
        , successor_sampler_(this->template wrap<>(
              opts.get<std::shared_ptr<FDRSuccessorSamplerFactory>>(
                      "successor_sampler")
                  ->create_sampler(this->task_mdp.get())))
    {
    }

    std::string get_heuristic_search_name() const override { return "aostar"; }

    std::unique_ptr<FDRMDPEngine> create_engine() override
    {
        return this->template create_heuristic_search_engine<
            engines::ao_search::ao_star::AOStar>(successor_sampler_);
    }

protected:
    void print_additional_statistics() const override
    {
        successor_sampler_->print_statistics(std::cout);
        MDPHeuristicSearch<Bisimulation, false>::print_additional_statistics();
    }
};

class AOStarSolverFeature
    : public MDPHeuristicSearchSolverFeature<AOStarSolver> {
public:
    AOStarSolverFeature()
        : MDPHeuristicSearchSolverFeature<AOStarSolver>("aostar")
    {
        add_option<std::shared_ptr<FDRSuccessorSamplerFactory>>(
            "successor_sampler",
            "",
            "arbitrary_successor_selector_factory");
    }
};

static plugins::FeaturePlugin<AOStarSolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
