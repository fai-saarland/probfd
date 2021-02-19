#include "maxprob_pdb_heuristic.h"

#include "../../../globals.h"
#include "../../../operator_cost.h"
#include "../../../option_parser.h"
#include "../../../pdbs/pattern_collection_information.h"
#include "../../../pdbs/pattern_generator.h"
#include "../../../plugin.h"
#include "../../../utils/countdown_timer.h"
#include "../../analysis_objective.h"
#include "../../globals.h"
#include "../../logging.h"
#include "../algorithms/max_cliques.h"
#include "multiplicativity.h"
#include "probabilistic_projection.h"
#include "qualitative_result_store.h"
#include "quantitative_result_store.h"
#include "utils.h"

#include <cassert>
#include <iostream>
#include <string>

namespace probabilistic {
namespace pdbs {

using ::pdbs::PatternCollectionGenerator;
using ::pdbs::PatternCollectionInformation;

namespace {
void dump_projection(std::size_t i, ProbabilisticProjection& projection) {
    const bool print_transition_labels = true;
    const bool print_values = true;

    std::ostringstream path;
    path << "pattern" << i << ".dot";

    dump_graphviz(
        &projection,
        g_analysis_objective.get(),
        path.str(),
        print_transition_labels,
        print_values);
}
}

struct MaxProbPDBHeuristic::ProjectionInfo {
    ProjectionInfo(
        std::shared_ptr<AbstractStateMapper> state_mapper,
        AbstractAnalysisResult& result);

    std::shared_ptr<AbstractStateMapper> state_mapper;
    std::unique_ptr<QuantitativeResultStore> values;
    std::unique_ptr<QualitativeResultStore> dead_ends;
    std::unique_ptr<QualitativeResultStore> one_states;

    [[nodiscard]]
    value_type::value_t lookup(const AbstractState& s) const;
};

MaxProbPDBHeuristic::ProjectionInfo::ProjectionInfo(
    std::shared_ptr<AbstractStateMapper> state_mapper,
    AbstractAnalysisResult& result)
    : state_mapper(std::move(state_mapper))
    , values(result.value_table)
    , dead_ends(result.dead_ends)
    , one_states(result.one_states)
{
}

MaxProbPDBHeuristic::MaxProbPDBHeuristic(
    const options::Options& opts)
{
    ::verify_no_axioms_no_conditional_effects();

    // 1. Construct pattern collection
    auto patterns = construct_patterns(opts);

    // 2. Construct database
    auto clique_patterns = construct_database(opts, *patterns);

    // 3. Construct multiplicative cliques
    construct_cliques(opts, clique_patterns);

    // Statistics.
    statistics_.construction_time =
        statistics_.pattern_construction_time +
        statistics_.database_construction_time +
        statistics_.clique_computation_time;

    statistics_.constructed_patterns = patterns->size();
    statistics_.dead_end_patterns = dead_end_database_.size();
    statistics_.clique_patterns = clique_database_.size();
    statistics_.discarded_patterns =
        statistics_.constructed_patterns -
        statistics_.dead_end_patterns -
        statistics_.clique_patterns;

    std::size_t total = 0;
    std::size_t largest = 0;

    for (auto& subcollection : multiplicative_subcollections) {
        total += subcollection.size();
        largest = std::max(largest, subcollection.size());
    }

    statistics_.num_multiplicative_subcollections =
        multiplicative_subcollections.size();
    statistics_.largest_multiplicative_subcollection_size = largest;
    statistics_.average_multiplicative_subcollection_size =
        (double) total / multiplicative_subcollections.size();

    std::cout << "\nMaxProb Pattern Databases Initialization:" << std::endl;
    dump_construction_statistics(std::cout);
}

value_type::value_t
MaxProbPDBHeuristic::
ProjectionInfo::lookup(const AbstractState& s) const
{
    assert(!dead_ends->get(s));
    if (one_states == nullptr && values == nullptr) {
        return g_analysis_objective->max();
    }

    if (one_states != nullptr && one_states->get(s)) {
        return value_type::one;
    }

    return values->get(s);
}

EvaluationResult
MaxProbPDBHeuristic::evaluate(const GlobalState& state)
{
    if (initial_state_is_dead_end_) {
        return EvaluationResult(true, g_analysis_objective->min());
    }

    value_type::value_t result = g_analysis_objective->max();
    for (const ProjectionInfo& store : dead_end_database_) {
        const AbstractState x = store.state_mapper->operator()(state);
        if (store.dead_ends->get(x)) {
            return EvaluationResult(true, g_analysis_objective->min());
        }
        result = std::min(result, store.lookup(x));
    }

    std::vector<value_type::value_t> estimates(clique_database_.size());
    for (unsigned i = 0; i != clique_database_.size(); ++i) {
        const ProjectionInfo& store = clique_database_[i];
        const AbstractState x = store.state_mapper->operator()(state);
        if (store.dead_ends->get(x)) {
            return EvaluationResult(true, g_analysis_objective->min());
        }
        estimates[i] = store.lookup(x);
    }

    for (const PatternClique& clique : multiplicative_subcollections) {
        value_type::value_t multiplicative = value_type::one;
        for (const PatternID i : clique) {
            multiplicative *= estimates[i];
        }
        result = std::min(result, multiplicative);
    }

    return EvaluationResult(false, result);
}

static const char* const as_string[] =
    {"None", "Orthogonality", "Weak Orthogonality"};

void MaxProbPDBHeuristic::dump_construction_statistics(std::ostream& out) const {
    out << "  Multiplicativity: "
        << as_string[statistics_.multiplicativity]<< "\n";

    out << "  Constructed Projections: "
        << statistics_.constructed_patterns<< "\n";
    out << "  Discarded Projections: "
        << statistics_.discarded_patterns << "\n";
    out << "  Dead-End Projections: " << statistics_.dead_end_patterns << "\n";
    out << "  Clique Projections: " << statistics_.clique_patterns << "\n";

    out << "  Abstract states: " << statistics_.abstract_states << "\n";
    out << "  Reachable abstract states: "
        << statistics_.abstract_reachable_states << "\n";
    out << "  Abstract dead ends: " << statistics_.abstract_dead_ends << "\n";
    out << "  Abstract probability one states: "
        << statistics_.abstract_one_states << "\n";

    out << "  Pattern construction time: "
        << statistics_.pattern_construction_time << "s" << "\n";
    out << "  Database construction time: "
        << statistics_.database_construction_time << "s" << "\n";
    out << "  Clique construction time: "
        << statistics_.clique_computation_time << "s" << "\n";
    out << "  Total construction time: "
        << statistics_.construction_time << "s" << "\n";

    out << "  Multiplicative sub-collections: "
        << statistics_.num_multiplicative_subcollections << std::endl;
    out << "  Largest multiplicative sub-collection size: "
        << statistics_.largest_multiplicative_subcollection_size << std::endl;
    out << "  Average multiplicative sub-collection size: "
        << statistics_.average_multiplicative_subcollection_size << std::endl;
}

void MaxProbPDBHeuristic::dump_statistics() const {
    std::cout << "\nMaxProb Pattern Databases Statistics:" << std::endl;
    dump_construction_statistics(std::cout);
}

std::shared_ptr<PatternCollection>
MaxProbPDBHeuristic::construct_patterns(const options::Options& opts) {
    utils::Timer t;

    auto patterns_generator =
        opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns");
    auto patterns_info = patterns_generator->generate(NORMAL);
    auto patterns = patterns_info.get_patterns();

    statistics_.pattern_construction_time = t();

    return patterns;
}

std::vector<Pattern> MaxProbPDBHeuristic::construct_database(
    const options::Options& opts,
    const PatternCollection& patterns)
{
    utils::Timer t;

    std::vector<Pattern> clique_patterns_;

    const bool countdown_enabled = opts.get<double>("time_limit") > 0;
    const unsigned max_states = opts.get<int>("max_states");
    utils::CountdownTimer countdown(opts.get<double>("time_limit"));

    bool terminate = false;

    for (unsigned i = 0; i < patterns.size() && !terminate; ++i) {
        const Pattern& p = patterns[i];

        if (countdown_enabled && countdown.is_expired()) {
            break;
        }

        ProbabilisticProjection projection(p, ::g_variable_domain);
        auto state_mapper = projection.get_abstract_state_mapper();

        if (max_states - state_mapper->size() < statistics_.abstract_states) {
            terminate = true;
        }

        statistics_.abstract_states += state_mapper->size();

        if (opts.get<bool>("dump_projections")) {
            dump_projection(i, projection);
        }

        AbstractState s0 = state_mapper->operator()(g_initial_state_values);
        AbstractAnalysisResult result;

        if (opts.get<bool>("precompute_dead_ends")) {
            QualitativeResultStore dead_ends = projection.compute_dead_ends();
            initial_state_is_dead_end_ = dead_ends.get(s0);
            if (!initial_state_is_dead_end_) {
                result = compute_value_table(
                    &projection, g_analysis_objective.get(), &dead_ends);
                result.dead_ends =
                    new QualitativeResultStore(std::move(dead_ends));
            }
        } else {
            result =
                compute_value_table(&projection, g_analysis_objective.get());
            initial_state_is_dead_end_ = result.dead_ends->get(s0);
        }

#ifndef NDEBUG
        dump_pattern(logging::out, i, p);
#endif

        if (initial_state_is_dead_end_) {
            clique_patterns_.clear();
            clique_database_.clear();
            dead_end_database_.clear();
#if defined(NDEBUG)
            dump_pattern_short(logging::out, i, p);
#endif
            logging::out << " identifies initial state as dead-end!"
                         << std::endl;
            delete (result.value_table);
            delete (result.one_states);
            delete (result.dead_ends);
            break;
        }

        if (result.one == result.reachable_states) {
#ifndef NDEBUG
            logging::out << " **trivial projection** ~~> estimate(s0) = "
                         << result.one_state_reward << std::endl;
#endif
            delete (result.value_table);
            delete (result.one_states);
            delete (result.dead_ends);
            continue;
        }

        statistics_.abstract_reachable_states += result.reachable_states;
        statistics_.abstract_dead_ends += result.dead;
        statistics_.abstract_one_states += result.one;

#ifndef NDEBUG
        {
            assert(
                (result.one_states != nullptr && result.one_states->get(s0))
                || result.value_table->has_value(s0));
            logging::out << " ~~> estimate(s0) = "
                         << (result.one_states != nullptr &&
                             result.one_states->get(s0)
                            ? value_type::one
                            : result.value_table->get(s0))
                         << std::endl;
        }
#endif

        if (result.one + result.dead == result.reachable_states) {
#ifndef NDEBUG
            logging::out << " **deterministic projection**";
#endif
            delete (result.value_table);
            delete (result.one_states);
            result.value_table = nullptr;
            result.one_states = nullptr;
            dead_end_database_.emplace_back(state_mapper, result);
        } else {
            clique_patterns_.push_back(p);
            clique_database_.emplace_back(state_mapper, result);
        }
    }

    statistics_.database_construction_time = t();

    return clique_patterns_;
}

void MaxProbPDBHeuristic::construct_cliques(
    const options::Options& opts,
    std::vector<Pattern>& clique_patterns)
{
    utils::Timer t;

    const int m_type = opts.get_enum("multiplicativity");
    statistics_.multiplicativity = (Multiplicativity) m_type;

    switch (m_type) {
        default:
        case NONE: {
            const int size = (int) clique_patterns.size();

            for (int i = 0; i < size; ++i) {
                multiplicative_subcollections.push_back({i});
            }
            break;
        }
        case ORTHOGONALITY:
            max_cliques::compute_max_cliques(
                multiplicativity::
                buildCompatibilityGraphOrthogonality(clique_patterns),
                multiplicative_subcollections);
            break;
        case WEAK_ORTHOGONALITY:
            max_cliques::compute_max_cliques(
                multiplicativity::
                buildCompatibilityGraphWeakOrthogonality(clique_patterns),
                multiplicative_subcollections);
            break;
    }

    statistics_.clique_computation_time = t();
}

void
MaxProbPDBHeuristic::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
        "patterns", "", "systematic(pattern_max_size=2)");
    parser.add_option<bool>("precompute_dead_ends", "", "false");
    parser.add_option<double>("time_limit", "", "0");
    parser.add_option<int>("max_states", "", "-1");
    parser.add_option<bool>("dump_projections", "", "false");
    parser.add_enum_option(
        "multiplicativity",
        {"none", "orthogonality", "weak_orthogonality"},
        "",
        "none");
}

static Plugin<GlobalStateEvaluator> _plugin(
    "maxprob_pdb",
    options::parse<GlobalStateEvaluator, MaxProbPDBHeuristic>);

} // namespace pdbs
} // namespace probabilistic
