#include "probfd/heuristics/pdbs/cegar/single_cegar.h"

#include "probfd/heuristics/pdbs/cegar/flaw_generator.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"
#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/task_proxy.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/math.h"

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

SingleCEGARResult::~SingleCEGARResult() = default;

SingleCEGAR::SingleCEGAR(
    int max_pdb_size,
    double max_time,
    std::shared_ptr<FlawGenerator> flaw_generator,
    int goal,
    utils::LogProxy log,
    std::unordered_set<int> blacklisted_variables)
    : max_pdb_size(max_pdb_size)
    , max_time(max_time)
    , flaw_generator(std::move(flaw_generator))
    , goal(goal)
    , log(std::move(log))
    , blacklisted_variables(std::move(blacklisted_variables))
{
}

SingleCEGAR::~SingleCEGAR() = default;

void SingleCEGAR::refine(
    const ProbabilisticTaskProxy& task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    StateRankingFunction& abstraction_mapping,
    ProjectionStateSpace& projection_mdp,
    IncrementalValueTableEvaluator& h,
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

    abstraction_mapping = StateRankingFunction(
        task_proxy.get_variables(),
        extended_pattern(abstraction_mapping.get_pattern(), var));

    projection_mdp = ProjectionStateSpace(
        task_proxy,
        abstraction_mapping,
        task_cost_function,
        false,
        timer.get_remaining_time());

    h.on_refinement(abstraction_mapping, var);
}

SingleCEGARResult SingleCEGAR::generate_pdb(
    const ProbabilisticTaskProxy& task_proxy,
    FDRSimpleCostFunction& task_cost_function)
{
    if (log.is_at_least_normal()) {
        log << "SingleCEGAR options: \n"
            << "  flaw generation: " << flaw_generator->get_name() << "\n"
            << "  max pdb size: " << max_pdb_size << "\n"
            << "  max time: " << max_time << "\n"
            << "  goal variable: " << goal << "\n"
            << "  blacklisted variables: " << blacklisted_variables << "\n"
            << endl;
    }

    utils::CountdownTimer timer(max_time);

    const State initial_state = task_proxy.get_initial_state();
    initial_state.unpack();

    // Start with an initial abstraction mapping, projection state space and
    // a value table of appropriate size filled with zeroes.
    StateRankingFunction abstraction_mapping(
        task_proxy.get_variables(),
        {goal});

    std::unique_ptr<ProjectionStateSpace> projection(new ProjectionStateSpace(
        task_proxy,
        abstraction_mapping,
        task_cost_function,
        false,
        timer.get_remaining_time()));

    IncrementalValueTableEvaluator h(abstraction_mapping.num_states());

    if (log.is_at_least_normal()) {
        log << "SingleCEGAR initial pattern: "
            << abstraction_mapping.get_pattern();

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
                abstraction_mapping,
                *projection,
                h,
                initial_state,
                termination_cost,
                blacklisted_variables,
                max_pdb_size,
                log,
                timer);

            if (!flaw) {
                break;
            }

            timer.throw_if_expired();

            // if there was a flaw, then refine the abstraction
            // such that said flaw does not occur again
            refine(
                task_proxy,
                task_cost_function,
                abstraction_mapping,
                *projection,
                h,
                *flaw,
                timer);

            if (log.is_at_least_verbose()) {
                log << "SingleCEGAR: current pattern: "
                    << abstraction_mapping.get_pattern() << endl;
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
            << "  final pattern: " << abstraction_mapping.get_pattern() << "\n";
    }

    return SingleCEGARResult{
        std::move(projection),
        std::make_unique<ProbabilityAwarePatternDatabase>(
            std::move(abstraction_mapping),
            std::move(h.get_value_table()))};
}

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd