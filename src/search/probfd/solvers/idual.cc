#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/idual.h"

#include "probfd/engine_interfaces/evaluator.h"

#include "probfd/progress_report.h"

#include "downward/lp/lp_solver.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;

class IDualSolver : public MDPSolver {
    std::shared_ptr<TaskEvaluator> eval_;
    lp::LPSolverType solver_type_;

public:
    explicit IDualSolver(const plugins::Options& opts)
        : MDPSolver(opts)
        , eval_(opts.get<std::shared_ptr<TaskEvaluator>>("eval"))
        , solver_type_(opts.get<lp::LPSolverType>("lpsolver"))
    {
    }

    std::string get_engine_name() const override { return "idual"; }

    std::unique_ptr<TaskMDPEngineInterface> create_engine() override
    {
        using IDualEngine = engines::idual::IDual<State, OperatorID>;

        return engine_factory<IDualEngine>(
            eval_.get(),
            &progress_,
            solver_type_);
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

        add_option<std::shared_ptr<TaskEvaluator>>("eval", "", "blind_eval");
        lp::add_lp_solver_option_to_feature(*this);
    }
};

static plugins::FeaturePlugin<IDualSolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
