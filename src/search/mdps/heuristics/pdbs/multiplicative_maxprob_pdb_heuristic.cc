#include "multiplicative_maxprob_pdb_heuristic.h"

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

#include <cassert>
#include <iostream>
#include <string>

namespace probabilistic {
namespace pdbs {

using ::pdbs::PatternCollectionGenerator;
using ::pdbs::PatternCollectionInformation;
using ::pdbs::PatternCollection;
using ::pdbs::Pattern;
using ::pdbs::PatternID;
using ::pdbs::PatternClique;

namespace {
void dump_pattern_vars(std::ostream& out, const Pattern& p) {
    out << "[";
    for (unsigned j = 0; j < p.size(); j++) {
        out << (j > 0 ? ", " : "") << p[j];
    }
    out << "]";
}

void dump_pattern_short(std::ostream& out, PatternID i, const Pattern& p) {
    out << "pattern[" << i << "]: vars = ";
    dump_pattern_vars(out, p);
}

void dump_pattern(std::ostream& out, PatternID i, const Pattern& p) {
    dump_pattern_short(out, i, p);

    out << " ({";
    for (unsigned j = 0; j < p.size(); j++) {
        out << (j > 0 ? ", " : "") << ::g_fact_names[p[j]][0];
    }
    out << "})" << std::flush;
}

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

struct MultiplicativeMaxProbPDBHeuristic::ProjectionInfo {
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

MultiplicativeMaxProbPDBHeuristic::ProjectionInfo::ProjectionInfo(
    std::shared_ptr<AbstractStateMapper> state_mapper,
    AbstractAnalysisResult& result)
    : state_mapper(std::move(state_mapper))
    , values(result.value_table)
    , dead_ends(result.dead_ends)
    , one_states(result.one_states)
{
}

MultiplicativeMaxProbPDBHeuristic::MultiplicativeMaxProbPDBHeuristic(
    const options::Options& opts)
    : weak_orthogonality(opts.get<bool>("weak"))
{
    ::verify_no_axioms_no_conditional_effects();

    g_log << "Initializing Probabilistic CPDB Heuristic..." << std::endl;
    utils::Timer t_init;

    auto patterns_generator =
        opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns");
    auto patterns_info = patterns_generator->generate(NORMAL);
    const auto& patterns = *patterns_info.get_patterns();
    statistics_.total_projections = patterns.size();

    const bool countdown_enabled = opts.get<double>("time_limit") > 0;
    const unsigned max_states = opts.get<int>("max_states");
    utils::CountdownTimer countdown(opts.get<double>("time_limit"));

    bool terminate = false;

    for (unsigned i = 0; i < patterns.size(); i++) {
        const auto& p = patterns[i];

        if (terminate || (countdown_enabled && countdown.is_expired())) {
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
        dump_pattern(g_debug, i, p);
#endif

        if (initial_state_is_dead_end_) {
            clique_patterns_.clear();
            clique_database_.clear();
            dead_end_database_.clear();
#if defined(NDEBUG)
            dump_pattern_short(g_debug, i, p);
#endif
            g_log << " identifies initial state as dead-end!" << std::endl;
            delete (result.value_table);
            delete (result.one_states);
            delete (result.dead_ends);
            break;
        }

        if (result.one == result.reachable_states) {
#ifndef NDEBUG
            g_debug << " **trivial projection** ~~> estimate(s0) = "
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
            g_debug << " ~~> estimate(s0) = "
                    << ((result.one_states != nullptr
                         && result.one_states->get(s0))
                            ? value_type::one : result.value_table->get(s0))
                    << std::endl;
        }
#endif

        if (result.one + result.dead == result.reachable_states) {
#ifndef NDEBUG
            g_debug << " **deterministic projection**";
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

    statistics_.init_time = t_init();

    utils::Timer t_init_cliques;

    // Compute cliques
    if (weak_orthogonality) {
        std::cout << "NOTE: Using the weak orthogonality criterion." << std::endl;
        max_cliques::compute_max_cliques(
            multiplicativity::
            buildCompatibilityGraphWeakOrthogonality(clique_patterns_),
            this->cliques_);
    } else {
        std::cout << "NOTE: Using the orthogonality criterion." << std::endl;
        max_cliques::compute_max_cliques(
            multiplicativity::
            buildCompatibilityGraphOrthogonality(clique_patterns_),
            this->cliques_);
    }

    statistics_.clique_init_time = t_init_cliques();

    dump_construction_statistics();

    auto estimate = static_cast<value_type::value_t>(
        MultiplicativeMaxProbPDBHeuristic::evaluate(g_initial_state()));

    std::cout << "Initial state estimate: " << estimate << std::endl;
}

value_type::value_t
MultiplicativeMaxProbPDBHeuristic::
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
MultiplicativeMaxProbPDBHeuristic::evaluate(const GlobalState& state)
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

    for (const PatternClique& clique : cliques_) {
        value_type::value_t multiplicative = value_type::one;
        for (const PatternID i : clique) {
            multiplicative *= estimates[i];
        }
        result = std::min(result, multiplicative);
    }

    return EvaluationResult(false, result);
}

void MultiplicativeMaxProbPDBHeuristic::dump_construction_statistics() const {
    std::ostream& out = std::cout;

    out << "Multiplicative MaxProb-PDB initialization complete:\n";

    out << "  Criterion: " << (weak_orthogonality ? "Weak" : "")
        << " Orthogonality\n";

    out << "  Initialization time: " << statistics_.init_time << "\n";
    out << "  Clique construction time: " << statistics_.clique_init_time
        << "\n";

    const size_t num_stored_clique = clique_database_.size();
    const size_t num_stored_deadend = dead_end_database_.size();
    const size_t num_stored = num_stored_clique + num_stored_deadend;

    out << "  Total Projections: " << statistics_.total_projections;
    out << "  Stored Projections: " << num_stored;
    out << "  Dead-End Projections: " << num_stored_deadend;
    out << "  Clique Projections: " << num_stored_clique;

    out << "  Abstract states: " << statistics_.abstract_states;
    out << "  Reachable abstract states: " <<
    statistics_.abstract_reachable_states;
    out << "  Abstract dead ends: " << statistics_.abstract_dead_ends;
    out << "  Abstract probability one states: " <<
    statistics_.abstract_one_states;

    std::cout << "  Number of cliques: " << cliques_.size() << std::endl;
}

void MultiplicativeMaxProbPDBHeuristic::dump_statistics() const {
    dump_construction_statistics();
}

void
MultiplicativeMaxProbPDBHeuristic::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
        "patterns", "", "systematic(pattern_max_size=2)");
    parser.add_option<bool>("precompute_dead_ends", "", "false");
    parser.add_option<double>("time_limit", "", "0");
    parser.add_option<int>("max_states", "", "-1");
    parser.add_option<bool>("dump_projections", "", "false");
    parser.add_option<bool>("weak", "", "false");
}

static Plugin<GlobalStateEvaluator> _plugin(
    "ppdb_mul",
    options::parse<GlobalStateEvaluator, MultiplicativeMaxProbPDBHeuristic>);

} // namespace pdbs
} // namespace probabilistic
