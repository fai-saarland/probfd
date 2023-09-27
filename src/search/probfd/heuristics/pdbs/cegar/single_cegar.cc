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
    ProbabilisticTaskProxy proxy,
    FDRSimpleCostFunction& task_cost_function,
    ProjectionInfo& proj,
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
    proj = ProjectionInfo(
        proxy,
        task_cost_function,
        extended_pattern(proj.get_pattern(), var),
        proxy.get_initial_state(),
        *proj.pdb,
        false,
        timer.get_remaining_time());
}

static auto next_flaw(
    ProbabilisticTaskProxy proxy,
    ProjectionInfo& proj,
    FlawGenerator& generator,
    utils::LogProxy log,
    utils::CountdownTimer& timer,
    int iteration)
{
    if (log.is_at_least_verbose()) {
        log << "SingleCEGAR: iteration #" << iteration
            << ", current pattern: " << proj.get_pattern() << endl;
    }

    timer.throw_if_expired();

    return generator.next_flaw(proxy, proj, log, timer);
}

void run_cegar_refinement_loop(
    ProbabilisticTaskProxy proxy,
    FDRSimpleCostFunction& task_cost_function,
    ProjectionInfo& proj,
    FlawGenerator& gen,
    utils::LogProxy log,
    utils::CountdownTimer& timer)
{
    if (log.is_at_least_normal()) {
        log << "SingleCEGAR max time: " << timer.get_remaining_time() << "\n"
            << endl;
    }

    int iteration = 1;

    try {
        while (auto flaw = next_flaw(proxy, proj, gen, log, timer, iteration)) {
            refine(proxy, task_cost_function, proj, *flaw, log, timer);
            ++iteration;
        }

        if (log.is_at_least_normal()) {
            log << "SingleCEGAR: No flaw could be produced. "
                << "No further refinements possible." << endl;
        }
    } catch (utils::TimeoutException&) {
        if (log.is_at_least_normal()) {
            log << "SingleCEGAR: Time limit reached." << endl;
        }
    }

    if (log.is_at_least_normal()) {
        log << "\nSingleCEGAR statistics:\n"
            << "  computation time: " << timer.get_elapsed_time() << "\n"
            << "  number of iterations: " << iteration << "\n"
            << "  final pattern: " << proj.get_pattern() << endl;
    }
}

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd