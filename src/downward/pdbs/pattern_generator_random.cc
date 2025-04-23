#include "downward/pdbs/pattern_generator_random.h"

#include "downward/pdbs/pattern_database.h"
#include "downward/pdbs/random_pattern.h"
#include "downward/pdbs/utils.h"

#include "downward/fact_pair.h"
#include "downward/state.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include <vector>

using namespace std;

namespace downward::pdbs {
PatternGeneratorRandom::PatternGeneratorRandom(
    int max_pdb_size,
    double max_time,
    bool bidirectional,
    int random_seed,
    utils::Verbosity verbosity)
    : PatternGenerator(verbosity)
    , max_pdb_size(max_pdb_size)
    , max_time(max_time)
    , bidirectional(bidirectional)
    , rng(utils::get_rng(random_seed))
{
}

string PatternGeneratorRandom::name() const
{
    return "random pattern generator";
}

PatternInformation
PatternGeneratorRandom::compute_pattern(const shared_ptr<AbstractTask>& task)
{
    vector<vector<int>> cg_neighbors =
        compute_cg_neighbors(task, bidirectional);
    vector<FactPair> goals = get_goals_in_random_order(*task, *rng);

    Pattern pattern = generate_random_pattern(
        max_pdb_size,
        max_time,
        log,
        rng,
        *task,
        goals[0].var,
        cg_neighbors);

    return PatternInformation(*task, pattern, log);
}

} // namespace pdbs
