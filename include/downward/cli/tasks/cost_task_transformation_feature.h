#ifndef DOWNWARD_CLI_TASKS_COST_TASK_TRANSFORMATION_FEATURE_H
#define DOWNWARD_CLI_TASKS_COST_TASK_TRANSFORMATION_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace downward::cli::tasks {

language::plugins::InternalFunctionDefinitionBase&
add_cost_task_transformation_features(language::plugins::Namespace& nspace);

}

#endif