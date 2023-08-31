#ifndef PROBFD_PLUGINS_MULTI_FEATURE_PLUGIN_H
#define PROBFD_PLUGINS_MULTI_FEATURE_PLUGIN_H

#include "downward/plugins/plugin.h"

namespace probfd {

template <template <bool, bool> typename T>
class MultiCategoryPlugin {
    static_assert(std::is_base_of_v<plugins::CategoryPlugin, T<false, false>>);
    static_assert(std::is_base_of_v<plugins::CategoryPlugin, T<false, true>>);
    static_assert(std::is_base_of_v<plugins::CategoryPlugin, T<true, false>>);
    static_assert(std::is_base_of_v<plugins::CategoryPlugin, T<true, true>>);

    T<false, false> category_plugin1;
    T<false, true> category_plugin2;
    T<true, false> category_plugin3;
    T<true, true> category_plugin4;
};

template <template <bool> typename T>
class BinaryFeaturePlugin {
    plugins::FeaturePlugin<T<false>> plugin1;
    plugins::FeaturePlugin<T<true>> plugin2;
};

template <template <bool, bool> typename T>
class MultiFeaturePlugin {
    plugins::FeaturePlugin<T<false, false>> plugin1;
    plugins::FeaturePlugin<T<false, true>> plugin2;
    plugins::FeaturePlugin<T<true, false>> plugin3;
    plugins::FeaturePlugin<T<true, true>> plugin4;
};

} // namespace probfd

#endif