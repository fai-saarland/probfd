#ifndef LEGACY_BUDGET_SHRINK_H
#define LEGACY_BUDGET_SHRINK_H

#include "legacy/merge_and_shrink/shrink_strategy.h"

namespace legacy {
namespace merge_and_shrink {

class BudgetShrink : public ShrinkStrategy {
public:
    BudgetShrink(const options::Options& opts);

    virtual std::string name() const override;

    virtual bool reduce_labels_before_shrinking() const override
    {
        return false;
    }

    virtual void shrink(Abstraction& abs, int, bool) override { prune(abs); }

    virtual void shrink_atomic(Abstraction& abs1) override { prune(abs1); }

    virtual void
    shrink_before_merge(Abstraction& abs1, Abstraction& abs2) override
    {
        prune(abs1);
        prune(abs2);
    }

private:
    void prune(Abstraction& abstraction) const;
    const int budget_;
};

} // namespace merge_and_shrink
} // namespace legacy

#endif
