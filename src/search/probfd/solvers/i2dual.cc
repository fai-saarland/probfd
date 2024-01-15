#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/i2dual.h"

#include "downward/plugins/plugin.h"

#include <memory>
#include <string>

namespace probfd {
namespace solvers {
namespace i2dual {
namespace {

using namespace algorithms;
using namespace plugins;

class I2DualSolver : public MDPSolver {
    bool hpom_enabled_;
    bool incremental_hpom_updates_;
    lp::LPSolverType solver_type_;

public:
    explicit I2DualSolver(const Options& opts)
        : MDPSolver(opts)
        , hpom_enabled_(!opts.get<bool>("disable_hpom"))
        , incremental_hpom_updates_(opts.get<bool>("incremental_updates"))
        , solver_type_(opts.get<lp::LPSolverType>("lpsolver"))
    {
    }

    std::string get_algorithm_name() const override { return "i2dual"; }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return std::make_unique<algorithms::i2dual::I2Dual>(
            this->task,
            this->task_cost_function,
            hpom_enabled_,
            incremental_hpom_updates_,
            solver_type_);
    }
};

class I2DualSolverFeature
    : public TypedFeature<SolverInterface, I2DualSolver> {
public:
    I2DualSolverFeature()
        : TypedFeature<SolverInterface, I2DualSolver>("i2dual")
    {
        document_title("i^2-dual");

        MDPSolver::add_options_to_feature(*this);

        add_option<bool>("disable_hpom", "", "false");
        add_option<bool>("incremental_updates", "", "true");

        lp::add_lp_solver_option_to_feature(*this);
    }
};

static FeaturePlugin<I2DualSolverFeature> _plugin;

} // namespace
} // namespace i2dual
} // namespace solvers
} // namespace probfd
