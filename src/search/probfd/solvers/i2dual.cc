#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/i2dual.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace i2dual {
namespace {

using namespace engine_interfaces;

class I2DualSolver : public MDPSolver {
    std::shared_ptr<TaskEvaluator> heuristic_;
    bool hpom_enabled_;
    bool incremental_hpom_updates_;
    lp::LPSolverType solver_type_;

public:
    explicit I2DualSolver(const plugins::Options& opts)
        : MDPSolver(opts)
        , heuristic_(opts.get<std::shared_ptr<TaskEvaluator>>("eval"))
        , hpom_enabled_(!opts.get<bool>("disable_hpom"))
        , incremental_hpom_updates_(opts.get<bool>("incremental_updates"))
        , solver_type_(opts.get<lp::LPSolverType>("lpsolver"))
    {
    }

    std::string get_engine_name() const override { return "i2dual"; }

    std::unique_ptr<TaskMDPEngineInterface> create_engine() override
    {
        using I2DualEngine = engines::i2dual::I2Dual<State, OperatorID>;

        return engine_factory<I2DualEngine>(
            heuristic_.get(),
            &progress_,
            hpom_enabled_,
            incremental_hpom_updates_,
            solver_type_);
    }
};

class I2DualSolverFeature
    : public plugins::TypedFeature<SolverInterface, I2DualSolver> {
public:
    I2DualSolverFeature()
        : plugins::TypedFeature<SolverInterface, I2DualSolver>("i2dual")
    {
        document_title("i^2-dual");

        MDPSolver::add_options_to_feature(*this);

        add_option<std::shared_ptr<TaskEvaluator>>("eval", "", "blind_eval");
        add_option<bool>("disable_hpom", "", "false");
        add_option<bool>("incremental_updates", "", "true");
        lp::add_lp_solver_option_to_feature(*this);
    }
};

static plugins::FeaturePlugin<I2DualSolverFeature> _plugin;

} // namespace
} // namespace i2dual
} // namespace solvers
} // namespace probfd
