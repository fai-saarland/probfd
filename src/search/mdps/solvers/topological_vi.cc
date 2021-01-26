#include "../../option_parser.h"
#include "../../plugin.h"
#include "../engines/topological_value_iteration.h"
#include "../state_evaluator.h"
#include "mdp_solver.h"

namespace probabilistic {

using TVIEngine = topological_vi::
    TopologicalValueIteration<GlobalState, const ProbabilisticOperator*>;

class TopologicalVISolver : public MDPSolver {
public:
    explicit TopologicalVISolver(const options::Options& opts)
        : MDPSolver(opts)
        , prune_(
              opts.contains("eval")
                  ? opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval")
                  : nullptr)
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

    virtual MDPEngineInterface<GlobalState>* create_engine() override
    {
        return engine_factory<TVIEngine>(prune_.get());
    }

private:
    std::shared_ptr<GlobalStateEvaluator> prune_;
};

static Plugin<SolverInterface> _plugin(
    "topological_value_iteration",
    options::parse<SolverInterface, TopologicalVISolver>);

} // namespace probabilistic
