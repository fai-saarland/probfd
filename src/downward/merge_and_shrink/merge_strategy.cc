#include "downward/merge_and_shrink/merge_strategy.h"

using namespace std;

namespace merge_and_shrink {
MergeStrategy::MergeStrategy(const FactoredTransitionSystem& fts)
    : fts(fts)
{
}
} // namespace merge_and_shrink
