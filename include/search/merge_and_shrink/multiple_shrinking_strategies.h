#pragma once

#include "merge_and_shrink/shrink_strategy.h"

#include <memory>
#include <vector>

namespace merge_and_shrink {

class MultipleShrinkingStrategy : public ShrinkStrategy {
public:
    MultipleShrinkingStrategy(
            const options::Options& opts);
    virtual std::string name() const override;
    virtual bool reduce_labels_before_shrinking() const override;
    virtual void
    shrink(Abstraction& abs, int threshold, bool force = false) override;
    virtual void shrink_atomic(Abstraction& abs1) override;
    virtual void
    shrink_before_merge(Abstraction& abs1, Abstraction& abs2) override;
    virtual bool is_goal_leading() const override;
    virtual bool is_backward_pruned() const override;
    virtual bool require_distances() const override;
    virtual const std::vector<bool>& get_caught_labels_set() const override;

private:
    std::vector<std::shared_ptr<ShrinkStrategy>> strategies_;
};

} // namespace merge_and_shrink
