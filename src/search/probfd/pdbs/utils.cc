#include "probfd/pdbs/utils.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/utils/graph_visualization.h"

#include "downward/utils/rng.h"

#include <algorithm>
#include <cassert>
#include <sstream>
#include <string>

namespace probfd::pdbs {

Pattern extended_pattern(const Pattern& pattern, int add_var)
{
    assert(!utils::contains(pattern, add_var));

    Pattern added;
    added.reserve(pattern.size() + 1);
    auto it = std::upper_bound(pattern.begin(), pattern.end(), add_var);
    added.insert(added.end(), pattern.begin(), it);
    added.emplace_back(add_var);
    added.insert(added.end(), it, pattern.end());

    return added;
}

std::vector<int> get_goals_in_random_order(
    ProbabilisticTaskProxy task_proxy,
    utils::RandomNumberGenerator& rng)
{
    std::vector<int> goals;

    for (const auto fact : task_proxy.get_goals()) {
        goals.push_back(fact.get_variable().get_id());
    }

    rng.shuffle(goals);

    return goals;
}

void dump_graphviz(
    ProbabilisticTaskProxy task_proxy,
    ProjectionStateSpace& mdp,
    const ProbabilityAwarePatternDatabase& pdb,
    StateRank initial_state,
    std::ostream& out,
    bool transition_labels)
{
    ProjectionOperatorToString op_names(task_proxy);

    auto sts = [&pdb, &mdp](StateRank x) {
        std::ostringstream out;
        out.precision(3);

        const value_t value = pdb.lookup_estimate(x);
        std::string value_text =
            value != INFINITE_VALUE ? std::to_string(value) : "&infin";

        out << x << "\\n"
            << "h = " << value_text;

        if (value == mdp.get_non_goal_termination_cost()) {
            out << "(dead)";
        }

        out << std::endl;

        return out.str();
    };

    auto ats = [=](const ProjectionOperator* const& op) {
        return transition_labels ? op_names(op) : "";
    };

    graphviz::dump_state_space_dot_graph<StateRank, const ProjectionOperator*>(
        out,
        initial_state,
        &mdp,
        nullptr,
        sts,
        ats,
        true);
}

} // namespace probfd::pdbs
