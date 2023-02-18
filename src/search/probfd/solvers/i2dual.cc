#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/i2dual.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {
namespace i2dual {
namespace {

using namespace engine_interfaces;

class I2DualSolver : public MDPSolver {
    std::shared_ptr<TaskStateEvaluator> heuristic_;
    bool hpom_enabled_;
    bool incremental_hpom_updates_;
    lp::LPSolverType solver_type_;

public:
    explicit I2DualSolver(const options::Options& opts)
        : MDPSolver(opts)
        , heuristic_(opts.get<std::shared_ptr<TaskStateEvaluator>>("eval"))
        , hpom_enabled_(!opts.get<bool>("disable_hpom"))
        , incremental_hpom_updates_(opts.get<bool>("incremental_updates"))
        , solver_type_(opts.get<lp::LPSolverType>("lpsolver"))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        MDPSolver::add_options_to_parser(parser);
        parser.add_option<std::shared_ptr<TaskStateEvaluator>>(
            "eval",
            "",
            "const_eval");
        parser.add_option<bool>("disable_hpom", "", "false");
        parser.add_option<bool>("incremental_updates", "", "true");
        lp::add_lp_solver_option_to_parser(parser);
    }

    virtual std::string get_engine_name() const override { return "i2dual"; }

    virtual engines::MDPEngineInterface<State>* create_engine() override
    {
        using I2DualEngine = engines::i2dual::I2Dual<State, OperatorID>;

        return engine_factory<I2DualEngine>(
            &progress_,
            heuristic_.get(),
            hpom_enabled_,
            incremental_hpom_updates_,
            solver_type_);
    }
};

static Plugin<SolverInterface>
    _plugin("i2dual", options::parse<SolverInterface, I2DualSolver>);

} // namespace
} // namespace i2dual
} // namespace solvers
} // namespace probfd
