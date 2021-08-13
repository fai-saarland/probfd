#include "pattern_collection_generator_deterministic.h"

#include "../../../../option_parser.h"
#include "../../../../options/options.h"
#include "../../../../pdbs/pattern_database.h"
#include "../../../../plugin.h"

#include "../expcost_projection.h"
#include "../maxprob_projection.h"

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <class PDBType>
PatternCollectionGeneratorDeterministic<PDBType>::
    PatternCollectionGeneratorDeterministic(
        std::shared_ptr<::pdbs::PatternCollectionGenerator> gen,
        std::shared_ptr<SubCollectionFinder> finder)
    : gen(gen)
    , finder(finder)
{
}

template <class PDBType>
PatternCollectionGeneratorDeterministic<
    PDBType>::PatternCollectionGeneratorDeterministic(options::Options& opts)
    : PatternCollectionGeneratorDeterministic(
          opts.get<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
              "generator"),
          opts.get<std::shared_ptr<SubCollectionFinder>>(
              "subcollection_finder"))
{
}

template <class PDBType>
PatternCollectionInformation<PDBType>
PatternCollectionGeneratorDeterministic<PDBType>::generate(
    OperatorCost cost_type)
{
    ::pdbs::PatternCollectionInformation det_info = gen->generate(cost_type);

    std::shared_ptr<PPDBCollection<PDBType>> ppdbs(
        new PPDBCollection<PDBType>());

    std::shared_ptr patterns = det_info.move_patterns();
    std::shared_ptr pdbs = det_info.move_pdbs();
    std::shared_ptr subcollections = det_info.move_pattern_cliques();

    assert(patterns);

    for (size_t i = 0; i != patterns->size(); ++i) {
        const Pattern& pattern = (*patterns)[i];

        if (pdbs) {
            std::shared_ptr pdb = (*pdbs)[i];
            assert(pdb);
            ppdbs->emplace_back(new PDBType(*pdb));
        } else {
            ppdbs->emplace_back(new PDBType(pattern));
        }
    }

    PatternCollectionInformation<PDBType> info(patterns, finder);

    info.set_pdbs(ppdbs);
    info.set_subcollections(subcollections);

    return info;
}

template <class PDBType>
std::shared_ptr<utils::Printable>
PatternCollectionGeneratorDeterministic<PDBType>::get_report() const
{
    return gen->get_report();
}

template <class PDBType>
static std::shared_ptr<PatternCollectionGeneratorDeterministic<PDBType>>
_parse(OptionParser& parser)
{
    if (parser.dry_run()) {
        return nullptr;
    }

    parser.document_synopsis(
        "Pattern Generator Adapter for the All Outcomes Determinization",
        "Generates all the pattern collection according to the underlying "
        "generator for the deterministic problem.");

    parser.add_option<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
        "generator",
        "The underlying pattern generator for the deterministic problem.",
        "systematic()");

    parser.add_option<std::shared_ptr<SubCollectionFinder>>(
        "subcollection_finder",
        "The subcollection finder.",
        "finder_max_orthogonality()");

    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return std::make_shared<PatternCollectionGeneratorDeterministic<PDBType>>(
        opts);
}

template class PatternCollectionGeneratorDeterministic<MaxProbProjection>;
template class PatternCollectionGeneratorDeterministic<ExpCostProjection>;

static Plugin<PatternCollectionGenerator<MaxProbProjection>>
    _plugin_mp("det_adapter_mp", _parse<MaxProbProjection>);

static Plugin<PatternCollectionGenerator<ExpCostProjection>>
    _plugin_ec("det_adapter_ec", _parse<ExpCostProjection>);

} // namespace pattern_selection
} // namespace pdbs
} // namespace probabilistic