#include "../engines/idual.h"

#include "../../lp/lp_solver.h"
#include "../../option_parser.h"
#include "../../plugin.h"
#include "../progress_report.h"
#include "../state_evaluator.h"
#include "mdp_solver.h"

namespace probabilistic {

using IDualEngine = idual::IDual<GlobalState, const ProbabilisticOperator*>;

class IDualSolver : public MDPSolver {
public:
    explicit IDualSolver(const options::Options& opts)
        : MDPSolver(opts)
        , eval_(opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval"))
        , solver_type_(lp::LPSolverType(opts.get_enum("lpsolver")))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "eval", "", "const");
        lp::add_lp_solver_option_to_parser(parser);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string get_engine_name() const override { return "idual"; }

    virtual MDPEngineInterface<GlobalState>* create_engine() override
    {
        return engine_factory<IDualEngine>(
            solver_type_, eval_.get(), &progress_);
    }

private:
    std::shared_ptr<GlobalStateEvaluator> eval_;
    lp::LPSolverType solver_type_;
};

static Plugin<SolverInterface>
    _plugin("idual", options::parse<SolverInterface, IDualSolver>);

} // namespace probabilistic
