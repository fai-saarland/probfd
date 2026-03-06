#ifndef PROBFD_CLI_OCCUPATION_MEASURES_CONSTRAINT_GENERATOR_FACTORY_CATEGORY_H
#define PROBFD_CLI_OCCUPATION_MEASURES_CONSTRAINT_GENERATOR_FACTORY_CATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::occupation_measures {

void add_constraint_generator_factory_category(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif