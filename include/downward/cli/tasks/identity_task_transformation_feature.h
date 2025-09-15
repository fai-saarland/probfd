#ifndef DOWNWARD_CLI_TASKS_IDENTITY_TASK_TRANSFORMATION_FEATURE_H
#define DOWNWARD_CLI_TASKS_IDENTITY_TASK_TRANSFORMATION_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::tasks {

void add_identity_task_transformation_features(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif