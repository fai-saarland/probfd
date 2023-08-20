#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/idual.h"

#include "probfd/evaluator.h"
#include "probfd/progress_report.h"

#include "downward/lp/lp_solver.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace algorithms;

class IDualSolver : public MDPSolver {
    lp::LPSolverType solver_type_;

public:
    explicit IDualSolver(const plugins::Options& opts)
        : MDPSolver(opts)
        , solver_type_(opts.get<lp::LPSolverType>("lpsolver"))
    {
    }

    std::string get_algorithm_name() const override { return "idual"; }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        using IDualAlgorithm = algorithms::idual::IDual<State, OperatorID>;

        return algorithm_factory<IDualAlgorithm>(&progress_, solver_type_);
    }
};

class IDualSolverFeature
    : public plugins::TypedFeature<SolverInterface, IDualSolver> {
public:
    IDualSolverFeature()
        : plugins::TypedFeature<SolverInterface, IDualSolver>("idual")
    {
        document_title("i-dual");

        MDPSolver::add_options_to_feature(*this);

        lp::add_lp_solver_option_to_feature(*this);
    }
};

static plugins::FeaturePlugin<IDualSolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
