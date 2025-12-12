#ifndef PROBFD_CLI_TASK_STATE_SPACE_FACTORY_FEATURES_H
#define PROBFD_CLI_TASK_STATE_SPACE_FACTORY_FEATURES_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli {

void add_task_state_space_factory_features(
    language::plugins::Registry& registry);

}

#endif