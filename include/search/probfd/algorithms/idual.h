#ifndef PROBFD_ALGORITHMS_IDUAL_H
#define PROBFD_ALGORITHMS_IDUAL_H

#include "probfd/mdp_algorithm.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/progress_report.h"

#include "downward/lp/lp_solver.h"

#include <set>
#include <vector>

// Forward Declarations
namespace utils {
class CountdownTimer;
}

/// Namespace dedicated to the i-dual MDP algorithm.
namespace probfd::algorithms::idual {

/**
 * @brief I-Dual algorithm statistics.
 */
struct Statistics {
    unsigned long long iterations = 0;
    unsigned long long expansions = 0;
    unsigned long long open = 0;
    unsigned long long lp_variables = 0;
    unsigned long long lp_constraints = 0;

    void print(std::ostream& out) const;
};

struct PerStateInfo {
    enum { NEW, OPEN, CLOSED, TERMINAL };
    unsigned idx = std::numeric_limits<unsigned>::max();
    uint8_t status = NEW;
};

struct FrontierStateInfo {
    std::vector<unsigned> incoming;
};

class ValueGroup {
public:
    value_t operator[](unsigned i) const { return values_[i]; }

    unsigned get_id(value_t val);

private:
    struct Comparator {
        const std::vector<value_t>& values;

        bool operator()(unsigned x, unsigned y) const
        {
            return values[x] < values[y];
        }
    };

    std::vector<value_t> values_;
    std::set<unsigned, Comparator> indices_ =
        std::set<unsigned, Comparator>(Comparator{values_});
};

/**
 * @brief Implementation of the I-Dual algorithm \cite trevizan:etal:ijcai-17 .
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class IDual : public MDPAlgorithm<State, Action> {
    using Base = typename IDual::MDPAlgorithm;

    using MDPType = typename Base::MDPType;
    using EvaluatorType = typename Base::EvaluatorType;

    lp::LPSolver lp_solver_;
    storage::PerStateStorage<PerStateInfo> state_infos_;
    ValueGroup terminals_;

    Statistics statistics_;

public:
    explicit IDual(lp::LPSolverType solver_type);

    Interval solve(
        MDPType& mdp,
        EvaluatorType& heuristic,
        param_type<State> initial_state,
        ProgressReport progress,
        double max_time) override;
};

} // namespace probfd::algorithms::idual

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_IDUAL_H
#include "probfd/algorithms/idual_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_IDUAL_H

#endif // PROBFD_ALGORITHMS_IDUAL_H