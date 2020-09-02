#pragma once

#include "../algorithms/segmented_vector.h"
#include "../operator_cost.h"
#include "../utils/timer.h"
#include "algorithms/types_common.h"
#include "distribution.h"
#include "interfaces/i_printable.h"

#include <iostream>
#include <memory>
#include <vector>

class GlobalState;
class Heuristic;
class StateRegistry;

namespace probabilistic {

class ProbabilisticOperator;

namespace algorithms {

template<>
class TransitionGenerator<GlobalState, const ProbabilisticOperator*> {
public:
    struct Statistics : public IPrintable {
        unsigned long long calls;
        unsigned long long transitions;
#if defined(EXPENSIVE_STATISTICS)
        utils::Timer time;
#endif
        Statistics()
            : calls(0)
            , transitions(0)
        {
#if defined(EXPENSIVE_STATISTICS)
            time.stop();
#endif
        }
        virtual void print(std::ostream& out) const override;
    };

    TransitionGenerator(
        StateRegistry* state_registry,
        const std::vector<ProbabilisticOperator>& ops,
        const std::vector<std::shared_ptr<Heuristic>>&
            path_dependent_heuristics,
        bool cache_transitions);

    TransitionGenerator(
        StateRegistry* state_registry,
        const std::vector<ProbabilisticOperator>& ops,
        const std::vector<std::shared_ptr<Heuristic>>&
            path_dependent_heuristics,
        OperatorCost cost_type,
        int fh_var,
        bool cache_transitions);

    ~TransitionGenerator() = default;

    void notify_applicable_actions(
        const GlobalState& s,
        const std::vector<const ProbabilisticOperator*>& aops);

    Distribution<GlobalState>
    operator()(const GlobalState& state, const ProbabilisticOperator* op);

    const IPrintable& get_statistics() const { return statistics_; }

private:
    const std::vector<std::shared_ptr<Heuristic>> notify_;
    const bool cache_transitions_;
    const bool is_fh_;
    const int fh_var_;
    const OperatorCost cost_type_;

    StateRegistry* state_registry_;

    Statistics statistics_;

    struct CacheEntry {
        explicit CacheEntry()
            : naops(0)
            , aops(nullptr)
            , succs(nullptr)
            , probs(nullptr)
        {
        }
        CacheEntry(CacheEntry&& e)
            : naops(std::move(e.naops))
            , aops(std::move(e.aops))
            , succs(std::move(succs))
            , probs(std::move(e.probs))
        {
            e.naops = 0;
        }
#if !defined(NDEBUG)
        CacheEntry(const CacheEntry& e)
            : CacheEntry()
        {
            assert(e.naops == 0);
        }
#else
        CacheEntry(const CacheEntry&)
            : CacheEntry()
        {
        }
#endif
        ~CacheEntry()
        {
            if (naops && aops[0] > 0) {
                delete[](succs);
                delete[](probs);
            }
            if (naops) {
                delete[](aops);
            }
            naops = 0;
        }

        unsigned naops;
        uint32_t* aops;
        uint32_t* succs;
        value_type::value_t* probs;

        static constexpr const uint32_t SPLIT = 16;
        static constexpr const uint32_t MASK = ((uint32_t)-1) >> SPLIT;
    };

    Distribution<GlobalState> compute_transition(
        const GlobalState& state,
        const ProbabilisticOperator* op);

    // using CacheEntry = std::vector<std::pair<unsigned,
    // Distribution<uint32_t>>>;
    const ProbabilisticOperator* op_ptr_;
    segmented_vector::DynamicSegmentedVector<CacheEntry> cache_;
    CacheEntry* last_entry_;
    StateID last_access_;
};

} // namespace algorithms

using TransitionGenerator =
    algorithms::TransitionGenerator<GlobalState, const ProbabilisticOperator*>;

} // namespace probabilistic
