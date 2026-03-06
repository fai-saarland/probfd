#ifndef PROBFD_CLI_HEURISTICS_TASK_HEURISTIC_FACTORY_CATEGORY_H
#define PROBFD_CLI_HEURISTICS_TASK_HEURISTIC_FACTORY_CATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::heuristics {

void add_task_heuristic_factory_category(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif