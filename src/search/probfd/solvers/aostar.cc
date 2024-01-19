#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/ao_star.h"

#include "probfd/plugins/naming_conventions.h"

#include "downward/plugins/plugin.h"

#include <iostream>
#include <memory>
#include <string>

namespace probfd::solvers {
namespace {

using namespace algorithms;
using namespace algorithms::ao_search::ao_star;

using namespace plugins;

template <bool Bisimulation>
class AOStarSolver : public MDPHeuristicSearch<Bisimulation, false> {
    using Sampler = WrappedType<SuccessorSampler, Bisimulation, false>;

    const std::shared_ptr<Sampler> successor_sampler_;

public:
    explicit AOStarSolver(const Options& opts)
        : MDPHeuristicSearch<Bisimulation, false>(opts)
        , successor_sampler_(
              opts.get<std::shared_ptr<Sampler>>("successor_sampler"))
    {
    }

    [[nodiscard]] std::string get_heuristic_search_name() const override
    {
        return "aostar";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return this->template create_heuristic_search_algorithm<AOStar>(
            successor_sampler_);
    }

protected:
    void print_additional_statistics() const override
    {
        MDPHeuristicSearch<Bisimulation, false>::print_additional_statistics();
        successor_sampler_->print_statistics(std::cout);
    }
};

template <bool Bisimulation>
class AOStarSolverFeature
    : public TypedFeature<SolverInterface, AOStarSolver<Bisimulation>> {
public:
    AOStarSolverFeature()
        : TypedFeature<SolverInterface, AOStarSolver<Bisimulation>>(
              add_wrapper_algo_suffix<Bisimulation, false>("aostar"))
    {
        MDPHeuristicSearch<Bisimulation, false>::add_options_to_feature(*this);

        this->template add_option<std::shared_ptr<
            WrappedType<SuccessorSampler, Bisimulation, false>>>(
            "successor_sampler",
            "",
            add_mdp_type_to_option<Bisimulation, false>(
                "arbitrary_successor_sampler"));
    }
};

} // namespace

static FeaturePlugin<AOStarSolverFeature<false>> _plugin;
static FeaturePlugin<AOStarSolverFeature<true>> _plugin2;

} // namespace probfd::solvers
