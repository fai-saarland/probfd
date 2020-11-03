#include "probabilistic_canonical_pdb_heuristic.h"

#include "../../../global_state.h"
#include "../../../globals.h"
#include "../../../operator_cost.h"
#include "../../../option_parser.h"
#include "../../../pdbs/pattern_cliques.h"
#include "../../../pdbs/pattern_collection_information.h"
#include "../../../pdbs/pattern_generator.h"
#include "../../../plugin.h"
#include "../../../utils/countdown_timer.h"
#include "../../../utils/timer.h"
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
#include <sstream>
#include <string>

namespace probabilistic {
namespace pdbs {

ProbabilisticCanonicalPDBHeuristic::ProjectionInfo::ProjectionInfo(
    std::shared_ptr<AbstractStateMapper> state_mapper,
    AbstractAnalysisResult& result)
    : state_mapper(state_mapper)
    , values(result.value_table)
    , dead_ends(result.dead_ends)
    , one_states(result.one_states)
{
}

ProbabilisticCanonicalPDBHeuristic::ProbabilisticCanonicalPDBHeuristic(
    const options::Options& opts)
    : initial_state_is_dead_end_(false)
{
    g_log << "Initializing Probabilistic CPDB Heuristic..." << std::endl;
    ::verify_no_axioms_no_conditional_effects();
    utils::Timer t_init;

    std::shared_ptr<::pdbs::PatternCollectionGenerator> patterns_generator =
        opts.get<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
            "patterns");
    ::pdbs::PatternCollectionInformation patterns_info =
        patterns_generator->generate(NORMAL);
    std::shared_ptr<::pdbs::PatternCollection> patterns =
        patterns_info.get_patterns();

    bool max_clique_patterns_enabled =
        opts.get<int>("max_clique_patterns") >= 0;
    unsigned max_clique_patterns = max_clique_patterns_enabled
        ? opts.get<int>("max_clique_patterns")
        : patterns->size();

    const bool countdown_enabled = opts.get<double>("time_limit") > 0;
    const unsigned max_states = opts.get<int>("max_states");
    utils::CountdownTimer countdown(opts.get<double>("time_limit"));
    unsigned states = 0;
    // value_type::value_t ival = g_property->get_optimistic_bound();

    unsigned reachable = 0;
    unsigned dead_ends = 0;
    unsigned one_states = 0;
    unsigned deterministic = 0;
    bool terminate = false;

    for (unsigned i = 0; i < patterns->size(); i++) {
        if (terminate || (countdown_enabled && countdown.is_expired())) {
            break;
        }
        ProbabilisticProjection projection(
            patterns->at(i), ::g_variable_domain);
        auto state_mapper = projection.get_abstract_state_mapper();

        if (max_states - state_mapper->size() < states) {
            terminate = true;
        }
        states += state_mapper->size();

        if (opts.get<bool>("dump_projections")) {
            std::ostringstream path;
            path << "pattern" << i << ".dot";
            const bool print_transition_labels = true;
            const bool print_values = true;
            dump_graphviz(
                &projection,
                g_analysis_objective.get(),
                path.str(),
                print_transition_labels,
                print_values);
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
        {
            const auto& p = patterns->at(i);
            g_debug << "pattern[" << i << "]: vars = [";
            for (unsigned j = 0; j < p.size(); j++) {
                g_debug << (j > 0 ? ", " : "") << p[j];
            }
            g_debug << "] ({";
            for (unsigned j = 0; j < p.size(); j++) {
                g_debug << (j > 0 ? ", " : "") << ::g_fact_names[p[j]][0];
            }
            g_debug << "})" << std::flush;
        }
#endif

        if (initial_state_is_dead_end_) {
            clique_patterns_.clear();
            clique_database_.clear();
            dead_end_database_.clear();
#if defined(NDEBUG)
            g_log << "Pattern [";
            const auto& p = patterns->at(i);
            for (unsigned j = 0; j < p.size(); j++) {
                g_log << (j > 0 ? ", " : "") << p[j];
            }
            g_log << "]";
#endif
            g_log << " identifies initial state as dead-end!" << std::endl;
            delete (result.value_table);
            delete (result.one_states);
            delete (result.dead_ends);
            break;
        }

        if (result.one == result.reachable_states) {
#ifndef NDEBUG
            g_debug << " **trivial projection**"
                    << " ~~> estimate(s0) = " << result.one_state_reward
                    << std::endl;
#endif
            delete (result.value_table);
            delete (result.one_states);
            delete (result.dead_ends);
            continue;
        }

        reachable += result.reachable_states;
        dead_ends += result.dead;
        one_states += result.one;

        one_state_reward_ = result.one_state_reward;

#ifndef NDEBUG
        {
            assert(
                (result.one_states != nullptr && result.one_states->get(s0))
                || result.value_table->has_value(s0));
            g_debug << " ~~> estimate(s0) = "
                    << ((result.one_states != nullptr
                         && result.one_states->get(s0))
                            ? one_state_reward_
                            : result.value_table->get(s0))
                    << std::endl;
        }
#endif

        if (result.one + result.dead == result.reachable_states) {
#ifndef NDEBUG
            g_debug << " **deterministic projection**";
#endif
            ++deterministic;
            delete (result.value_table);
            delete (result.one_states);
            result.value_table = nullptr;
            result.one_states = nullptr;
            dead_end_database_.emplace_back(state_mapper, result);
        } else if (
            max_clique_patterns_enabled
            && max_clique_patterns <= clique_patterns_.size()) {
            dead_end_database_.emplace_back(state_mapper, result);
        } else {
            clique_patterns_.push_back(patterns->at(i));
            clique_database_.emplace_back(state_mapper, result);
        }
    }

    std::cout << "PCPDB pattern initialization completed after " << t_init
              << " [t=" << ::utils::g_timer << "]" << std::endl;
    g_log << "Stored " << (dead_end_database_.size() + clique_database_.size())
          << "/" << patterns->size() << " projections (" << deterministic
          << " are non-probabilistic)" << std::endl;
    g_log << "Abstract states: " << states << " (" << reachable
          << " reachable, " << dead_ends << " dead ends, " << one_states
          << " one states)" << std::endl;
    std::cout << "Deadend Projections: " << dead_end_database_.size()
              << std::endl;
    std::cout << "Clique Projections: " << clique_database_.size() << std::endl;
    std::cout << "Discarded Patterns: "
              << patterns->size()
            - (clique_database_.size() + dead_end_database_.size())
              << std::endl;

    utils::Timer t_init_cliques;

    // Compute cliques
    const bool use_weak = opts.get<bool>("weak");

    if (use_weak) {
        std::cout << "NOTE: Using the weak orthogonality criterion." << std::endl;
        max_cliques::compute_max_cliques(
            multiplicativity::buildCompatibilityGraphWeakOrthogonality(clique_patterns_),
            this->cliques_);
    } else {
        std::cout << "NOTE: Using the orthogonality criterion." << std::endl;
        max_cliques::compute_max_cliques(
            multiplicativity::buildCompatibilityGraphOrthogonality(clique_patterns_),
            this->cliques_);
    }

    std::cout << "Multiplicative PPDBs -- cliques computed in " << t_init_cliques
              << " [t=" << ::utils::g_timer << "]" << std::endl;

    std::cout << "Number of cliques: " << cliques_.size() << std::endl;

    std::size_t total_nodes = 0;

#ifdef DUMP_CLIQUES
    std::cout << "Computed cliques:" << std::endl;

    for (const PatternClique& clique : cliques_) {
        std::cout << "\t"
                  << "[";

        total_nodes += clique.size();

        for (std::size_t j = 0; j != clique.size(); ++j) {
            std::cout << (j > 0 ? ", " : "") << clique[j];
        }

        std::cout << "]" << std::endl;
    }
#else
    for (const PatternClique& clique : cliques_) {
        total_nodes += clique.size();
    }
#endif

    std::cout << "Cliques total pattern count: " << total_nodes << std::endl;

    std::cout << "Initial state value estimate: ";
    if (initial_state_is_dead_end_) {
        std::cout << "dead-end" << std::endl;
    } else {
        // Technically a virtual function call in a constructor, so be verbose.
        value_type::value_t estimate =
            static_cast<value_type::value_t>(
                ProbabilisticCanonicalPDBHeuristic::
                evaluate(g_initial_state())
            );
        std::cout << estimate << std::endl;
    }
}

value_type::value_t
ProbabilisticCanonicalPDBHeuristic::lookup(
    const ProjectionInfo& info,
    const AbstractState& s) const
{
    assert(!info.dead_ends->get(s));
    if (info.one_states == nullptr && info.values == nullptr) {
        return g_analysis_objective->max();
    }
    if (info.one_states != nullptr && info.one_states->get(s)) {
        return one_state_reward_;
    }
    return info.values->get(s);
}

EvaluationResult
ProbabilisticCanonicalPDBHeuristic::evaluate(const GlobalState& state)
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
        const value_type::value_t estimate = lookup(store, x);
        result = std::min(result, estimate);
    }

    std::vector<value_type::value_t> estimates(clique_database_.size());
    for (unsigned i = 0; i != clique_database_.size(); ++i) {
        const ProjectionInfo& store = clique_database_[i];
        const AbstractState x = store.state_mapper->operator()(state);
        if (store.dead_ends->get(x)) {
            return EvaluationResult(true, g_analysis_objective->min());
        }
        estimates[i] = lookup(store, x);
        assert(estimates[i] > 0);
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

static void
dumpPattern(std::ostream& out, const std::vector<int>& pattern)
{
    for (unsigned i = 0; i < pattern.size(); i++) {
        out << (i > 0 ? ", " : "") << pattern[i];
    }
}

void
ProbabilisticCanonicalPDBHeuristic::dumpCliqueWithHValues(
    std::ostream& out,
    const GlobalState& state,
    const std::vector<PatternID>& clique)
{
    out << "Clique : [";

    for (std::size_t i = 0; i != clique.size(); ++i) {
        PatternID id = clique[i];
        auto& store = clique_database_[id];
        const auto& pattern = store.state_mapper->get_variables();
        const AbstractState x = (*store.state_mapper)(state);
        const value_type::value_t proj_est = store.values->get(x);

        out << (i > 0 ? ", " : "");

        out << "{";
        dumpPattern(out, pattern);
        out << "} (h = " << proj_est << ")";
    }

    std::cerr << "]" << std::endl;
}

void
ProbabilisticCanonicalPDBHeuristic::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
        "patterns", "", "systematic(pattern_max_size=2)");
    parser.add_option<bool>("precompute_dead_ends", "", "false");
    parser.add_option<double>("time_limit", "", "0");
    parser.add_option<int>("max_states", "", "-1");
    parser.add_option<bool>("dump_projections", "", "false");
    parser.add_option<bool>("use_dpa", "", "false");
    parser.add_option<int>("max_clique_patterns", "", "-1");
    parser.add_option<bool>("weak", "false");
}

static Plugin<GlobalStateEvaluator> _plugin(
    "ppdb_mul",
    options::parse<GlobalStateEvaluator, ProbabilisticCanonicalPDBHeuristic>);

} // namespace pdbs
} // namespace probabilistic
