#include "downward/cli/plugins/plugin.h"

#include "downward/cli/pruning_method_options.h"

#include "downward/pruning/stubborn_sets_atom_centric.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::stubborn_sets_atom_centric;

using namespace downward::cli;
using namespace downward::cli::plugins;

namespace {

class StubbornSetsAtomCentricFeature
    : public TypedFeature<downward::PruningMethod, StubbornSetsAtomCentric> {
public:
    StubbornSetsAtomCentricFeature()
        : TypedFeature("atom_centric_stubborn_sets")
    {
        document_title("Atom-centric stubborn sets");
        document_synopsis(
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

        add_option<bool>(
            "use_sibling_shortcut",
            "use variable-based marking in addition to atom-based marking",
            "true");
        add_option<AtomSelectionStrategy>(
            "atom_selection_strategy",
            "Strategy for selecting unsatisfied atoms from action "
            "preconditions or "
            "the goal atoms. All strategies use the fast_downward strategy for "
            "breaking ties.",
            "quick_skip");
        add_pruning_options_to_feature(*this);
    }

    virtual shared_ptr<StubbornSetsAtomCentric>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<StubbornSetsAtomCentric>(
            opts.get<bool>("use_sibling_shortcut"),
            opts.get<AtomSelectionStrategy>("atom_selection_strategy"),
            get_pruning_arguments_from_options(opts));
    }
};

FeaturePlugin<StubbornSetsAtomCentricFeature> _plugin;

TypedEnumPlugin<AtomSelectionStrategy> _enum_plugin(
    {{"fast_downward",
      "select the atom (v, d) with the variable v that comes first in the Fast "
      "Downward variable ordering (which is based on the causal graph)"},
     {"quick_skip",
      "if possible, select an unsatisfied atom whose producers are already "
      "marked"},
     {"static_small",
      "select the atom achieved by the fewest number of actions"},
     {"dynamic_small",
      "select the atom achieved by the fewest number of actions that are not "
      "yet part of the stubborn set"}});

} // namespace
