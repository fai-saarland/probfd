#include "probfd/occupation_measures/pho_constraints.h"

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/task_utils/task_properties.h"

#include "downward/utils/collections.h"

#include "downward/lp/lp_solver.h"

#include <iterator>
#include <set>
#include <utility>
#include <vector>

using namespace probfd::pdbs;
using namespace downward;

namespace probfd::occupation_measures {

using namespace pdbs;

PHOGenerator::PHOGenerator(pdbs::PPDBCollection pdbs)
    : pdbs_(std::move(pdbs))
{
}

void PHOGenerator::initialize_constraints(
    const std::shared_ptr<ProbabilisticTask>& task,
    const std::shared_ptr<FDRCostFunction>&,
    lp::LinearProgram& lp)
{
    ProbabilisticTaskProxy task_proxy(*task);
    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();

    const double lp_infinity = lp.get_infinity();

    auto& lp_variables = lp.get_variables();
    auto& lp_constraints = lp.get_constraints();

    std::vector<std::set<int>> affected_vars;
    affected_vars.reserve(operators.size());

    for (const ProbabilisticOperatorProxy op : operators) {
        auto& var_set = affected_vars.emplace_back();
        task_properties::get_affected_vars(
            op,
            std::inserter(var_set, var_set.begin()));
        lp_variables.emplace_back(0.0, lp_infinity, 1.0);
    }

    for (auto& pdb : pdbs_) {
        auto& pdb_constraint = lp_constraints.emplace_back(0.0, lp_infinity);

        for (const ProbabilisticOperatorProxy op : operators) {
            const int op_id = op.get_id();
            const bool affects_pdb = utils::have_common_element(
                pdb->get_pattern(),
                affected_vars[op_id]);

            pdb_constraint.insert(op_id, affects_pdb ? 1.0 : 0.0);
        }
    }
}

void PHOGenerator::update_constraints(const State& state, lp::LPSolver& solver)
{
    for (std::size_t i = 0; i != pdbs_.size(); ++i) {
        solver.set_constraint_lower_bound(i, pdbs_[i]->lookup_estimate(state));
    }
}

void PHOGenerator::reset_constraints(const State&, lp::LPSolver&)
{
    // No need to reset, constraints are overwritten in the next iteration
}

PHOGeneratorFactory::PHOGeneratorFactory(
    std::shared_ptr<pdbs::PatternCollectionGenerator> generator)
    : generator_(std::move(generator))
{
}

std::unique_ptr<ConstraintGenerator>
PHOGeneratorFactory::construct_constraint_generator(
    const std::shared_ptr<ProbabilisticTask>& task,
    const std::shared_ptr<FDRCostFunction>& task_cost_function)
{
    auto r = generator_->generate(task, task_cost_function);
    return std::make_unique<PHOGenerator>(r.get_pdbs());
}

} // namespace probfd::occupation_measures
