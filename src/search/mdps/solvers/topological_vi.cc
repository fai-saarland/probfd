#include "../../option_parser.h"
#include "../../plugin.h"
#include "../engines/topological_value_iteration.h"
#include "../state_evaluator.h"
#include "mdp_solver.h"

#include "../globals.h"
#include "../analysis_objective.h"

namespace probabilistic {
namespace solvers {

using TVIEngine = engines::topological_vi::
    TopologicalValueIteration<GlobalState, const ProbabilisticOperator*>;

class TopologicalVISolver : public MDPSolver {
public:
    explicit TopologicalVISolver(const options::Options& opts)
        : MDPSolver(opts)
        , prune_(
              opts.contains("eval")
                  ? opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval")
                  : nullptr)
        , init_value(g_analysis_objective->max())
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "eval", "", options::OptionParser::NONE);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string get_engine_name() const override
    {
        return "topological_value_iteration";
    }

    virtual engines::MDPEngineInterface<GlobalState>* create_engine() override
    {
        return engine_factory<TVIEngine>(
            value_utils::SingleValue(init_value), prune_.get());
    }

private:
    std::shared_ptr<GlobalStateEvaluator> prune_;
    value_type::value_t init_value;
};

static Plugin<SolverInterface> _plugin(
    "topological_value_iteration",
    options::parse<SolverInterface, TopologicalVISolver>);

} // namespace solvers
} // namespace probabilistic
