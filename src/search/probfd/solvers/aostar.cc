#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/ao_star.h"

#include "probfd/algorithms/successor_sampler.h"

#include "probfd/plugins/naming_conventions.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace algorithms;
using namespace algorithms::ao_search::ao_star;

using namespace plugins;

template <bool Bisimulation>
using Sampler = std::conditional_t<
    Bisimulation,
    SuccessorSampler<bisimulation::QuotientAction>,
    SuccessorSampler<OperatorID>>;

template <bool Bisimulation>
class AOStarSolver : public MDPHeuristicSearch<Bisimulation, false> {
    const std::shared_ptr<Sampler<Bisimulation>> successor_sampler_;

public:
    explicit AOStarSolver(const Options& opts)
        : MDPHeuristicSearch<Bisimulation, false>(opts)
        , successor_sampler_(opts.get<std::shared_ptr<Sampler<Bisimulation>>>(
              "successor_sampler"))
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
        successor_sampler_->print_statistics(std::cout);
        MDPHeuristicSearch<Bisimulation, false>::print_additional_statistics();
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
        MDPHeuristicSearchBase::add_options_to_feature(*this);

        this->template add_option<std::shared_ptr<Sampler<Bisimulation>>>(
            "successor_sampler",
            "",
            add_mdp_type_to_option<Bisimulation, false>(
                "arbitrary_successor_sampler"));
    }
};

static FeaturePlugin<AOStarSolverFeature<false>> _plugin;
static FeaturePlugin<AOStarSolverFeature<true>> _plugin2;

} // namespace
} // namespace solvers
} // namespace probfd
