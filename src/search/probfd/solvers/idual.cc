#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/idual.h"

#include "probfd/engine_interfaces/evaluator.h"

#include "probfd/progress_report.h"

#include "lp/lp_solver.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;

class IDualSolver : public MDPSolver {
    std::shared_ptr<TaskEvaluator> eval_;
    lp::LPSolverType solver_type_;

public:
    explicit IDualSolver(const options::Options& opts)
        : MDPSolver(opts)
        , eval_(opts.get<std::shared_ptr<TaskEvaluator>>("eval"))
        , solver_type_(opts.get<lp::LPSolverType>("lpsolver"))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<TaskEvaluator>>(
            "eval",
            "",
            "const_eval");
        lp::add_lp_solver_option_to_parser(parser);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string get_engine_name() const override { return "idual"; }

    virtual engines::MDPEngineInterface<State>* create_engine() override
    {
        using IDualEngine = engines::idual::IDual<State, OperatorID>;

        return engine_factory<IDualEngine>(
            eval_.get(),
            &progress_,
            solver_type_);
    }
};

static Plugin<SolverInterface>
    _plugin("idual", options::parse<SolverInterface, IDualSolver>);

} // namespace
} // namespace solvers
} // namespace probfd
