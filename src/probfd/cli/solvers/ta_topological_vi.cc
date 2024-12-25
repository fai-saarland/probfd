#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include <memory>
#include <string>

using namespace probfd;
using namespace probfd::solvers;

using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

namespace {

class TATopologicalVISolver : public MDPSolver {
public:
    using MDPSolver::MDPSolver;

    std::string get_algorithm_name() const override
    {
        return "ta_topological_value_iteration";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        using TVIAlgorithm = algorithms::ta_topological_vi::
            TATopologicalValueIteration<State, OperatorID>;
        return std::make_unique<TVIAlgorithm>();
    }
};

class TATopologicalVISolverFeature
    : public TypedFeature<SolverInterface, TATopologicalVISolver> {
public:
    TATopologicalVISolverFeature()
        : TypedFeature<SolverInterface, TATopologicalVISolver>(
              "ta_topological_value_iteration")
    {
        document_title("Trap-Aware Topological Value Iteration.");
        add_base_solver_options_to_feature(*this);
    }

protected:
    std::shared_ptr<TATopologicalVISolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<TATopologicalVISolver>(
            get_base_solver_args_from_options(options));
    }
};

FeaturePlugin<TATopologicalVISolverFeature> _plugin;

} // namespace
