#include "downward/cli/plugins/plugin.h"

#include "downward/cli/potentials/potential_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/potentials/diverse_potential_heuristics.h"
#include "downward/potentials/potential_max_heuristic.h"
#include "downward/potentials/potential_function.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace potentials;

using namespace downward::cli::plugins;
using namespace downward::cli::potentials;
using namespace downward::cli::utils;

namespace {

class DiversePotentialMaxHeuristicFeature
    : public TypedFeature<Evaluator, PotentialMaxHeuristic> {
public:
    DiversePotentialMaxHeuristicFeature()
        : TypedFeature("diverse_potentials")
    {
        document_subcategory("heuristics_potentials");
        document_title("Diverse potential heuristics");
        document_synopsis(get_admissible_potentials_reference());

        add_option<int>(
            "num_samples",
            "Number of states to sample",
            "1000",
            Bounds("0", "infinity"));
        add_option<int>(
            "max_num_heuristics",
            "maximum number of potential heuristics",
            "infinity",
            Bounds("0", "infinity"));
        add_admissible_potentials_options_to_feature(
            *this,
            "diverse_potentials");
        add_rng_options_to_feature(*this);
    }

    virtual shared_ptr<PotentialMaxHeuristic>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared<PotentialMaxHeuristic>(
            DiversePotentialHeuristics(
                opts.get<int>("num_samples"),
                opts.get<int>("max_num_heuristics"),
                opts.get<double>("max_potential"),
                opts.get<lp::LPSolverType>("lpsolver"),
                opts.get<shared_ptr<AbstractTask>>("transform"),
                opts.get<int>("random_seed"),
                opts.get<utils::Verbosity>("verbosity"))
                .find_functions(),
            opts.get<shared_ptr<AbstractTask>>("transform"),
            opts.get<bool>("cache_estimates"),
            opts.get<string>("description"),
            opts.get<utils::Verbosity>("verbosity"));
    }
};

FeaturePlugin<DiversePotentialMaxHeuristicFeature> _plugin;

} // namespace
