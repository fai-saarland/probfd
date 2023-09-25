#include "probfd/heuristics/pdbs/cegar/single_cegar.h"

#include "probfd/heuristics/pdbs/cegar/flaw.h"
#include "probfd/heuristics/pdbs/cegar/flaw_generator.h"

#include "probfd/heuristics/pdbs/projection_info.h"
#include "probfd/heuristics/pdbs/utils.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/rng.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

static void refine(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    ProjectionInfo& pdb_info,
    const Flaw& flaw,
    utils::LogProxy log,
    utils::CountdownTimer& timer)
{
    int var = flaw.variable;

    if (log.is_at_least_verbose()) {
        log << "SingleCEGAR: found violated "
            << (flaw.is_precondition ? "precondition" : "goal") << " on " << var
            << endl;
    }

    // compute new solution
    pdb_info = ProjectionInfo(
        task_proxy,
        task_cost_function,
        extended_pattern(pdb_info.get_pattern(), var),
        task_proxy.get_initial_state(),
        *pdb_info.pdb,
        false,
        timer.get_remaining_time());
}

void run_cegar_refinement_loop(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    ProjectionInfo& pdb_info,
    FlawGenerator& flaw_generator,
    utils::LogProxy log,
    utils::CountdownTimer& timer)
{
    if (log.is_at_least_normal()) {
        log << "SingleCEGAR max time: " << timer.get_remaining_time() << "\n"
            << endl;
    }

    int refinement_counter = 1;

    try {
        for (;;) {
            if (log.is_at_least_verbose()) {
                log << "SingleCEGAR: iteration #" << refinement_counter
                    << ", current pattern: " << pdb_info.get_pattern() << endl;
            }

            auto flaw_if_found =
                flaw_generator.generate_flaws(task_proxy, pdb_info, log, timer);

            if (!flaw_if_found) {
                if (log.is_at_least_verbose()) {
                    log << "SingleCEGAR: NO flaw could be produced. "
                        << "No further refinements possible." << endl;
                }
                break;
            }

            timer.throw_if_expired();

            // if there was a flaw, then refine the abstraction
            // such that said flaw does not occur again
            refine(
                task_proxy,
                task_cost_function,
                pdb_info,
                *flaw_if_found,
                log,
                timer);

            ++refinement_counter;
        }
    } catch (utils::TimeoutException&) {
        if (log.is_at_least_normal()) {
            log << "SingleCEGAR: Time limit reached." << endl;
        }
    }

    if (log.is_at_least_normal()) {
        log << "\nSingleCEGAR statistics:\n"
            << "  computation time: " << timer.get_elapsed_time() << "\n"
            << "  number of iterations: " << refinement_counter << "\n"
            << "  final pattern: " << pdb_info.get_pattern() << endl;
    }
}

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd