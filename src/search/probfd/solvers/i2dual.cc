#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/i2dual.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {
namespace i2dual {

using namespace engine_interfaces;

using I2DualEngine =
    engines::i2dual::I2Dual<GlobalState, const ProbabilisticOperator*>;

class I2DualSolver : public MDPSolver {
public:
    explicit I2DualSolver(const options::Options& opts)
        : MDPSolver(opts)
        , heuristic_(opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval"))
        , hpom_enabled_(!opts.get<bool>("disable_hpom"))
        , incremental_hpom_updates_(opts.get<bool>("incremental_updates"))
        , solver_type_(opts.get<lp::LPSolverType>("lpsolver"))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        MDPSolver::add_options_to_parser(parser);
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "eval",
            "",
            "const");
        parser.add_option<bool>("disable_hpom", "", "false");
        parser.add_option<bool>("incremental_updates", "", "true");
        lp::add_lp_solver_option_to_parser(parser);
    }

    virtual std::string get_engine_name() const override { return "i2dual"; }

    virtual engines::MDPEngineInterface<GlobalState>* create_engine() override
    {
        return engine_factory<I2DualEngine>(
            &progress_,
            heuristic_.get(),
            hpom_enabled_,
            incremental_hpom_updates_,
            solver_type_);
    }

private:
    std::shared_ptr<GlobalStateEvaluator> heuristic_;
    bool hpom_enabled_;
    bool incremental_hpom_updates_;
    lp::LPSolverType solver_type_;
};

static Plugin<SolverInterface>
    _plugin("i2dual", options::parse<SolverInterface, I2DualSolver>);

} // namespace i2dual
} // namespace solvers
} // namespace probfd
