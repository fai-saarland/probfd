#ifndef LP_LP_INTERNALS_H
#define LP_LP_INTERNALS_H

/*
  This file provides some internal functions for the LP solver code.
  They could be implemented in linear_program.cc but we moved them here
  to avoid a long and complex file. They should not be necessary outside
  of linear_program.cc. If you need them, think about extending the
  LP class instead.
*/

#include <memory>

class CoinError;
class OsiSolverInterface;

namespace lp {
enum class LPSolverType;

std::unique_ptr<OsiSolverInterface> create_lp_solver(LPSolverType solver_type);

/*
  The MIP gap is an error tolerance on solutions for MIPs. It corresponds to the
  maximal difference ("gap") allowed between the upper- and lower-bound of the
  LP approximations. This function was added to set the MIP gap to 0 in the
  context of optimal planning; the default MIP gap in CPLEX lead to inadmissible
  heuristic values for problems with large action costs and/or long plans (see
  issue983).
*/
extern void set_mip_gap(OsiSolverInterface* lp_solver, double relative_gap);

/*
  Print the CoinError and then exit with ExitCode::SEARCH_CRITICAL_ERROR.
  Note that out-of-memory conditions occurring within CPLEX code cannot
  be caught by a try/catch block. When CPLEX runs out of memory,
  the planner will attempt to terminate gracefully, like it does with
  uncaught out-of-memory exceptions in other parts of the code.
*/
[[noreturn]] void handle_coin_error(const CoinError& error);
}

#endif
