#ifndef PROBFD_SOLVERS_SOLVER_INTERFACE_H
#define PROBFD_SOLVERS_SOLVER_INTERFACE_H

namespace probfd {
class SolverInterface {
public:
    SolverInterface() = default;
    virtual ~SolverInterface() = default;
    virtual void print_statistics() const {}
    virtual void save_result_if_necessary() const {}
    virtual bool found_solution() const = 0;
    virtual void solve() = 0;
};
} // namespace probfd

#endif
