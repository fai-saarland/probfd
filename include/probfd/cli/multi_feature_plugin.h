#ifndef CLI_MULTI_FEATURE_PLUGIN_H
#define CLI_MULTI_FEATURE_PLUGIN_H

#include "downward/cli/plugins/plugin.h"

namespace probfd::cli {

template <typename T>
static constexpr bool is_category_plugin =
    std::is_base_of_v<downward::cli::plugins::CategoryPlugin, T>;

template <template <bool...> typename T, bool... b>
concept instantiable = requires { typename T<b...>; };

template <template <bool...> typename T, bool... b>
struct suffix_instantiable_h : std::false_type {};

template <template <bool...> typename T, bool b, bool... tail>
struct suffix_instantiable_h<T, b, tail...>
    : std::bool_constant<
          instantiable<T, b, tail...> ||
          suffix_instantiable_h<T, tail...>::value> {};

template <template <bool...> typename T, bool... b>
concept suffix_instantiable = suffix_instantiable_h<T, b...>::value;

template <template <bool...> typename T, bool... b>
concept strict_suffix_instantiable =
    suffix_instantiable<T, b...> && !instantiable<T, b...>;

template <template <bool...> typename T, bool... b>
concept partial_specialization = !strict_suffix_instantiable<T, b...>;

template <template <bool...> typename T, bool... b>
    requires partial_specialization<T, b...>
struct MultiCategoryPlugin;

template <template <bool...> typename T, bool... b>
    requires partial_specialization<T, b...> && instantiable<T, b...> &&
             is_category_plugin<T<b...>>
struct MultiCategoryPlugin<T, b...> {
    T<b...> plugin_;
};

template <template <bool...> typename T, bool... b>
    requires partial_specialization<T, b...> && (!instantiable<T, b...>)
struct MultiCategoryPlugin<T, b...>
    : MultiCategoryPlugin<T, b..., true>
    , MultiCategoryPlugin<T, b..., false> {};

template <template <bool...> typename T, bool... b>
    requires partial_specialization<T, b...>
struct MultiFeaturePlugin;

template <template <bool...> typename T, bool... b>
    requires partial_specialization<T, b...> && instantiable<T, b...>
struct MultiFeaturePlugin<T, b...> {
    downward::cli::plugins::FeaturePlugin<T<b...>> plugin_;
};

template <template <bool...> typename T, bool... b>
    requires partial_specialization<T, b...> && (!instantiable<T, b...>)
struct MultiFeaturePlugin<T, b...>
    : MultiFeaturePlugin<T, b..., true>
    , MultiFeaturePlugin<T, b..., false> {};

} // namespace probfd::cli

#endif // CLI_MULTI_FEATURE_PLUGIN_H
