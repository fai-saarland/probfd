#include "probfd/cartesian_abstractions/policy_based_flaw_generator.h"

#include "probfd/cartesian_abstractions/policy_flaw_finder.h"
#include "probfd/cartesian_abstractions/policy_generator.h"

#include "probfd/cartesian_abstractions/complete_policy_flaw_finder.h"
#include "probfd/cartesian_abstractions/ilao_policy_generator.h"

#include "probfd/utils/guards.h"

#include "probfd/policy.h"

#include "downward/utils/logging.h"

#include <ostream>

using namespace std;
using namespace downward;

namespace probfd::cartesian_abstractions {

PolicyBasedFlawGenerator::PolicyBasedFlawGenerator(
    PolicyGenerator* policy_generator,
    PolicyFlawFinder* policy_flaw_finder)
    : policy_generator_(policy_generator)
    , policy_flaw_finder_(policy_flaw_finder)
{
}

PolicyBasedFlawGenerator::~PolicyBasedFlawGenerator() = default;

unique_ptr<Solution> PolicyBasedFlawGenerator::find_solution(
    CartesianAbstraction& abstraction,
    const AbstractState* init,
    CartesianHeuristic& heuristic,
    utils::CountdownTimer& timer)
{
    TimerScope scope(find_policy_timer_);
    return policy_generator_
        ->find_solution(abstraction, init, heuristic, timer);
}

optional<Flaw> PolicyBasedFlawGenerator::find_flaw(
    const ProbabilisticTaskProxy& task_proxy,
    const std::vector<int>& domain_sizes,
    CartesianAbstraction& abstraction,
    Solution& solution,
    utils::LogProxy& log,
    utils::CountdownTimer& timer)
{
    TimerScope scope(find_flaw_timer_);
    return policy_flaw_finder_->find_flaw(
        task_proxy,
        domain_sizes,
        abstraction,
        solution,
        log,
        timer);
}

std::optional<Flaw> PolicyBasedFlawGenerator::generate_flaw(
    const ProbabilisticTaskProxy& task_proxy,
    const std::vector<int>& domain_sizes,
    CartesianAbstraction& abstraction,
    const AbstractState* init,
    CartesianHeuristic& heuristic,
    utils::LogProxy& log,
    utils::CountdownTimer& timer)
{
    unique_ptr<Solution> solution =
        find_solution(abstraction, init, heuristic, timer);

    if (!solution) {
        if (log.is_at_least_normal()) {
            log << "Abstract task is unsolvable." << endl;
        }

        return std::nullopt;
    }

    optional<Flaw> flaw =
        find_flaw(task_proxy, domain_sizes, abstraction, *solution, log, timer);

    if (!flaw && log.is_at_least_normal()) {
        log << "Found a policy without a flaw." << endl;
    }

    return flaw;
}

void PolicyBasedFlawGenerator::notify_split()
{
}

void PolicyBasedFlawGenerator::print_statistics(utils::LogProxy& log)
{
    if (log.is_at_least_normal()) {
        log << "Time for finding abstract policies: " << find_policy_timer_
            << endl;
        log << "Time for finding policy flaws: " << find_flaw_timer_ << endl;
    }
}

ILAOFlawGeneratorFactory::ILAOFlawGeneratorFactory(
    value_t convergence_epsilon,
    int max_search_states)
    : convergence_epsilon_(convergence_epsilon)
    , max_search_states_(max_search_states)
{
}

std::unique_ptr<FlawGenerator> ILAOFlawGeneratorFactory::create_flaw_generator()
{
    return std::make_unique<PolicyBasedFlawGenerator>(
        new ILAOPolicyGenerator(convergence_epsilon_),
        new CompletePolicyFlawFinder(max_search_states_));
}

} // namespace probfd::cartesian_abstractions