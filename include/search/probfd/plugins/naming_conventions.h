#ifndef PROBFD_PLUGINS_NAMING_CONVENTION_H
#define PROBFD_PLUGINS_NAMING_CONVENTION_H

#include <string>

namespace probfd {

template <bool Bisimulation, bool Fret>
std::string add_wrapper_algo_suffix(const std::string& str)
{
    if constexpr (!Bisimulation) {
        if constexpr (!Fret) {
            return str;
        } else {
            return str + "_fret";
        }
    } else {
        if constexpr (!Fret) {
            return str + "_bisim";
        } else {
            return str + "_bisim_fret";
        }
    }
}

template <bool Bisimulation, bool Fret>
std::string add_mdp_type_to_option(const std::string& str)
{
    if constexpr (!Bisimulation) {
        if constexpr (!Fret) {
            return str;
        } else {
            return "q_" + str;
        }
    } else {
        if constexpr (!Fret) {
            return "b_" + str;
        } else {
            return "qb_" + str;
        }
    }
}

template <bool Bisimulation, bool Fret>
std::string add_mdp_type_to_category(const std::string& str)
{
    if constexpr (!Bisimulation) {
        if constexpr (!Fret) {
            return str;
        } else {
            return "Q" + str;
        }
    } else {
        if constexpr (!Fret) {
            return "B" + str;
        } else {
            return "QB" + str;
        }
    }
}

} // namespace probfd

#endif