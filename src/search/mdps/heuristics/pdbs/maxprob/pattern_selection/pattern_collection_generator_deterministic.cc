#include "pattern_collection_generator_deterministic.h"

#include "../../../../../option_parser.h"
#include "../../../../../pdbs/pattern_database.h"
#include "../../../../../plugin.h"

#include "../maxprob_projection.h"

using namespace std;

namespace probabilistic {
namespace pdbs {
namespace maxprob {
namespace pattern_selection {

PatternCollectionGeneratorDeterministic::
    PatternCollectionGeneratorDeterministic(
        shared_ptr<::pdbs::PatternCollectionGenerator> gen,
        shared_ptr<multiplicativity::MultiplicativityStrategy> multiplicativity)
    : gen(gen)
    , multiplicativity(multiplicativity)
{
}

PatternCollectionGeneratorDeterministic::
    PatternCollectionGeneratorDeterministic(options::Options& opts)
    : PatternCollectionGeneratorDeterministic(
          opts.get<shared_ptr<::pdbs::PatternCollectionGenerator>>("generator"),
          opts.get<shared_ptr<multiplicativity::MultiplicativityStrategy>>(
              "multiplicativity_strategy"))
{
}

PatternCollectionInformation
PatternCollectionGeneratorDeterministic::generate(OperatorCost cost_type)
{
    ::pdbs::PatternCollectionInformation det_info = gen->generate(cost_type);

    std::shared_ptr<MaxProbPDBCollection> maxprob_pdbs(
        new MaxProbPDBCollection());

    std::shared_ptr patterns = det_info.move_patterns();
    std::shared_ptr pdbs = det_info.move_pdbs();
    std::shared_ptr cliques = det_info.move_pattern_cliques();

    assert(patterns);

    for (size_t i = 0; i != patterns->size(); ++i) {
        const Pattern& pattern = (*patterns)[i];

        if (pdbs) {
            shared_ptr pdb = (*pdbs)[i];
            assert(pdb);
            maxprob_pdbs->emplace_back(new MaxProbProjection(*pdb));
        } else {
            maxprob_pdbs->emplace_back(new MaxProbProjection(pattern));
        }
    }

    PatternCollectionInformation info(patterns, multiplicativity);

    info.set_pdbs(maxprob_pdbs);
    info.set_pattern_cliques(cliques);

    return info;
}

std::shared_ptr<utils::Printable>
PatternCollectionGeneratorDeterministic::get_report() const
{
    return gen->get_report();
}

static shared_ptr<PatternCollectionGenerator> _parse(OptionParser& parser)
{
    if (parser.dry_run()) {
        return nullptr;
    }

    parser.document_synopsis(
        "Pattern Generator Adapter for the Determinization",
        "Generates all the pattern collection according to the underlying "
        "generator for the deterministic problem.");

    parser.add_option<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
        "generator",
        "The underlying pattern generator for the deterministic problem.",
        "systematic()");

    parser.add_option<
        std::shared_ptr<multiplicativity::MultiplicativityStrategy>>(
        "multiplicativity_strategy",
        "The multiplicativity strategy.",
        "multiplicativity_orthogonality");

    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return make_shared<PatternCollectionGeneratorDeterministic>(opts);
}

static Plugin<PatternCollectionGenerator> _plugin("det_adapter_mp", _parse);

} // namespace pattern_selection
} // namespace maxprob
} // namespace pdbs
} // namespace probabilistic