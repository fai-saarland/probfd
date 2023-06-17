#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/engines/exhaustive_ao.h"

#include "probfd/open_lists/task_open_list_factory.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;

template <bool Bisimulation>
class ExhaustiveAOSolver : public MDPHeuristicSearch<Bisimulation, false> {
    template <typename T>
    using WrappedType =
        typename MDPHeuristicSearch<Bisimulation, false>::template WrappedType<
            T>;

    WrappedType<std::shared_ptr<TaskOpenList>> open_list_;

public:
    explicit ExhaustiveAOSolver(const plugins::Options& opts)
        : MDPHeuristicSearch<Bisimulation, false>(opts)
        , open_list_(this->wrap(
              opts.get<std::shared_ptr<TaskOpenListFactory>>("open_list")
                  ->create_open_list(this->state_space_.get())))
    {
    }

    std::string get_heuristic_search_name() const override
    {
        return "exhaustive_ao";
    }

    std::unique_ptr<TaskMDPEngineInterface> create_engine() override
    {
        return this->template create_heuristic_search_engine<
            engines::exhaustive_ao::ExhaustiveAOSearch>(open_list_.get());
    }
};

class ExhaustiveAOSolverFeature
    : public MDPHeuristicSearchSolverFeature<ExhaustiveAOSolver> {
public:
    ExhaustiveAOSolverFeature()
        : MDPHeuristicSearchSolverFeature<ExhaustiveAOSolver>("exhaustive_ao")
    {
        add_option<std::shared_ptr<TaskOpenListFactory>>(
            "open_list",
            "",
            "lifo_open_list_factory");
    }
};

static plugins::FeaturePlugin<ExhaustiveAOSolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
