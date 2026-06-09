#ifndef PROBFD_HEURISTICS_LP_HEURISTIC_H
#define PROBFD_HEURISTICS_LP_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"

#include "downward/lp/lp_solver.h"

#include "downward/utils/timer.h"

#include <memory>
#include <print>
#include <vector>

namespace probfd::heuristics {

/**
 * @brief Base class for heuristics based on linear programming.
 *
 * This class uses CRTP and requires the methods:
 *
 * ```
 * void update_constraints(const State& state) const;
 * void reset_constraints(const State& state) const;
 * ```
 */
template <typename Derived>
class LPHeuristic : public FDRHeuristic {
protected:
    mutable downward::lp::LPSolver lp_solver_;

    mutable std::size_t evaluation_counter = 0;
    mutable std::size_t frequency = 1;

    static constexpr std::size_t next_frequency = 10;
    static constexpr std::size_t frequency_factor = 10;

public:
    explicit LPHeuristic(downward::lp::LPSolverType solver_type)
        : lp_solver_(solver_type)
    {
    }

    value_t evaluate(const downward::State& state) const final
    {
        assert(!lp_solver_.has_temporary_constraints());

        const bool print = evaluation_counter % frequency == 0;

        downward::utils::Timer timer(false);

        if (print) { timer.resume(); }

        static_cast<const Derived*>(this)->update_constraints(state);

        lp_solver_.solve();

        const value_t result = lp_solver_.has_optimal_solution()
                                   ? lp_solver_.get_objective_value()
                                   : INFINITE_VALUE;

        lp_solver_.clear_temporary_constraints();
        static_cast<const Derived*>(this)->reset_constraints(state);

        if (print) {
            std::println(
                "Evaluation #{} took {:.4f}s",
                evaluation_counter,
                timer.stop().count());
        }

        if (++evaluation_counter == frequency * next_frequency) {
            frequency *= frequency_factor;
        }

        return result;
    }
};

} // namespace probfd::heuristics

#endif