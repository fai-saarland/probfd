#include "pattern_collection_generator_deterministic.h"

#include "../../../../../option_parser.h"
#include "../../../../../plugin.h"
#include "../../../../../pdbs/pattern_database.h"

#include "../expcost_projection.h"

using namespace std;

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

PatternCollectionGeneratorDeterministic::
PatternCollectionGeneratorDeterministic(
    shared_ptr<::pdbs::PatternCollectionGenerator> gen,
    shared_ptr<additivity::AdditivityStrategy> additivity)
    : gen(gen)
    , additivity(additivity) { }

PatternCollectionGeneratorDeterministic::
PatternCollectionGeneratorDeterministic(options::Options& opts)
    : PatternCollectionGeneratorDeterministic(
        opts.get<shared_ptr<::pdbs::PatternCollectionGenerator>>("generator"),
        opts.get<shared_ptr<additivity::AdditivityStrategy>>(
            "additivity_strategy"))
{

}

PatternCollectionInformation
PatternCollectionGeneratorDeterministic::generate(OperatorCost cost_type)
{
    ::pdbs::PatternCollectionInformation det_info = gen->generate(cost_type);

    std::shared_ptr<ExpCostPDBCollection> expcost_pdbs(new ExpCostPDBCollection());

    for (auto& pdb_ptr : *det_info.get_pdbs()) {
        expcost_pdbs->emplace_back(new ExpCostProjection(*pdb_ptr));
        pdb_ptr.reset();
    }

    PatternCollectionInformation info(det_info.get_patterns(), additivity);
    
    info.set_pdbs(expcost_pdbs);
    info.set_pattern_cliques(det_info.get_pattern_cliques());

    return info;
}

static shared_ptr<PatternCollectionGenerator> _parse(OptionParser &parser) {
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

    parser.add_option<std::shared_ptr<additivity::AdditivityStrategy>>(
        "additivity_strategy",
        "The additivity strategy.",
        "max_orthogonality");

    Options opts = parser.parse();
    if (parser.dry_run())
        return nullptr;

    return make_shared<PatternCollectionGeneratorDeterministic>(opts);
}

static Plugin<PatternCollectionGenerator> _plugin("det_adapter", _parse);

}
}
}