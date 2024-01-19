#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/idual.h"

#include "downward/lp/lp_solver.h"

#include "downward/plugins/plugin.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <memory>
#include <string>

namespace probfd::solvers {
namespace {

using namespace algorithms;
using namespace plugins;

class IDualSolver : public MDPSolver {
    lp::LPSolverType solver_type_;

public:
    explicit IDualSolver(const Options& opts)
        : MDPSolver(opts)
        , solver_type_(opts.get<lp::LPSolverType>("lpsolver"))
    {
    }

    std::string get_algorithm_name() const override { return "idual"; }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        using IDualAlgorithm = algorithms::idual::IDual<State, OperatorID>;

        return std::make_unique<IDualAlgorithm>(solver_type_);
    }
};

class IDualSolverFeature : public TypedFeature<SolverInterface, IDualSolver> {
public:
    IDualSolverFeature()
        : TypedFeature<SolverInterface, IDualSolver>("idual")
    {
        document_title("i-dual");

        MDPSolver::add_options_to_feature(*this);

        lp::add_lp_solver_option_to_feature(*this);
    }
};

} // namespace

static FeaturePlugin<IDualSolverFeature> _plugin;

} // namespace probfd::solvers
