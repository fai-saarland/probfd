#include "lp/lp_solver.h"

#include "lp/lp_internals.h"

#include "utils/system.h"

#include "option_parser.h"

#ifdef USE_LP
#ifdef __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include <CoinPackedMatrix.hpp>
#include <CoinPackedVector.hpp>
#include <OsiSolverInterface.hpp>
#ifdef __GNUG__
#pragma GCC diagnostic pop
#endif
#endif

#include <algorithm>
#include <cassert>
#include <numeric>

using namespace std;

namespace lp {
void
add_lp_solver_option_to_parser(OptionParser& parser)
{
    parser.document_note(
        "Note",
        "to use an LP solver, you must build the planner with LP support. "
        "See LPBuildInstructions.");
    vector<string> lp_solvers;
    vector<string> lp_solvers_doc;
    lp_solvers.push_back("CLP");
    lp_solvers_doc.push_back("default LP solver shipped with the COIN library");
    lp_solvers.push_back("CPLEX");
    lp_solvers_doc.push_back("commercial solver by IBM");
    lp_solvers.push_back("GUROBI");
    lp_solvers_doc.push_back("commercial solver");
    lp_solvers.push_back("SOPLEX");
    lp_solvers_doc.push_back("open source solver by ZIB");
    parser.add_enum_option(
        "lpsolver",
        lp_solvers,
        "external solver that should be used to solve linear programs",
        "CPLEX",
        lp_solvers_doc);
}

LPConstraint::LPConstraint(double lower_bound, double upper_bound)
    : lower_bound(lower_bound)
    , upper_bound(upper_bound)
{
}

void
LPConstraint::clear()
{
    variables.clear();
    coefficients.clear();
}

bool
LPConstraint::empty() const
{
    return variables.empty();
}

void
LPConstraint::insert(int index, double coefficient)
{
    auto it = std::lower_bound(variables.begin(), variables.end(), index);
    unsigned i = std::distance(variables.begin(), it);
    if (it == variables.end() || (*it) > index) {
        variables.insert(it, index);
        coefficients.insert(coefficients.begin() + i, coefficient);
    } else {
        coefficients[i] += coefficient;
    }
}

double
LPConstraint::remove(int index)
{
    auto it = std::lower_bound(variables.begin(), variables.end(), index);
    if (it == variables.end() || (*it) > index) {
        return 0;
    }
    variables.erase(it);
    unsigned i = std::distance(variables.begin(), it);
    const double res = coefficients[i];
    coefficients.erase(coefficients.begin() + i);
    return res;
}

void
LPConstraint::dump() const
{
    std::cout << lower_bound << " <= ";
    for (unsigned i = 0; i < variables.size(); i++) {
        double c = coefficients[i];
        if (i > 0) {
            if (coefficients[i] < 0) {
                std::cout << " - ";
                c = -c;
            } else {
                std::cout << " + ";
            }
        }
        if (c != 1) {
            std::cout << c;
        }
        std::cout << "v" << variables[i];
    }
    std::cout << " <= " << upper_bound << std::endl;
}

LPVariable::LPVariable(
    double lower_bound_,
    double upper_bound_,
    double objective_coefficient_)
    : lower_bound(lower_bound_)
    , upper_bound(upper_bound_)
    , objective_coefficient(objective_coefficient_)
{
}

void
LPVariable::dump(unsigned i) const
{
    std::cout << lower_bound << " <= ";
    if (objective_coefficient != 1) {
        std::cout << objective_coefficient;
    }
    std::cout << "v" << i << " <= " << upper_bound << std::endl;
}

LPSolver::~LPSolver() { }

#ifdef USE_LP

LPSolver::LPSolver(LPSolverType solver_type)
    : is_initialized(false)
    , is_solved(false)
    , num_permanent_constraints(0)
    , num_variables(0)
    , has_temporary_constraints_(false)
    , solver_type(solver_type)
{
    lp_solver = create_lp_solver(solver_type);
}

void
LPSolver::clear_temporary_data()
{
    elements.clear();
    indices.clear();
    starts.clear();
    col_lb.clear();
    col_ub.clear();
    objective.clear();
    row_lb.clear();
    row_ub.clear();
    rows.clear();
}

void
LPSolver::load_problem(
    LPObjectiveSense sense,
    const std::vector<LPVariable>& variables,
    const std::vector<LPConstraint>& constraints)
{
    clear_temporary_data();
    is_initialized = false;
    num_permanent_constraints = constraints.size();
    num_variables = variables.size();

    for (const LPVariable& var : variables) {
        col_lb.push_back(var.lower_bound);
        col_ub.push_back(var.upper_bound);
        objective.push_back(var.objective_coefficient);
    }
    for (const LPConstraint& constraint : constraints) {
        row_lb.push_back(constraint.get_lower_bound());
        row_ub.push_back(constraint.get_upper_bound());
    }

    if (sense == LPObjectiveSense::MINIMIZE) {
        lp_solver->setObjSense(1);
    } else {
        lp_solver->setObjSense(-1);
    }

    for (const LPConstraint& constraint : constraints) {
        const vector<int>& vars = constraint.get_variables();
        const vector<double>& coeffs = constraint.get_coefficients();
        assert(vars.size() == coeffs.size());
        starts.push_back(elements.size());
        indices.insert(indices.end(), vars.begin(), vars.end());
        elements.insert(elements.end(), coeffs.begin(), coeffs.end());
    }
    /*
      There are two ways to pass the lengths of vectors to a CoinMatrix:
      1) 'starts' contains one entry per vector and we pass a separate array
         of vector 'lengths' to the constructor.
      2) If there are no gaps in the elements, we can also add elements.size()
         as a last entry in the vector 'starts' and leave the parameter for
         'lengths' at its default (0).
      OSI recreates the 'lengths' array in any case and uses optimized code
      for the second case, so we use it here.
     */
    starts.push_back(elements.size());

    try {
        CoinPackedMatrix matrix(
            false,
            variables.size(),
            constraints.size(),
            elements.size(),
            elements.data(),
            indices.data(),
            starts.data(),
            0);
        lp_solver->loadProblem(
            matrix,
            col_lb.data(),
            col_ub.data(),
            objective.data(),
            row_lb.data(),
            row_ub.data());
        assert(lp_solver->getNumCols() == (int)variables.size());
        assert(lp_solver->getNumRows() == (int)constraints.size());
    } catch (CoinError& error) {
        handle_coin_error(error);
    }

    clear_temporary_data();
}

void
LPSolver::add_temporary_constraints(
    const std::vector<LPConstraint>& constraints)
{
    if (!constraints.empty()) {
        clear_temporary_data();
        int num_rows = constraints.size();
        for (const LPConstraint& constraint : constraints) {
            row_lb.push_back(constraint.get_lower_bound());
            row_ub.push_back(constraint.get_upper_bound());
            rows.push_back(new CoinShallowPackedVector(
                constraint.get_variables().size(),
                constraint.get_variables().data(),
                constraint.get_coefficients().data(),
                false));
        }

        try {
            lp_solver->addRows(
                num_rows, rows.data(), row_lb.data(), row_ub.data());
        } catch (CoinError& error) {
            handle_coin_error(error);
        }
        for (CoinPackedVectorBase* row : rows) {
            delete row;
        }
        clear_temporary_data();
        has_temporary_constraints_ = true;
        is_solved = false;
    }
}

void
LPSolver::clear_temporary_constraints()
{
    if (has_temporary_constraints_) {
        try {
            lp_solver->restoreBaseModel(num_permanent_constraints);
        } catch (CoinError& error) {
            handle_coin_error(error);
        }
        has_temporary_constraints_ = false;
        is_solved = false;
    }
}

int
LPSolver::add_variable(
    const LPVariable& var,
    const std::vector<int>& constraint_index,
    const std::vector<double>& coefficients)
{
    assert(!has_temporary_constraints_);
    assert(constraint_index.size() == coefficients.size());
    is_solved = false;
    try {
        int res = lp_solver->getNumCols();
        lp_solver->addCol(
            coefficients.size(),
            constraint_index.data(),
            coefficients.data(),
            var.lower_bound,
            var.upper_bound,
            var.objective_coefficient);
        return res; // num_variables++;
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

void
LPSolver::add_variable(
    const LPVariable& var,
    const std::vector<int>& ids,
    const std::vector<double>& coefs,
    const std::string& name)
{
    assert(!has_temporary_constraints_);
    assert(ids.size() == coefs.size());
    is_solved = false;
    try {
        lp_solver->addCol(
            coefs.size(),
            ids.data(),
            coefs.data(),
            var.lower_bound,
            var.upper_bound,
            var.objective_coefficient,
            name);
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

void
LPSolver::add_constraints(const std::vector<LPConstraint>& constraints)
{
    assert(!has_temporary_constraints_);
    add_temporary_constraints(constraints);
    has_temporary_constraints_ = false;
    num_permanent_constraints += constraints.size();
}

void
LPSolver::add_constraint(
    const LPConstraint& constraint,
    const std::string& name)
{
    assert(!has_temporary_constraints_);
    is_solved = false;
    num_permanent_constraints++;
    try {
        CoinShallowPackedVector constr(
            constraint.get_variables().size(),
            constraint.get_variables().data(),
            constraint.get_coefficients().data(),
            false);
        lp_solver->addRow(
            constr,
            constraint.get_lower_bound(),
            constraint.get_upper_bound(),
            name);
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

int
LPSolver::add_constraint(const LPConstraint& constraint)
{
    assert(!has_temporary_constraints_);
    is_solved = false;
    num_permanent_constraints++;
    try {
#ifndef NDEBUG
        std::vector<int> vars(constraint.get_variables());
        assert(std::is_sorted(vars.begin(), vars.end()));
        assert(std::unique(vars.begin(), vars.end()) == vars.end());
#endif

        CoinShallowPackedVector constr(
            constraint.get_variables().size(),
            constraint.get_variables().data(),
            constraint.get_coefficients().data(),
            false);
        int res = lp_solver->getNumRows();
        lp_solver->addRow(
            constr, constraint.get_lower_bound(), constraint.get_upper_bound());
        return res;
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

void
LPSolver::delete_variables(int num, const int* var_ids)
{
    try {
        lp_solver->deleteCols(num, var_ids);
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
    is_solved = false;
}

void
LPSolver::delete_constraints(int num, const int* constraint_ids)
{
    try {
        lp_solver->deleteRows(num, constraint_ids);
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
    is_solved = false;
}

double
LPSolver::get_infinity() const
{
    try {
        return lp_solver->getInfinity();
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

void
LPSolver::set_objective_coefficients(const vector<double>& coefficients)
{
    assert(static_cast<int>(coefficients.size()) == get_num_variables());
    vector<int> indices(coefficients.size());
    iota(indices.begin(), indices.end(), 0);
    try {
        lp_solver->setObjCoeffSet(
            indices.data(),
            indices.data() + indices.size(),
            coefficients.data());
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
    is_solved = false;
}

void
LPSolver::set_objective_coefficient(int index, double coefficient)
{
    assert(index < get_num_variables());
    try {
        lp_solver->setObjCoeff(index, coefficient);
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
    is_solved = false;
}

void
LPSolver::set_constraint_lower_bound(int index, double bound)
{
    assert(index < get_num_constraints());
    try {
        lp_solver->setRowLower(index, bound);
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
    is_solved = false;
}

void
LPSolver::set_constraint_upper_bound(int index, double bound)
{
    assert(index < get_num_constraints());
    try {
        lp_solver->setRowUpper(index, bound);
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
    is_solved = false;
}

void
LPSolver::set_constraint_bounds(int index, double lb, double ub)
{
    assert(index < get_num_constraints());
    try {
        lp_solver->setRowBounds(index, lb, ub);
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
    is_solved = false;
}

void
LPSolver::set_variable_lower_bound(int index, double bound)
{
    assert(index < get_num_variables());
    try {
        lp_solver->setColLower(index, bound);
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
    is_solved = false;
}

void
LPSolver::set_variable_upper_bound(int index, double bound)
{
    assert(index < get_num_variables());
    try {
        lp_solver->setColUpper(index, bound);
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
    is_solved = false;
}

void
LPSolver::set_variable_upper_bounds(const std::vector<double>& bounds)
{
    assert((int)bounds.size() == get_num_variables());
    try {
        lp_solver->setColUpper(bounds.data());
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
    is_solved = false;
}

void
LPSolver::solve()
{
    try {
        // TODO REMOVE
        is_initialized = false;
        if (is_initialized) {
            lp_solver->resolve();
        } else {
            lp_solver->initialSolve();
            is_initialized = true;
        }
        if (lp_solver->isAbandoned()) {
            // The documentation of OSI is not very clear here but memory seems
            // to be the most common cause for this in our case.
            cerr << "Abandoned LP during resolve. "
                 << "Reasons include \"numerical difficulties\" and running "
                    "out of memory."
                 << endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }
        is_solved = true;
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

bool
LPSolver::has_optimal_solution() const
{
    assert(is_solved);
    try {
        // std::cout << lp_solver->isProvenPrimalInfeasible() << std::endl;
        // std::cout << lp_solver->isProvenDualInfeasible() << std::endl;
        // std::cout << lp_solver->isProvenOptimal() << std::endl;
        return !lp_solver->isProvenPrimalInfeasible()
            && !lp_solver->isProvenDualInfeasible()
            && lp_solver->isProvenOptimal();
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

double
LPSolver::get_objective_value() const
{
    assert(has_optimal_solution());
    try {
        return lp_solver->getObjValue();
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

std::vector<double>
LPSolver::extract_solution() const
{
    assert(has_optimal_solution());
    try {
        const double* sol = lp_solver->getColSolution();
        return std::vector<double>(sol, sol + get_num_variables());
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

std::vector<double>
LPSolver::extract_dual_solution() const
{
    assert(has_optimal_solution());
    try {
        const double* sol = lp_solver->getRowPrice();
        return std::vector<double>(sol, sol + lp_solver->getNumRows());
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

int
LPSolver::get_num_variables() const
{
    try {
        return lp_solver->getNumCols();
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

int
LPSolver::get_num_constraints() const
{
    try {
        return lp_solver->getNumRows();
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

double
LPSolver::get_constraint_lower_bound(int index) const
{
    try {
        return lp_solver->getRowLower()[index];
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

double
LPSolver::get_constraint_upper_bound(int index) const
{
    try {
        return lp_solver->getRowUpper()[index];
    } catch (CoinError& error) {
        handle_coin_error(error);
    }
}

int
LPSolver::has_temporary_constraints() const
{
    return has_temporary_constraints_;
}

void
LPSolver::print_statistics() const
{
    cout << "LP variables: " << get_num_variables() << endl;
    cout << "LP constraints: " << get_num_constraints() << endl;
}

LPSolverType
LPSolver::get_type() const
{
    return solver_type;
}

void
LPSolver::store(const char* file) const
{
    lp_solver->writeLp(file);
}

void
LPSolver::get_constraint(int index, std::vector<double>& coefficients) const
{
    coefficients.clear();
    assert(index < lp_solver->getNumRows());
    const CoinPackedMatrix* matrix = lp_solver->getMatrixByRow();
    assert(matrix != NULL);
    const auto& v = matrix->getVector(index);
    const auto& ind = v.getIndices();
    const auto& coe = v.getElements();
    coefficients.resize(lp_solver->getNumCols(), 0);
    for (int i = 0; i < v.getNumElements(); i++) {
        assert(ind[i] < (int)coefficients.size());
        coefficients[ind[i]] = coe[i];
    }
}

#endif
} // namespace lp
