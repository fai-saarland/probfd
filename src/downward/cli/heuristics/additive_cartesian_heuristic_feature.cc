#include "downward/cli/heuristics/additive_cartesian_heuristic_feature.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_function_definition.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/cartesian_abstractions/additive_cartesian_heuristic.h"
#include "downward/cartesian_abstractions/cartesian_heuristic_function.h"
#include "downward/cartesian_abstractions/cost_saturation.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::cartesian_abstractions;
using namespace downward::utils;

using namespace downward::cli;
using namespace language::parser;

using downward::cli::utils::add_rng_options_to_feature;

namespace {
class AdditiveCartesianHeuristicFactory
    : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    Verbosity verbosity;
    std::vector<std::shared_ptr<SubtaskGenerator>> subtasks;
    int max_states;
    int max_transitions;
    FSeconds max_time;
    PickSplit pick;
    bool use_general_costs;
    std::shared_ptr<RandomNumberGenerator> rng;

public:
    AdditiveCartesianHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        Verbosity verbosity,
        std::vector<std::shared_ptr<SubtaskGenerator>> subtasks,
        int max_states,
        int max_transitions,
        FSeconds max_time,
        PickSplit pick,
        bool use_general_costs,
        std::shared_ptr<RandomNumberGenerator> rng)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , subtasks(std::move(subtasks))
        , max_states(max_states)
        , max_transitions(max_transitions)
        , max_time(max_time)
        , pick(pick)
        , use_general_costs(use_general_costs)
        , rng(std::move(rng))
    {
        if (max_states < 1) {
            throw std::domain_error("max_states must be >= 1.");
        }

        if (max_transitions < 0) {
            throw std::domain_error("max_transitions must be >= 0.");
        }
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);
        return std::make_unique<AdditiveCartesianHeuristic>(
            std::move(subtasks),
            max_states,
            max_transitions,
            max_time,
            pick,
            use_general_costs,
            *rng,
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

} // namespace

namespace downward::cli::heuristics {

void add_additive_cartesian_heuristic_categories(
    NamespaceLevelDeclarationList& nspace)
{
    insert_enum_declaration<PickSplit>(
        nspace,
        "PickSplit",
        {{"random", "select a random variable (among all eligible variables)"},
         {"min_unwanted",
          "select an eligible variable which has the least unwanted values "
          "(number of values of v that land in the abstract state whose "
          "h-value will probably be raised) in the flaw state"},
         {"max_unwanted",
          "select an eligible variable which has the most unwanted values "
          "(number of values of v that land in the abstract state whose "
          "h-value will probably be raised) in the flaw state"},
         {"min_refined",
          "select an eligible variable which is the least refined "
          "(-1 * (remaining_values(v) / original_domain_size(v))) "
          "in the flaw state"},
         {"max_refined",
          "select an eligible variable which is the most refined "
          "(-1 * (remaining_values(v) / original_domain_size(v))) "
          "in the flaw state"},
         {"min_hadd",
          "select an eligible variable with minimal h^add(s_0) value "
          "over all facts that need to be removed from the flaw state"},
         {"max_hadd",
          "select an eligible variable with maximal h^add(s_0) value "
          "over all facts that need to be removed from the flaw state"}});
}

InternalFunctionDefinitionBase&
add_additive_cartesian_heuristic_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskDependentFactory<Evaluator>,
        AdditiveCartesianHeuristicFactory,
        shared_ptr<TaskTransformation>,
        bool,
        string,
        Verbosity,
        std::vector<std::shared_ptr<SubtaskGenerator>>,
        int,
        int,
        FSeconds,
        PickSplit,
        bool,
        std::shared_ptr<RandomNumberGenerator>>>(nspace, "cegar");

    f.document_title("Additive CEGAR heuristic");
    f.document_synopsis(
        "See the paper introducing Counterexample-guided Abstraction "
        "Refinement (CEGAR) for classical planning:" +
        format_conference_reference(
            {"Jendrik Seipp", "Malte Helmert"},
            "Counterexample-guided Cartesian Abstraction Refinement",
            "https://ai.dmi.unibas.ch/papers/seipp-helmert-icaps2013.pdf",
            "Proceedings of the 23rd International Conference on Automated "
            "Planning and Scheduling (ICAPS 2013)",
            "347-351",
            "AAAI Press",
            "2013") +
        "and the paper showing how to make the abstractions additive:" +
        format_conference_reference(
            {"Jendrik Seipp", "Malte Helmert"},
            "Diverse and Additive Cartesian Abstraction Heuristics",
            "https://ai.dmi.unibas.ch/papers/seipp-helmert-icaps2014.pdf",
            "Proceedings of the 24th International Conference on "
            "Automated Planning and Scheduling (ICAPS 2014)",
            "289-297",
            "AAAI Press",
            "2014") +
        "For more details on Cartesian CEGAR and saturated cost "
        "partitioning, "
        "see the journal paper" +
        format_journal_reference(
            {"Jendrik Seipp", "Malte Helmert"},
            "Counterexample-Guided Cartesian Abstraction Refinement for "
            "Classical Planning",
            "https://ai.dmi.unibas.ch/papers/seipp-helmert-jair2018.pdf",
            "Journal of Artificial Intelligence Research",
            "62",
            "535-577",
            "2018"));

    f.document_language_support("action costs", "supported");
    f.document_language_support("conditional effects", "not supported");
    f.document_language_support("axioms", "not supported");

    f.document_property("admissible", "yes");
    f.document_property("consistent", "yes");
    f.document_property("safe", "yes");
    f.document_property("preferred operators", "no");

    const auto n = add_heuristic_options_to_feature(f, "cegar", 0);

    f.make_optional_argument_with_default(
        n,
        "subtasks",
        "[landmarks(),goals()]",
        "subtask generators");
    f.make_optional_argument_with_default(
        n + 1,
        "max_states",
        "infinity()",
        "maximum sum of abstract states over all abstractions");
    f.make_optional_argument_with_default(
        n + 2,
        "max_transitions",
        "1M",
        "maximum sum of real transitions (excluding self-loops) over "
        " all abstractions");
    f.make_optional_argument_with_default(
        n + 3,
        "max_time",
        "seconds_max()",
        "maximum time in seconds for building abstractions");
    f.make_optional_argument_with_default(
        n + 4,
        "pick",
        "max_refined",
        "how to choose on which variable to split the flaw state");
    f.make_optional_argument_with_default(
        n + 5,
        "use_general_costs",
        "true",
        "allow negative costs in cost partitioning");
    add_rng_options_to_feature(f, n + 6);

    return f;
}

} // namespace downward::cli::heuristics
