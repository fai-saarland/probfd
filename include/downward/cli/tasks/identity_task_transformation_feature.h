#ifndef DOWNWARD_CLI_TASKS_IDENTITY_TASK_TRANSFORMATION_FEATURE_H
#define DOWNWARD_CLI_TASKS_IDENTITY_TASK_TRANSFORMATION_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::tasks {

language::plugins::InternalFunctionDefinitionBase&
add_identity_task_transformation_features(language::plugins::Namespace& nspace);

}

#endif