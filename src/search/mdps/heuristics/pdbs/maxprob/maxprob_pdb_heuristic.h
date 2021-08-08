#pragma once

#include "../../../../utils/printable.h"
#include "../../../evaluation_result.h"
#include "../../../state_evaluator.h"
#include "../abstract_state.h"
#include "../types.h"
#include "maxprob_projection.h"
#include "multiplicative_mppdbs.h"

#include "pattern_selection/pattern_generator.h"

#include <memory>
#include <ostream>
#include <vector>

class GlobalState;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace pdbs {
namespace maxprob {

/**
 * @brief Additive Expected-Cost PDB heuristic.
 */
class MaxProbPDBHeuristic : public GlobalStateEvaluator {
    struct Statistics;

    std::unique_ptr<Statistics> statistics_;
    std::shared_ptr<utils::Printable> generator_report;

    MultiplicativeMaxProbPDBs multiplicative_mppdbs;

public:
    /**
     * @brief Construct from options.
     *
     * @param opts - The following options are available:
     * + \em patterns - The generator used to generate the initial pattern
     * collection. By default, uses a systematic pattern generation algorithm
     * with size bound 2.
     * + \em max_time_dominance_pruning - The maximum time allowed for
     * dominance pruning. A value of zero disables dominance pruning. By
     * default, this option is disabled.
     * + \em time_limit - The maximal time allowed to construct the databases
     * for the generated pattern collection. A value of zero disables the time
     * limit. by default, no time limit is imposed.
     * + \em max_states - The maximal number of abstract states allowed. By
     * default, no restrictions are imposed.
     * + \em dump_projections - If true, dump the projection with graphviz.
     * False by default.
     * + \em additive - Specifies whether the additivity criterion should be
     * used. True by default.
     */
    explicit MaxProbPDBHeuristic(const options::Options& opts);

    MaxProbPDBHeuristic(
        std::shared_ptr<pattern_selection::PatternCollectionGenerator>
            generator,
        double max_time_dominance_pruning);

    ~MaxProbPDBHeuristic();

    static void add_options_to_parser(options::OptionParser& parser);

    void print_statistics() const override;

protected:
    EvaluationResult evaluate(const GlobalState& state) override;

private:
    MultiplicativeMaxProbPDBs get_multiplicative_mppdbs_from_options(
        std::shared_ptr<pattern_selection::PatternCollectionGenerator>
            generator,
        double max_time_dominance_pruning);
};

} // namespace maxprob
} // namespace pdbs
} // namespace probabilistic
