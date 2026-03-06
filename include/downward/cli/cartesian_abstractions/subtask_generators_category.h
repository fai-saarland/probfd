#ifndef DOWNWARD_SUBTASK_GENERATORS_CATEGORY_H
#define DOWNWARD_SUBTASK_GENERATORS_CATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli::cartesian_abstractions {

void add_subtask_generator_category(
    language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace downward::cli::cartesian_abstractions

#endif
