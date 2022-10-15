#ifndef PDBS_TYPES_H
#define PDBS_TYPES_H

#include <memory>
#include <vector>

namespace pdbs {
class PatternDatabase;
using Pattern = std::vector<int>;
using PatternCollection = std::vector<Pattern>;
using PDBCollection = std::vector<std::shared_ptr<PatternDatabase>>;
using PatternID = int;
/* NOTE: pattern cliques are often called maximal additive pattern subsets
   in the literature. A pattern clique is an additive set of patterns,
   represented by their IDs (indices) in a pattern collection. */
using PatternClique = std::vector<PatternID>;

struct FactPair {
    int var;
    int value;
    FactPair(int var, int value) : var(var), value(value) {}
    FactPair(const FactPair&) = default;
    FactPair(FactPair&&) = default;
    FactPair& operator=(FactPair&&) = default;
    FactPair& operator=(const FactPair&) = default;
    bool operator<(const FactPair& other) const { return var < other.var || (var == other.var && value < other.value); }
};
}

#endif
