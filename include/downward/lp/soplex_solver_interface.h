#ifndef LP_SOPLEX_SOLVER_INTERFACE_H
#define LP_SOPLEX_SOLVER_INTERFACE_H

#include "downward/lp/solver_interface.h"

#ifdef __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#if (__GNUG__ >= 11) || (__clang_major__ >= 12)
#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#endif
#if __GNUG__ >= 12
#pragma GCC diagnostic ignored "-Wuse-after-free"
#endif
#endif

#include <soplex.h>

#ifdef __GNUG__
#pragma GCC diagnostic pop
#endif

namespace downward::lp {
class SoPlexSolverInterface : public SolverInterface {
    // The reference to the solver is mutable because objValueReal is not const.
    mutable soplex::SoPlex soplex;
    int num_permanent_constraints;
    int num_temporary_constraints;

public:
    SoPlexSolverInterface();

    void load_problem(const LinearProgram& lp) override;
    void add_temporary_constraints(
        const named_vector::NamedVector<LPConstraint>& constraints) override;
    void clear_temporary_constraints() override;
    double get_infinity() const override;

    void set_objective_coefficients(
        const std::vector<double>& coefficients) override;
    void set_objective_coefficient(int index, double coefficient) override;
    void set_constraint_lower_bound(int index, double bound) override;
    void set_constraint_upper_bound(int index, double bound) override;
    void set_variable_lower_bound(int index, double bound) override;
    void set_variable_upper_bound(int index, double bound) override;

    void set_mip_gap(double gap) override;

    void solve() override;
    void write_lp(const std::string& filename) const override;
    void print_failure_analysis() const override;
    bool is_infeasible() const override;
    bool is_unbounded() const override;

    bool has_optimal_solution() const override;

    double get_objective_value() const override;

    std::vector<double> extract_solution() const override;

    int get_num_variables() const override;
    int get_num_constraints() const override;
    bool has_temporary_constraints() const override;
    void print_statistics(std::ostream& out) const override;

    std::vector<double> extract_dual_solution() const override;

    void add_variable(
        const LPVariable& var,
        const std::vector<int>& ids,
        const std::vector<double>& coefs,
        std::string_view name = "") override;

    void
    add_constraint(const LPConstraint& constraint, std::string_view name = "")
        override;
};
} // namespace downward::lp

#endif