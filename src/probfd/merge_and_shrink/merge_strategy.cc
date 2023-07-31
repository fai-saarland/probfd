#include "../../../include/probfd/merge_and_shrink/merge_strategy.h"

using namespace std;

namespace probfd::merge_and_shrink {
MergeStrategy::MergeStrategy(const FactoredTransitionSystem& fts)
    : fts(fts)
{
}
} // namespace probfd::merge_and_shrink
