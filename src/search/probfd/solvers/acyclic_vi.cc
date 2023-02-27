#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/acyclic_value_iteration.h"

#include "probfd/engine_interfaces/evaluator.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;

using AVIEngine = engines::acyclic_vi::AcyclicValueIteration<State, OperatorID>;

class AcyclicVISolver : public MDPSolver {
    std::shared_ptr<TaskEvaluator> prune_;

public:
    explicit AcyclicVISolver(const options::Options& opts)
        : MDPSolver(opts)
        , prune_(
              opts.contains("eval")
                  ? opts.get<std::shared_ptr<TaskEvaluator>>("eval")
                  : nullptr)
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<TaskEvaluator>>(
            "eval",
            "",
            options::OptionParser::NONE);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string get_engine_name() const override
    {
        return "acyclic_value_iteration";
    }

    virtual engines::MDPEngineInterface<State, OperatorID>*
    create_engine() override
    {
        return engine_factory<AVIEngine>(prune_.get());
    }
};

static Plugin<SolverInterface> _plugin(
    "acyclic_value_iteration",
    options::parse<SolverInterface, AcyclicVISolver>);

} // namespace
} // namespace solvers
} // namespace probfd
