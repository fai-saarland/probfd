#include "expected_cost_pdb_heuristic.h"

#include "../../../../globals.h"
#include "../../../../operator_cost.h"
#include "../../../../option_parser.h"
#include "../../../../pdbs/pattern_collection_information.h"
#include "../../../../pdbs/pattern_generator.h"
#include "../../../../plugin.h"
#include "../../../../utils/countdown_timer.h"
#include "../../../analysis_objective.h"
#include "../../../globals.h"
#include "../../../logging.h"
#include "../probabilistic_projection.h"
#include "../../../analysis_objectives/expected_cost_objective.h"
#include "../../../../algorithms/max_cliques.h"
#include "../multiplicativity.h"

#include <cassert>
#include <iostream>
#include <string>
#include <iomanip>
#include <numeric>

namespace probabilistic {
namespace pdbs {

using ::pdbs::PatternCollectionGenerator;
using ::pdbs::PatternCollectionInformation;
using ::pdbs::PatternCollection;

namespace {
#ifndef NDEBUG
void dump_pattern(
    std::ostream &out,
    unsigned id,
    const ExpectedCostPDBHeuristic::Pattern &p)
{
    out << "pattern[" << id << "]: vars = [";

    for (unsigned j = 0; j < p.size(); j++) {
        out << (j > 0 ? ", " : "") << p[j];
    }

    out << "] ({";

    for (unsigned j = 0; j < p.size(); j++) {
        out << (j > 0 ? ", " : "") << ::g_fact_names[p[j]][0];
    }

    out << "})" << std::flush;
}
#endif

void dumpProjection(
    unsigned int projection_id,
    ProbabilisticProjection &projection)
{
    std::ostringstream path;
    path << "pattern" << projection_id << ".dot";
    const bool print_transition_labels = true;
    const bool print_values = true;
    dump_graphviz(
        &projection,
        g_analysis_objective.get(),
        path.str(),
        print_transition_labels,
        print_values);
}

}


struct ExpectedCostPDBHeuristic::ProjectionInfo {
    ProjectionInfo(
        std::shared_ptr<AbstractStateMapper> state_mapper,
        AbstractAnalysisResult& result);

    std::shared_ptr<AbstractStateMapper> state_mapper;
    std::unique_ptr<QuantitativeResultStore> values;

    [[nodiscard]] value_type::value_t lookup(const GlobalState& s) const;
    [[nodiscard]] value_type::value_t lookup(const AbstractState& s) const;
};

ExpectedCostPDBHeuristic::ProjectionInfo::ProjectionInfo(
    std::shared_ptr<AbstractStateMapper> state_mapper,
    AbstractAnalysisResult& result)
    : state_mapper(std::move(state_mapper))
    , values(result.value_table)
{
}

value_type::value_t
ExpectedCostPDBHeuristic::ProjectionInfo::lookup(const GlobalState& s) const
{
    return lookup(state_mapper->operator()(s));
}

value_type::value_t
ExpectedCostPDBHeuristic::ProjectionInfo::lookup(const AbstractState& s) const
{
    assert(values);
    return values->get(s);
}

void ExpectedCostPDBHeuristic::dump_init_statistics(std::ostream &out) const {
    out << "  Additivity: " << (statistics_.additive ? "Enabled" : "Disabled")
        << std::endl;
    out << "  Construction time: " << statistics_.init_time << "s" << std::endl;
    out << "  Projections: " << statistics_.num_patterns << std::endl;
    out << "  Abstract states: " << statistics_.total_states << std::endl;
    out << "  Additive sub-collections: "
        << statistics_.num_additive_subcollections
        << std::endl;
    out << "  Largest additive subcollection size: "
        << statistics_.largest_additive_subcollection_size << std::endl;
    out << "  Average additive subcollection size: "
        << statistics_.average_additive_subcollection_size << std::endl;
}


ExpectedCostPDBHeuristic::ExpectedCostPDBHeuristic(
    const options::Options& opts)
{
    assert(dynamic_cast<ExpectedCostObjective*>(g_analysis_objective.get()));

    ::verify_no_axioms_no_conditional_effects();

    logging::out << "Initializing Probabilistic PDB Heuristic..." << std::endl;
    utils::Timer t_init;

    auto patterns_generator =
        opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns");
    auto patterns_info = patterns_generator->generate(NORMAL);
    auto patterns = patterns_info.get_patterns();

    database_.reserve(patterns->size());

    const bool countdown_enabled = opts.get<double>("time_limit") > 0;
    utils::CountdownTimer countdown(opts.get<double>("time_limit"));
    const unsigned max_states = opts.get<int>("max_states");

    // Statistics.
    bool terminate = false;

    for (unsigned i = 0; i < patterns->size() && !terminate; i++) {
        const auto& p = patterns->operator[](i);

        // Check if the time ran out
        if (countdown_enabled && countdown.is_expired()) {
            break;
        }

        // Set up the projection state space
        ProbabilisticProjection projection(p, ::g_variable_domain);
        auto state_mapper = projection.get_abstract_state_mapper();

        // If we exceed max_states, this will be the last projection
        if (max_states - state_mapper->size() < statistics_.total_states) {
            terminate = true;
        }

        // FIXME: Dumping projections is not supported for this analysis obj
        //if (opts.get<bool>("dump_projections")) {
        //    dumpProjection(i, projection);
        //}

        // Compute the value table for this projection
        AbstractAnalysisResult result =
            compute_value_table(&projection, g_analysis_objective.get());

        // Add to the list of PDB heuristics and update statistics.
        database_.emplace_back(state_mapper, result);
        statistics_.total_states += state_mapper->size();

#ifndef NDEBUG
        {
            dump_pattern(logging::out, i, p);

            AbstractState s0 = state_mapper->operator()(g_initial_state_values);
            assert(result.value_table && result.value_table->has_value(s0));
            logging::out << " ~~> estimate(s0) = "
                         << result.value_table->get(s0) << std::endl;
        }
#endif
    }

    const bool additive_patterns = opts.get<bool>("additive");
    statistics_.additive = additive_patterns;

    if (additive_patterns) {
        max_cliques::compute_max_cliques(
            multiplicativity::buildCompatibilityGraphOrthogonality<false>(
                *patterns),
            this->additive_patterns_);
    } else {
        int size = static_cast<int>(database_.size());

        for (PatternID i = 0; i != size; ++i) {
            additive_patterns_.push_back({i});
        }
    }

    // Gather statistics.
    statistics_.init_time = t_init();
    statistics_.num_patterns = database_.size();
    statistics_.num_additive_subcollections = additive_patterns_.size();

    std::size_t total = 0;
    std::size_t largest = 0;

    std::vector<std::size_t> sizes(additive_patterns_.size());
    for (auto& subcollection : additive_patterns_) {
        total += subcollection.size();
        largest = std::max(largest, subcollection.size());
    }

    statistics_.largest_additive_subcollection_size = largest;
    statistics_.average_additive_subcollection_size =
        total / additive_patterns_.size();

    // Dump the initialization statistics
    logging::out << "\nExpected-Cost Pattern Databases Initialization:" << std::endl;
    dump_init_statistics(logging::out);

    const auto val = static_cast<value_type::value_t>(
        ExpectedCostPDBHeuristic::evaluate(g_initial_state()));

    logging::out << "Initial state value estimate: " << val << std::endl;
}

void
ExpectedCostPDBHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
        "patterns", "", "systematic(pattern_max_size=3)");
    parser.add_option<double>("time_limit", "", "0");
    parser.add_option<int>("max_states", "", "-1");
    parser.add_option<bool>("dump_projections", "", "false");
    parser.add_option<bool>("additive", "", "false");
}

EvaluationResult
ExpectedCostPDBHeuristic::evaluate(const GlobalState& state)
{
#ifdef ECPDB_MEASURE_EVALUATE
    utils::Timer t;
#endif

    value_type::value_t result = g_analysis_objective->max();

    if (!database_.empty()) {
        // Get pattern estimates
        std::vector<value_type::value_t> estimates(database_.size());
        for (std::size_t i = 0; i != database_.size(); ++i) {
            estimates[i] = database_[i].lookup(state);
        }

        // Get lowest additive subcollection value
        for (const auto &additive_collection : additive_patterns_) {
            value_type::value_t val = value_type::zero;

            for (PatternID id : additive_collection) {
                val += estimates[id];
            }

            result = std::min(result, val);
        }
    }

    if (!value_type::approx_equal()(result, value_type::zero)) {
        ++statistics_.num_nontrivial_estimates;
    }

    ++statistics_.num_estimates;
    statistics_.highest_estimate =
        std::max(statistics_.highest_estimate, result);
    statistics_.lowest_estimate =
        std::min(statistics_.lowest_estimate, result);
    statistics_.average_estimate =
        (statistics_.average_estimate *
        (statistics_.num_estimates - 1) +
        result) / statistics_.num_estimates;

#ifdef ECPDB_MEASURE_EVALUATE
    statistics_.evaluate_time += t();
#endif

    return {false, result};
}

void ExpectedCostPDBHeuristic::print_statistics() const {
    auto& out = logging::out;

    // Set fp output precision to 5 digits
    out << std::setprecision(4);

    out << "\nExpected-Cost Pattern Databases Statistics:" << std::endl;

    dump_init_statistics(out);

    out << "  Estimate calls: " << statistics_.num_estimates << std::endl;
    out << "  Non-Trivial estimates: " << statistics_.num_nontrivial_estimates
        << std::endl;
    out << "  Average estimate: " << statistics_.average_estimate << std::endl;
    out << "  Lowest estimate: " << statistics_.lowest_estimate << std::endl;
    out << "  Highest estimate: " << statistics_.highest_estimate << std::endl;

    out << "  Initialization time: " << statistics_.init_time << "s"
        << std::endl;
#ifdef ECPDB_MEASURE_EVALUATE
    out << "  Estimate time: " << statistics_.evaluate_time << "s" << std::endl;
#endif

    // Undo for later statistics
    out << std::setprecision(std::numeric_limits<double>::digits10 + 1);
}

static Plugin<GlobalStateEvaluator> _plugin(
    "ecpdb",
    options::parse<GlobalStateEvaluator, ExpectedCostPDBHeuristic>);

} // namespace pdbs
} // namespace probabilistic
