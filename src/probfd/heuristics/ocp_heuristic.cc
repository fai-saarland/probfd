#include "probfd/heuristics/ocp_heuristic.h"

#include "probfd/heuristics/additive_pdb_heuristic.h"
#include "probfd/heuristics/lp_heuristic.h"

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/labelled_successor_pmf.h"
#include "probfd/probabilistic_operator_space.h"
#include "probfd/probabilistic_task.h"
#include "probfd/value_type.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/rng.h"

#include <algorithm>
#include <utility>

using namespace downward;
using namespace probfd::pdbs;

namespace probfd::heuristics {

namespace {
struct PatternInfo {
    std::shared_ptr<ProbabilityAwarePatternDatabase> pdb;
    ProjectionStateSpace state_space;
    int variable_offset;

    explicit PatternInfo(
        const SharedProbabilisticTask& task,
        std::shared_ptr<ProbabilityAwarePatternDatabase> pdb,
        int variable_offset);

    int get_state_variable_index(StateRank abs_state) const
    {
        return variable_offset + static_cast<int>(abs_state);
    }

    int get_cost_variable_index(OperatorID op_id) const
    {
        return get_cost_variable_index(op_id.get_index());
    }

    int get_cost_variable_index(int op_id) const
    {
        return variable_offset + pdb->num_states() + op_id;
    }
};

class OCPHeuristic : public LPHeuristic<OCPHeuristic> {
    std::vector<PatternInfo> projections;
    std::vector<int> abstraction_offsets;
    std::vector<int> abstraction_matrix_offsets;
    int num_total_abstract_states;
    std::vector<named_vector::NamedVector<lp::LPConstraint>>
        abstract_state_constraints;
    std::vector<char> aliveness_matrices;

public:
    explicit OCPHeuristic(
        lp::LPSolverType solver_type,
        const SharedProbabilisticTask& task,
        PPDBCollection pdbs);

    void update_constraints(const State& state) const;
    void reset_constraints(const State& state) const;
};
} // namespace

PatternInfo::PatternInfo(
    const SharedProbabilisticTask& task,
    std::shared_ptr<ProbabilityAwarePatternDatabase> pdb,
    int variable_offset)
    : pdb(std::move(pdb))
    , state_space(task, this->pdb->ranking_function, false)
    , variable_offset(variable_offset)
{
}

OCPHeuristic::OCPHeuristic(
    lp::LPSolverType solver_type,
    const SharedProbabilisticTask& task,
    PPDBCollection pdbs)
    : LPHeuristic(solver_type)
    , num_total_abstract_states(0)
{
    const auto& operators = get_operators(task);
    const auto& cost_function = get_cost_function(task);

    const auto num_operators = operators.size();

    named_vector::NamedVector<lp::LPVariable> variables;
    named_vector::NamedVector<lp::LPConstraint> constraints;

    int sum_of_squares = 0;

    abstraction_offsets.reserve(pdbs.size());
    abstraction_matrix_offsets.reserve(pdbs.size());

    for (auto& pdb : pdbs) {
        const auto& projection =
            projections.emplace_back(task, std::move(pdb), variables.size());

        const int num_abs_states = projection.pdb->num_states();

        abstraction_offsets.push_back(num_total_abstract_states);
        abstraction_matrix_offsets.push_back(sum_of_squares);
        num_total_abstract_states += num_abs_states;
        sum_of_squares += num_abs_states * num_abs_states;

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

    abstract_state_constraints.resize(num_total_abstract_states);
    aliveness_matrices.resize(sum_of_squares, false);

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

    // Compute abstract state transition constraints and aliveness matrices
    // An aliveness matrix is computed by computing the connectivity matrix in
    // the solvable sub-problem and then computing the transitive closure using
    // Floyd-Warshall.
    for (auto [i, projection] : std::views::enumerate(projections)) {
        const auto is_unsolvable = [&](StateRank abs_state) {
            return projection.pdb->lookup_abstract_estimate(abs_state) ==
                   INFINITE_VALUE;
        };

        const int num_abs_states = projection.pdb->num_states();

        auto* proj_constraints =
            abstract_state_constraints.data() + abstraction_offsets[i];

        auto* proj_matrix =
            aliveness_matrices.data() + abstraction_matrix_offsets[i];

        for (StateRank abs_state = 0; abs_state != num_abs_states;
             ++abs_state) {
            // Unsolvable states have no transition constraints
            if (is_unsolvable(abs_state)) {
                continue;
            }

            auto* state_row = proj_matrix + num_abs_states * abs_state;

            // state is connected to itself
            state_row[abs_state] = true;

            const auto var_index =
                projection.get_state_variable_index(abs_state);

            auto& transition_constraints = proj_constraints[abs_state];

            std::vector<LabelledSuccessorPMF<const ProjectionOperator*>>
                transitions;
            projection.state_space.generate_all_transitions(
                abs_state,
                transitions);

            for (const auto& [action, successor_dist] : transitions) {
                // Skip transitions which are not alive
                if (std::ranges::any_of(
                        successor_dist.non_source_successor_pmf.support(),
                        is_unsolvable))
                    continue;

                // Add constraint for this transition.
                // (1 - delta(s)) J_s
                // - C_o + \sum_{t. t != s} -delta(t) * J_t <= 0
                auto& constr = transition_constraints.emplace_back(
                    -lp_solver_.get_infinity(),
                    0.0);

                constr.insert(
                    projection.get_cost_variable_index(action->operator_id),
                    -1);

                if (successor_dist.non_source_probability == 0_vt) continue;

                // 1 - delta(s)
                constr.insert(var_index, successor_dist.non_source_probability);

                for (const auto& [succ, prob] :
                     successor_dist.non_source_successor_pmf) {
                    // - delta(t)
                    constr.insert(
                        projection.get_state_variable_index(succ),
                        -prob);

                    // state is connected to successor
                    state_row[succ] = true;
                }
            }
        }

        // Use edge-connectivity variant of Floyd-Warshall to compute
        // forward-reachability matrix
        for (StateRank k = 0; k != num_abs_states; ++k) {
            for (StateRank s = 0; s != num_abs_states; ++s) {
                if (!proj_matrix[num_abs_states * s + k]) continue;
                for (StateRank t = 0; t != num_abs_states; ++t) {
                    if (!proj_matrix[num_abs_states * k + t]) continue;
                    proj_matrix[num_abs_states * s + t] = true;
                }
            }
        }
    }

    lp_solver_.load_problem(
        lp::LinearProgram{lp::LPObjectiveSense::MAXIMIZE,
                          std::move(variables),
                          std::move(constraints),
                          lp_solver_.get_infinity()});
}

void OCPHeuristic::update_constraints(const State& state) const
{
    named_vector::NamedVector<lp::LPConstraint> constraints;

    for (auto [i, projection] : std::views::enumerate(projections)) {
        const auto is_unsolvable = [&](StateRank abs_state) {
            return projection.pdb->lookup_abstract_estimate(abs_state) ==
                   INFINITE_VALUE;
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

        const auto& proj_state_constraints =
            abstract_state_constraints.data() + abstraction_offsets[i];

        auto* proj_matrix =
            aliveness_matrices.data() + abstraction_matrix_offsets[i];

        const int num_abs_states = projection.pdb->num_states();

        auto* state_row = proj_matrix + num_abs_states * init_abs_state;

        for (StateRank other_abs_state = 0; other_abs_state != num_abs_states;
             ++other_abs_state) {
            if (state_row[other_abs_state]) {
                constraints.append(proj_state_constraints[other_abs_state]);
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