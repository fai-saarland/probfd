#include "downward/pdbs/pattern_generator_cegar.h"

#include "downward/pdbs/cegar.h"
#include "downward/pdbs/utils.h"

#include "downward/state.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include <vector>

using namespace std;

namespace downward::pdbs {
PatternGeneratorCEGAR::PatternGeneratorCEGAR(
    int max_pdb_size,
    double max_time,
    bool use_wildcard_plans,
    int random_seed,
    utils::Verbosity verbosity)
    : PatternGenerator(verbosity)
    , max_pdb_size(max_pdb_size)
    , max_time(max_time)
    , use_wildcard_plans(use_wildcard_plans)
    , rng(utils::get_rng(random_seed))
{
}

string PatternGeneratorCEGAR::name() const
{
    return "CEGAR pattern generator";
}

PatternInformation
PatternGeneratorCEGAR::compute_pattern(const SharedAbstractTask& task)
{
    vector<FactPair> goals =
        get_goals_in_random_order(get_goal(task), *rng);
    return generate_pattern_with_cegar(
        max_pdb_size,
        max_time,
        use_wildcard_plans,
        log,
        rng,
        task,
        goals[0]);
}

} // namespace downward::pdbs
