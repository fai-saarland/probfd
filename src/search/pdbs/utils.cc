#include "pdbs/utils.h"

#include "pdbs/pattern_collection_information.h"
#include "pdbs/pattern_database.h"
#include "pdbs/pattern_information.h"

#include "utils/logging.h"

#include "globals.h"

using namespace std;

namespace pdbs {
int compute_pdb_size(const Pattern &pattern) {
    int size = 1;
    for (int var : pattern) {
        size *= g_variable_domain[var];
    }
    return size;
}

int compute_total_pdb_size(const PatternCollection &pattern_collection) {
    int size = 0;
    for (const Pattern &pattern : pattern_collection) {
        size += compute_pdb_size(pattern);
    }
    return size;
}

void dump_pattern_generation_statistics(
    const string &identifier,
    utils::Duration runtime,
    const PatternInformation &pattern_info) {
    const Pattern &pattern = pattern_info.get_pattern();
    cout << identifier << " pattern: " << pattern << endl;
    cout << identifier << " number of variables: " << pattern.size() << endl;
    cout << identifier << " PDB size: "
         << compute_pdb_size(pattern) << endl;
    cout << identifier << " computation time: " << runtime << endl;
}

void dump_pattern_collection_generation_statistics(
    const string &identifier,
    utils::Duration runtime,
    const PatternCollectionInformation &pci,
    bool dump_collection) {
    const PatternCollection &pattern_collection = *pci.get_patterns();
    if (dump_collection) {
        cout << identifier << " collection: " << pattern_collection << endl;
    }
    cout << identifier << " number of patterns: " << pattern_collection.size()
         << endl;
    cout << identifier << " total PDB size: "
         << compute_total_pdb_size(pattern_collection)
         << endl;
    cout << identifier << " computation time: " << runtime << endl;
}
}
