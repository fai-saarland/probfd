#ifndef BUDGET_SHRINK_H
#define BUDGET_SHRINK_H

#include "shrink_strategy.h"

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

#endif
