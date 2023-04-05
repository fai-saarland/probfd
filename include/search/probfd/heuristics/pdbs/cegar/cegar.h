#ifndef PDBS_CEGAR_H
#define PDBS_CEGAR_H

#include "pdbs/pattern_collection_information.h"
#include "pdbs/pattern_database.h"
#include "pdbs/pattern_information.h"

#include <memory>
#include <vector>

namespace options {
class OptionParser;
}

namespace utils {
enum class Verbosity;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

extern PatternCollectionInformation generate_pattern_collection_with_cegar(
    int max_pdb_size,
    int max_collection_size,
    double max_time,
    bool use_wildcard_plans,
    utils::Verbosity verbosity,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    const std::shared_ptr<AbstractTask>& task,
    const std::vector<FactPair>& goals,
    std::unordered_set<int>&& blacklisted_variables =
        std::unordered_set<int>());

extern PatternInformation generate_pattern_with_cegar(
    int max_pdb_size,
    double max_time,
    bool use_wildcard_plans,
    utils::Verbosity verbosity,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    const std::shared_ptr<AbstractTask>& task,
    const FactPair& goal,
    std::unordered_set<int>&& blacklisted_variables =
        std::unordered_set<int>());

extern void add_cegar_wildcard_option_to_parser(options::OptionParser& parser);

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif
