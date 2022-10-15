#ifndef MDPS_NEW_STATE_HANDLERS_STORE_PREFERRED_OPERATORS_H
#define MDPS_NEW_STATE_HANDLERS_STORE_PREFERRED_OPERATORS_H

#include "probfd/new_state_handler.h"

#include "probfd/storage/per_state_storage.h"

#include <cassert>
#include <memory>
#include <vector>

class Heuristic;
class GlobalOperator;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
class ProbabilisticOperator;

/// Namespace dedicated to new state listener implementations.
namespace new_state_handlers {

class StorePreferredOperators;

class PrefOpsCacheEntry {
public:
    friend class StorePreferredOperators;

    explicit PrefOpsCacheEntry()
        : ops_(nullptr)
    {
    }

    PrefOpsCacheEntry(const PrefOpsCacheEntry&
#ifndef NDEBUG
                          o
#endif
                      )
        : PrefOpsCacheEntry()
    {
        assert(o.ops_ == nullptr);
    }

    ~PrefOpsCacheEntry()
    {
        if (ops_) {
            delete[](ops_);
        }
    }

    bool contains(unsigned id) const;
    bool contains(const ProbabilisticOperator* ptr) const;

private:
    unsigned* ops_;
};

class StorePreferredOperators : public NewGlobalStateHandler {
public:
    explicit StorePreferredOperators(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

    const PrefOpsCacheEntry& get_cached_ops(const StateID& s);

    virtual void touch(const GlobalState& s) override;
    virtual void touch_goal(const GlobalState& s) override;
    virtual void touch_dead_end(const GlobalState& s) override;

private:
    const bool fetch_only_;
    std::shared_ptr<Heuristic> heuristic_;
    storage::PerStateStorage<PrefOpsCacheEntry> store_;
    std::vector<const GlobalOperator*> pref_;
    std::vector<unsigned> ids_;
    std::vector<unsigned> refs_;
};

} // namespace new_state_handlers
} // namespace probfd

#endif // __STORE_PREFERRED_OPERATORS_H__