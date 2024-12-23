#ifndef PROBFD_PLUGINS_MULTI_FEATURE_PLUGIN_H
#define PROBFD_PLUGINS_MULTI_FEATURE_PLUGIN_H

#include "downward_plugins/plugins/plugin.h"

namespace probfd_plugins {

template <template <bool, bool> typename T>
class MultiCategoryPlugin {
    static_assert(std::is_base_of_v<
                  downward_plugins::plugins::CategoryPlugin,
                  T<false, false>>);
    static_assert(std::is_base_of_v<
                  downward_plugins::plugins::CategoryPlugin,
                  T<false, true>>);
    static_assert(std::is_base_of_v<
                  downward_plugins::plugins::CategoryPlugin,
                  T<true, false>>);
    static_assert(std::is_base_of_v<
                  downward_plugins::plugins::CategoryPlugin,
                  T<true, true>>);

    T<false, false> category_plugin_1_;
    T<false, true> category_plugin_2_;
    T<true, false> category_plugin_3_;
    T<true, true> category_plugin_4_;
};

template <template <bool> typename T>
class BinaryFeaturePlugin {
    downward_plugins::plugins::FeaturePlugin<T<false>> plugin_1_;
    downward_plugins::plugins::FeaturePlugin<T<true>> plugin_2_;
};

template <template <bool, bool> typename T>
class MultiFeaturePlugin {
    downward_plugins::plugins::FeaturePlugin<T<false, false>> plugin_1_;
    downward_plugins::plugins::FeaturePlugin<T<false, true>> plugin_2_;
    downward_plugins::plugins::FeaturePlugin<T<true, false>> plugin_3_;
    downward_plugins::plugins::FeaturePlugin<T<true, true>> plugin_4_;
};

} // namespace probfd_plugins

#endif