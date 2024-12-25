#include "downward/cli/pdbs/utils.h"

#include "downward/utils/markup.h"

namespace downward::cli::pdbs {

std::string get_rovner_et_al_reference()
{
    return utils::format_conference_reference(
        {"Alexander Rovner", "Silvan Sievers", "Malte Helmert"},
        "Counterexample-Guided Abstraction Refinement for Pattern Selection "
        "in Optimal Classical Planning",
        "https://ai.dmi.unibas.ch/papers/rovner-et-al-icaps2019.pdf",
        "Proceedings of the 29th International Conference on Automated "
        "Planning and Scheduling (ICAPS 2019)",
        "362-367",
        "AAAI Press",
        "2019");
}

} // namespace downward::cli::pdbs
