#ifndef PROBFD_CLI_HEURISTICS_TASK_HEURISTIC_FACTORY_CATEGORY_H
#define PROBFD_CLI_HEURISTICS_TASK_HEURISTIC_FACTORY_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::heuristics {

void add_task_heuristic_factory_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif