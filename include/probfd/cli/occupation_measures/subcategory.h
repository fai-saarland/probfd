#ifndef PROBFD_CLI_OCCUPATION_MEASURES_SUBCATEGORY_H
#define PROBFD_CLI_OCCUPATION_MEASURES_SUBCATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::occupation_measures {

void add_occupation_measure_heuristics_features(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif