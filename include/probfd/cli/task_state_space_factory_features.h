#ifndef PROBFD_CLI_TASK_STATE_SPACE_FACTORY_FEATURES_H
#define PROBFD_CLI_TASK_STATE_SPACE_FACTORY_FEATURES_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli {

void add_task_state_space_factory_features(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif