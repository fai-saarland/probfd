#include <gtest/gtest.h>

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_mapping.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/fts_factory.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/task_proxy.h"

#include "probfd/utils/json.h"

#include "downward/utils/logging.h"

#include "tests/tasks/blocksworld.h"

#include <format>
#include <fstream>

using namespace probfd;
using namespace probfd::merge_and_shrink;

using namespace tests;

TEST(MnSTests, test_atomic_fts)
{
    BlocksworldTask task(1, {{0}}, {{0}});

    ProbabilisticTaskProxy task_proxy(task);
    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));
    FactoredTransitionSystem fts =
        create_factored_transition_system(task_proxy, false, false, log);

    ASSERT_EQ(fts.get_size(), task.get_num_variables())
        << "Unexpected number of atomic factors!";

    for (int i = 0; i != fts.get_size(); ++i) {
        const auto& ts = fts.get_transition_system(i);
        std::ifstream file(
            std::format("resources/mns_tests/atomic_ts_bw1_{}.json", i));
        const auto expected_ts = json::read<TransitionSystem>(file);
        ASSERT_EQ(ts, expected_ts) << std::format(
            "Transition system {} did not match the expected transition "
            "system!",
            i);
    }
}

TEST(MnSTests, test_atomic_fts2)
{
    BlocksworldTask task(2, {{0, 1}}, {{0, 1}});

    ProbabilisticTaskProxy task_proxy(task);
    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));
    FactoredTransitionSystem fts =
        create_factored_transition_system(task_proxy, false, false, log);

    ASSERT_EQ(fts.get_size(), task.get_num_variables())
        << "Unexpected number of atomic factors!";

    for (int i = 0; i != fts.get_size(); ++i) {
        const auto& ts = fts.get_transition_system(i);
        std::ifstream file(
            std::format("resources/mns_tests/atomic_ts_bw2_{}.json", i));
        const auto expected_ts = json::read<TransitionSystem>(file);
        ASSERT_EQ(ts, expected_ts) << std::format(
            "Transition system {} did not match the expected transition "
            "system!",
            i);
    }
}

TEST(MnSTests, test_atomic_fts3)
{
    BlocksworldTask task(3, {{0, 1, 2}}, {{0, 1, 2}});

    ProbabilisticTaskProxy task_proxy(task);
    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));
    FactoredTransitionSystem fts =
        create_factored_transition_system(task_proxy, false, false, log);

    ASSERT_EQ(fts.get_size(), task.get_num_variables())
        << "Unexpected number of atomic factors!";

    for (int i = 0; i != fts.get_size(); ++i) {
        const auto& ts = fts.get_transition_system(i);
        std::ifstream file(
            std::format("resources/mns_tests/atomic_ts_bw3_{}.json", i));
        const auto expected_ts = json::read<TransitionSystem>(file);
        ASSERT_EQ(ts, expected_ts) << std::format(
            "Transition system {} did not match the expected transition "
            "system!",
            i);
    }
}