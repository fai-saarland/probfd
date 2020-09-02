#ifndef SOLVER_INTERFACE_H
#define SOLVER_INTERFACE_H

class SolverInterface {
public:
    SolverInterface();
    virtual ~SolverInterface();
    virtual void print_statistics() const;
    virtual void save_result_if_necessary() const;
    virtual bool found_solution() const = 0;
    virtual void solve() = 0;
};

#endif
