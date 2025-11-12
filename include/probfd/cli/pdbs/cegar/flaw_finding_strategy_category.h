#ifndef PROBFD_CLI_PDBS_CEGAR_FLAW_FINDER_STRATEGY_H
#define PROBFD_CLI_PDBS_CEGAR_FLAW_FINDER_STRATEGY_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::pdbs::cegar {

void add_flaw_finding_strategy_category(
    downward::cli::plugins::Registry& raw_registry);

}

#endif