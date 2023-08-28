#include "probfd/heuristics/pdbs/cegar/single_cegar.h"

#include "probfd/heuristics/pdbs/cegar/flaw_generator.h"
#include "probfd/heuristics/pdbs/cegar/projection_factory.h"
#include "probfd/heuristics/pdbs/cegar/projection_info.h"

#include "probfd/heuristics/pdbs/projection_state_space.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"
#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/task_proxy.h"

#include "downward/utils/countdown_timer.h"

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

SingleCEGAR::SingleCEGAR(
    double max_time,
    std::shared_ptr<FlawGenerator> flaw_generator,
    std::shared_ptr<ProjectionFactory> projection_factory,
    utils::LogProxy log)
    : max_time(max_time)
    , flaw_generator(std::move(flaw_generator))
    , projection_factory(std::move(projection_factory))
    , log(std::move(log))
{
}

SingleCEGAR::~SingleCEGAR() = default;

void SingleCEGAR::refine(
    const ProbabilisticTaskProxy& task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    ProjectionInfo& projection_info,
    Flaw flaw,
    utils::CountdownTimer& timer)
{
    const int var = flaw.variable;

    if (log.is_at_least_verbose()) {
        log << "SingleCEGAR: chosen flaw: violated";
        if (flaw.is_precondition) {
            log << " precondition ";
        } else {
            log << " goal ";
        }
        log << "on " << var << endl;
    }

    *projection_info.abstraction_mapping = StateRankingFunction(
        task_proxy.get_variables(),
        extended_pattern(
            projection_info.abstraction_mapping->get_pattern(),
            var));

    *projection_info.projection_mdp = ProjectionStateSpace(
        task_proxy,
        *projection_info.abstraction_mapping,
        task_cost_function,
        false,
        timer.get_remaining_time());

    projection_info.on_refinement(var);
}

ProjectionInfo SingleCEGAR::generate_pdb(
    const ProbabilisticTaskProxy& task_proxy,
    FDRSimpleCostFunction& task_cost_function)
{
    if (log.is_at_least_normal()) {
        log << "SingleCEGAR options: \n"
            << "  flaw generation: " << flaw_generator->get_name() << "\n"
            << "  max time: " << max_time << "\n"
            << endl;
    }

    utils::CountdownTimer timer(max_time);

    const State initial_state = task_proxy.get_initial_state();
    initial_state.unpack();

    ProjectionInfo projection_info = projection_factory->create_projection(
        task_proxy,
        task_cost_function,
        timer);

    if (log.is_at_least_normal()) {
        log << "SingleCEGAR initial pattern: " << projection_info.get_pattern();

        if (log.is_at_least_verbose()) {
            log << endl;
        }
    }

    // main loop of the algorithm
    int refinement_counter = 1;

    const value_t termination_cost =
        task_cost_function.get_non_goal_termination_cost();

    try {
        for (;; ++refinement_counter) {
            if (log.is_at_least_verbose()) {
                log << "iteration #" << refinement_counter << endl;
            }

            auto flaw = flaw_generator->generate_flaw(
                task_proxy,
                projection_info,
                initial_state,
                termination_cost,
                log,
                timer);

            if (!flaw) {
                if (log.is_at_least_verbose()) {
                    log << "No flaw was generated. Stopping refinement loop."
                        << endl;
                }
                break;
            }

            timer.throw_if_expired();

            // if there was a flaw, then refine the abstraction
            // such that said flaw does not occur again
            refine(
                task_proxy,
                task_cost_function,
                projection_info,
                *flaw,
                timer);

            if (log.is_at_least_verbose()) {
                log << "SingleCEGAR: current pattern: "
                    << projection_info.get_pattern() << endl;
            }
        }
    } catch (utils::TimeoutException&) {
        if (log.is_at_least_normal()) {
            log << "SingleCEGAR: Time limit reached." << endl;
        }
    }

    if (log.is_at_least_normal()) {
        log << endl
            << "SingleCEGAR statistics:\n"
            << "  computation time: " << timer.get_elapsed_time() << "\n"
            << "  number of iterations: " << refinement_counter << "\n"
            << "  final pattern: " << projection_info.get_pattern() << "\n";
    }

    return projection_info;
}

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd