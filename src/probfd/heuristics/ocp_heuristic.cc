#include "probfd/heuristics/ocp_heuristic.h"

#include "probfd/heuristics/additive_pdb_heuristic.h"

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/saturation.h"

#include "probfd/tasks/range_operator_cost_function.h"

#include "probfd/probabilistic_operator_space.h"
#include "probfd/probabilistic_task.h"
#include "probfd/transition_tail.h"
#include "probfd/value_type.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/rng.h"

#include "downward/initial_state_values.h"
#include "probfd/heuristics/lp_heuristic.h"
#include "probfd/pdbs/projection_operator.h"

#include <algorithm>
#include <cassert>
#include <utility>

using namespace downward;
using namespace probfd::pdbs;

namespace probfd::heuristics {

struct PatternInfo {
    std::shared_ptr<ProbabilityAwarePatternDatabase> pdb;
    ProjectionStateSpace state_space;
    int variable_offset;

    explicit PatternInfo(
        SharedProbabilisticTask task,
        std::shared_ptr<ProbabilityAwarePatternDatabase> pdb,
        int variable_offset);

    int get_state_variable_index(StateRank abs_state) const
    { return variable_offset + static_cast<int>(abs_state); }

    int get_cost_variable_index(OperatorID op_id) const
    { return get_cost_variable_index(op_id.get_index()); }

    int get_cost_variable_index(int op_id) const
    { return variable_offset + pdb->num_states() + op_id; }
};

class OCPHeuristic : public LPHeuristic<OCPHeuristic> {
    std::vector<PatternInfo> projections;

public:
    explicit OCPHeuristic(
        lp::LPSolverType solver_type,
        const SharedProbabilisticTask& task,
        PPDBCollection pdbs);

    void update_constraints(const State& state) const;
    void reset_constraints(const State& state) const;
};

PatternInfo::PatternInfo(
    SharedProbabilisticTask task,
    std::shared_ptr<ProbabilityAwarePatternDatabase> pdb,
    int variable_offset)
    : pdb(std::move(pdb))
    , state_space(std::move(task), this->pdb->ranking_function, false)
    , variable_offset(variable_offset)
{
}

OCPHeuristic::OCPHeuristic(
    lp::LPSolverType solver_type,
    const SharedProbabilisticTask& task,
    PPDBCollection pdbs)
    : LPHeuristic(solver_type)
{
    const auto& operators = get_operators(task);
    const auto& cost_function = get_cost_function(task);

    const auto num_operators = operators.size();

    named_vector::NamedVector<lp::LPVariable> variables;
    named_vector::NamedVector<lp::LPConstraint> constraints;

    for (auto& pdb : pdbs) {
        const auto& projection =
            projections.emplace_back(task, std::move(pdb), variables.size());

        const int num_abs_states = projection.pdb->num_states();

        for (int i = 0; i != num_abs_states; ++i) {
            variables.emplace_back(
                -lp_solver_.get_infinity(),
                projection.state_space.is_goal(i) ? 0.0
                                                  : lp_solver_.get_infinity(),
                0.0);
        }

        // Add cost variables
        for (std::size_t i = 0; i != num_operators; ++i) {
            variables.emplace_back(
                -lp_solver_.get_infinity(),
                lp_solver_.get_infinity(),
                0.0);
        }
    }

    // Add cost partitioning constraints
    for (const auto& op : operators) {
        auto& cp_constr = constraints.emplace_back(
            -lp_solver_.get_infinity(),
            cost_function.get_operator_cost(op.get_id()));

        for (auto& projection : projections) {
            cp_constr.insert(
                projection.get_cost_variable_index(op.get_id()),
                1);
        }
    }

    lp_solver_.load_problem(
        lp::LinearProgram{
            lp::LPObjectiveSense::MAXIMIZE,
            std::move(variables),
            std::move(constraints),
            lp_solver_.get_infinity()});
}

void OCPHeuristic::update_constraints(const State& state) const
{
    named_vector::NamedVector<lp::LPConstraint> constraints;

    for (auto& projection : projections) {
        const auto is_unsolvable = [&](StateRank abs_state) {
            return projection.pdb->lookup_estimate(abs_state) == INFINITE_VALUE;
        };

        const auto init_abs_state = projection.pdb->get_abstract_state(state);

        // Set objective coefficient
        lp_solver_.set_objective_coefficient(
            projection.get_state_variable_index(init_abs_state),
            1);

        // Check if state is solvable
        if (is_unsolvable(init_abs_state)) {
            continue;
        }

        // Traverse alive-restricted projections and add constraints for each
        // encountered transition.
        std::stack<StateRank> queue({init_abs_state});
        std::vector<bool> seen(projection.pdb->num_states(), false);
        seen[init_abs_state] = true;

        while (!queue.empty()) {
            const int next_state = queue.top();
            queue.pop();

            const int next_state_var_index =
                projection.get_state_variable_index(next_state);

            std::vector<TransitionTail<const ProjectionOperator*>> transitions;
            projection.state_space.generate_all_transitions(
                next_state,
                transitions);

            for (const auto& [action, successor_dist] : transitions) {
                // Skip transitions which are not alive
                if (std::ranges::any_of(
                        successor_dist.non_source_successor_dist.support(),
                        is_unsolvable))
                    continue;

                // Add constraint for this transition.
                // (1 - delta(s)) J_s
                // - C_o + \sum_{t. t != s} -delta(t) * J_t <= 0
                auto& constr =
                    constraints.emplace_back(-lp_solver_.get_infinity(), 0.0);

                constr.insert(
                    projection.get_cost_variable_index(action->operator_id),
                    -1);

                if (successor_dist.non_source_probability == 0_vt) continue;

                // 1 - delta(s)
                constr.insert(
                    next_state_var_index,
                    successor_dist.non_source_probability);

                for (const auto& [succ, prob] :
                     successor_dist.non_source_successor_dist) {
                    // - delta(t)
                    constr.insert(
                        projection.get_state_variable_index(succ),
                        -prob);

                    // add successor state to exploration queue
                    if (!seen[succ]) {
                        queue.push(succ);
                        seen[succ] = true;
                    }
                }
            }
        }
    }

    lp_solver_.add_temporary_constraints(constraints);
}

void OCPHeuristic::reset_constraints(const State& state) const
{
    for (auto& projection : projections) {
        const auto init_abs_state = projection.pdb->get_abstract_state(state);

        // Reset objective coefficient
        lp_solver_.set_objective_coefficient(
            projection.get_state_variable_index(init_abs_state),
            0);
    }

    lp_solver_.clear_temporary_constraints();
}

OCPHeuristicFactory::OCPHeuristicFactory(
    downward::lp::LPSolverType solver_type,
    std::shared_ptr<PatternCollectionGenerator> pattern_collection_generator)
    : solver_type(solver_type)
    , pattern_collection_generator_(std::move(pattern_collection_generator))
{
}

std::unique_ptr<FDRHeuristic>
OCPHeuristicFactory::create_object(const SharedProbabilisticTask& task)
{
    auto pattern_collection_info =
        pattern_collection_generator_->generate(task);

    auto patterns = pattern_collection_info.get_patterns();
    auto pdbs = pattern_collection_info.get_pdbs();

    return std::make_unique<OCPHeuristic>(solver_type, task, std::move(pdbs));
}

} // namespace probfd::heuristics
