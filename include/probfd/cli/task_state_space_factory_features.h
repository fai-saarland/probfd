#ifndef PROBFD_CLI_TASK_STATE_SPACE_FACTORY_FEATURES_H
#define PROBFD_CLI_TASK_STATE_SPACE_FACTORY_FEATURES_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli {

void add_task_state_space_factory_features(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif