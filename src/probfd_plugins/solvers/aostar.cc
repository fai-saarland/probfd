#include "probfd_plugins/multi_feature_plugin.h"
#include "probfd_plugins/naming_conventions.h"

#include "probfd_plugins/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/ao_star.h"

#include "downward/plugins/plugin.h"

#include <iostream>
#include <memory>
#include <string>

using namespace plugins;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::ao_search::ao_star;

using namespace probfd_plugins;
using namespace probfd_plugins::solvers;

namespace {

template <bool Bisimulation>
class AOStarSolver : public MDPHeuristicSearch<Bisimulation, false> {
    using Sampler = SuccessorSampler<ActionType<Bisimulation, false>>;

    const std::shared_ptr<Sampler> successor_sampler_;

public:
    template <typename... Args>
    explicit AOStarSolver(std::shared_ptr<Sampler> sampler, Args&&... args)
        : MDPHeuristicSearch<Bisimulation, false>(std::forward<Args>(args)...)
        , successor_sampler_(std::move(sampler))
    {
    }

    std::string get_heuristic_search_name() const override { return "aostar"; }

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
    using Sampler = SuccessorSampler<ActionType<Bisimulation, false>>;

public:
    AOStarSolverFeature()
        : TypedFeature<SolverInterface, AOStarSolver<Bisimulation>>(
              add_wrapper_algo_suffix<Bisimulation, false>("aostar"))
    {
        add_mdp_hs_options_to_feature<Bisimulation, false>(*this);

        this->template add_option<std::shared_ptr<Sampler>>(
            "successor_sampler",
            "",
            add_mdp_type_to_option<Bisimulation, false>(
                "arbitrary_successor_sampler()"));
    }

protected:
    std::shared_ptr<AOStarSolver<Bisimulation>>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<AOStarSolver<Bisimulation>>(
            options.get<std::shared_ptr<Sampler>>("successor_sampler"),
            get_mdp_hs_args_from_options<Bisimulation, false>(options));
    }
};

BinaryFeaturePlugin<AOStarSolverFeature> _plugin;

} // namespace
