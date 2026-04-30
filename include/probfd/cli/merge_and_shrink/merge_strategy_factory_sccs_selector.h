#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_SELECTOR_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_SELECTOR_H

namespace language::plugins {
class Namespace;
}

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_sccs_selector_feature(
    language::plugins::Namespace& nspace);

}

#endif