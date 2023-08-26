#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/exhaustive_ao.h"
#include "probfd/algorithms/open_list.h"

#include "probfd/plugins/multi_feature_plugin.h"
#include "probfd/plugins/naming_conventions.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace algorithms;
using namespace plugins;

template <bool Bisimulation>
using OpenList = std::conditional_t<
    Bisimulation,
    algorithms::OpenList<bisimulation::QuotientAction>,
    algorithms::OpenList<OperatorID>>;

template <bool Bisimulation>
class ExhaustiveAOSolver : public MDPHeuristicSearch<Bisimulation, false> {
    std::shared_ptr<OpenList<Bisimulation>> open_list_;

public:
    explicit ExhaustiveAOSolver(const Options& opts)
        : MDPHeuristicSearch<Bisimulation, false>(opts)
        , open_list_(
              opts.get<std::shared_ptr<OpenList<Bisimulation>>>("open_list"))
    {
    }

    std::string get_heuristic_search_name() const override
    {
        return "exhaustive_ao";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return this->template create_heuristic_search_algorithm<
            algorithms::exhaustive_ao::ExhaustiveAOSearch>(open_list_);
    }
};

template <bool Bisimulation>
class ExhaustiveAOSolverFeature
    : public MDPHeuristicSearchSolverFeature<ExhaustiveAOSolver, Bisimulation> {
public:
    ExhaustiveAOSolverFeature()
        : MDPHeuristicSearchSolverFeature<ExhaustiveAOSolver, Bisimulation>(
              add_wrapper_algo_suffix<Bisimulation, false>("exhaustive_ao"))
    {
        this->template add_option<std::shared_ptr<OpenList<Bisimulation>>>(
            "open_list",
            "",
            add_mdp_type_to_option<Bisimulation, false>("lifo_open_list()"));
    }
};

static BinaryFeaturePlugin<ExhaustiveAOSolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
