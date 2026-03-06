#include "downward/cli/pruning/stubborn_sets_atom_centric_feature.h"

#include "downward/cli/pruning/pruning_method_options.h"

#include "downward/pruning/stubborn_sets_atom_centric.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_function_definition.h"

using namespace std;
using namespace downward::utils;
using namespace downward::stubborn_sets_atom_centric;

using namespace downward::cli;
using namespace language::parser;

namespace {

InternalFunctionDefinitionBase& add_stubborn_sets_atomic_centric_to_namespace(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        downward::PruningMethod,
        StubbornSetsAtomCentric,
        bool,
        AtomSelectionStrategy,
        Verbosity>>(nspace, "atom_centric_stubborn_sets");

    f.document_title("Atom-centric stubborn sets");
    f.document_synopsis(
        "Stubborn sets are a state pruning method which computes a subset "
        "of applicable actions in each state such that completeness and "
        "optimality of the overall search is preserved. Previous stubborn "
        "set "
        "implementations mainly track information about actions. In "
        "contrast, "
        "this implementation focuses on atomic propositions (atoms), which "
        "often speeds up the computation on IPC benchmarks. For details, "
        "see" +
        format_conference_reference(
            {"Gabriele Roeger",
             "Malte Helmert",
             "Jendrik Seipp",
             "Silvan Sievers"},
            "An Atom-Centric Perspective on Stubborn Sets",
            "https://ai.dmi.unibas.ch/papers/roeger-et-al-socs2020.pdf",
            "Proceedings of the 13th Annual Symposium on Combinatorial "
            "Search "
            "(SoCS 2020)",
            "57-65",
            "AAAI Press",
            "2020"));

    f.make_optional_argument_with_default(
        0,
        "use_sibling_shortcut",
        "true",
        "use variable-based marking in addition to atom-based marking");
    f.make_optional_argument_with_default(
        1,
        "atom_selection_strategy",
        "quick_skip",
        "Strategy for selecting unsatisfied atoms from action "
        "preconditions or "
        "the goal atoms. All strategies use the fast_downward strategy for "
        "breaking ties.");
    add_pruning_options_to_feature(f, 2);

    return f;
}

} // namespace

namespace downward::cli::pruning {

void add_stubborn_sets_atom_centric_feature(
    NamespaceLevelDeclarationList& nspace)
{
    insert_enum_declaration<AtomSelectionStrategy>(
        nspace,
        "AtomSelectionStrategy",
        {{"fast_downward",
          "select the atom (v, d) with the variable v that comes first in the "
          "Fast "
          "Downward variable ordering (which is based on the causal graph)"},
         {"quick_skip",
          "if possible, select an unsatisfied atom whose producers are already "
          "marked"},
         {"static_small",
          "select the atom achieved by the fewest number of actions"},
         {"dynamic_small",
          "select the atom achieved by the fewest number of actions that are "
          "not "
          "yet part of the stubborn set"}});

    add_stubborn_sets_atomic_centric_to_namespace(nspace);
}

} // namespace downward::cli::pruning
