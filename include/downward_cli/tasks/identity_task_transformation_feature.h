#ifndef DOWNWARD_CLI_TASKS_IDENTITY_TASK_TRANSFORMATION_FEATURE_H
#define DOWNWARD_CLI_TASKS_IDENTITY_TASK_TRANSFORMATION_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::tasks {

void add_identity_task_transformation_features(
    language::plugins::RawRegistry& raw_registry);

}

#endif