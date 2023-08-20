#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/idual.h"

#include "probfd/evaluator.h"
#include "probfd/progress_report.h"

#include "downward/lp/lp_solver.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engines;

class IDualSolver : public MDPSolver {
    lp::LPSolverType solver_type_;

public:
    explicit IDualSolver(const plugins::Options& opts)
        : MDPSolver(opts)
        , solver_type_(opts.get<lp::LPSolverType>("lpsolver"))
    {
    }

    std::string get_engine_name() const override { return "idual"; }

    std::unique_ptr<FDRMDPEngine> create_engine() override
    {
        using IDualEngine = engines::idual::IDual<State, OperatorID>;

        return engine_factory<IDualEngine>(&progress_, solver_type_);
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
