#include "probfd/heuristics/pdbs/pattern_generator.h"

#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

PatternCollectionGenerator::PatternCollectionGenerator(
    const options::Options& opts)
    : log(utils::get_log_from_options(opts))
{
}

PatternCollectionGenerator::PatternCollectionGenerator(
    const utils::LogProxy& log)
    : log(log)
{
}

PatternGenerator::PatternGenerator(const options::Options& opts)
    : log(utils::get_log_from_options(opts))
{
}

PatternGenerator::PatternGenerator(const utils::LogProxy& log)
    : log(log)
{
}

std::shared_ptr<utils::Printable> PatternCollectionGenerator::get_report() const
{
    return nullptr;
};

std::shared_ptr<utils::Printable> PatternGenerator::get_report() const
{
    return nullptr;
};

void add_generator_options_to_parser(options::OptionParser& parser)
{
    utils::add_log_options_to_parser(parser);
}

static PluginTypePlugin<PatternCollectionGenerator> _type_plugin(
    "PPDBPatternCollectionGenerator",
    "Factory for pattern collections and/or corresponding probability-aware "
    "PDBs");

static PluginTypePlugin<PatternGenerator> _type_plugin2(
    "PPDBPatternGenerator",
    "Factory for a pattern and/or the corresponding probability-aware PDB");

} // namespace pdbs
} // namespace heuristics
} // namespace probfd