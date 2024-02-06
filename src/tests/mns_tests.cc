#include <gtest/gtest.h>

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/fts_factory.h"
#include "probfd/merge_and_shrink/merge_and_shrink_representation.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include "tests/tasks/blocksworld.h"

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

    ASSERT_EQ(fts.get_size(), 3) << "Unexpected number of atomic factors!";

    auto& ts0 = fts.get_transition_system(0);
    auto& ts1 = fts.get_transition_system(1);
    auto& ts2 = fts.get_transition_system(2);

    ASSERT_EQ(ts0.get_size(), 2) << "Unexpected size of transitions system 0!";
    ASSERT_EQ(ts1.get_size(), 3) << "Unexpected size of transitions system 1!";
    ASSERT_EQ(ts2.get_size(), 2) << "Unexpected size of transitions system 2!";

    ASSERT_EQ(ts0.get_incorporated_variables(), std::vector{0});
    ASSERT_EQ(ts1.get_incorporated_variables(), std::vector{1});
    ASSERT_EQ(ts2.get_incorporated_variables(), std::vector{2});

    ASSERT_EQ(ts0.get_init_state(), 1);
    ASSERT_EQ(ts1.get_init_state(), 1);
    ASSERT_EQ(ts2.get_init_state(), 1);

    ASSERT_TRUE(ts0.is_goal_state(0));
    ASSERT_TRUE(ts0.is_goal_state(1));
    ASSERT_TRUE(!ts1.is_goal_state(0));
    ASSERT_TRUE(ts1.is_goal_state(1));
    ASSERT_TRUE(!ts1.is_goal_state(2));
    ASSERT_TRUE(ts2.is_goal_state(0));
    ASSERT_TRUE(ts2.is_goal_state(1));

    std::ranges::input_range auto label_infos0 = ts0.label_infos().base();
    std::ranges::input_range auto label_infos1 = ts1.label_infos().base();
    std::ranges::input_range auto label_infos2 = ts2.label_infos().base();

    ASSERT_EQ(label_infos0[0].get_cost(), 1);
    ASSERT_EQ(
        label_infos0[0].get_probabilities(),
        (std::vector{0.75_vt, 0.25_vt}));
    ASSERT_EQ(label_infos0[0].get_label_group(), std::vector{0});
    ASSERT_EQ(label_infos0[0].get_transitions().size(), 1);
    ASSERT_EQ(label_infos0[0].get_transitions()[0].src, 1);
    ASSERT_EQ(
        label_infos0[0].get_transitions()[0].targets,
        (std::vector{1, 1}));

    ASSERT_EQ(label_infos0[1].get_cost(), 0);
    ASSERT_EQ(label_infos0[1].get_probabilities(), (std::vector{1_vt}));
    ASSERT_EQ(label_infos0[1].get_label_group(), std::vector{1});
    ASSERT_EQ(label_infos0[1].get_transitions().size(), 1);
    ASSERT_EQ(label_infos0[1].get_transitions()[0].src, 1);
    ASSERT_EQ(label_infos0[1].get_transitions()[0].targets, (std::vector{1}));

    ASSERT_EQ(label_infos1[0].get_cost(), 1);
    ASSERT_EQ(
        label_infos1[0].get_probabilities(),
        (std::vector{0.75_vt, 0.25_vt}));
    ASSERT_EQ(label_infos1[0].get_label_group(), std::vector{0});
    ASSERT_EQ(label_infos1[0].get_transitions().size(), 1);
    ASSERT_EQ(label_infos1[0].get_transitions()[0].src, 1);
    ASSERT_EQ(
        label_infos1[0].get_transitions()[0].targets,
        (std::vector{2, 1}));

    ASSERT_EQ(label_infos1[1].get_cost(), 0);
    ASSERT_EQ(label_infos1[1].get_probabilities(), (std::vector{1_vt}));
    ASSERT_EQ(label_infos1[1].get_label_group(), std::vector{1});
    ASSERT_EQ(label_infos1[1].get_transitions().size(), 1);
    ASSERT_EQ(label_infos1[1].get_transitions()[0].src, 2);
    ASSERT_EQ(label_infos1[1].get_transitions()[0].targets, (std::vector{1}));

    ASSERT_EQ(label_infos2[0].get_cost(), 1);
    ASSERT_EQ(
        label_infos2[0].get_probabilities(),
        (std::vector{0.75_vt, 0.25_vt}));
    ASSERT_EQ(label_infos2[0].get_label_group(), std::vector{0});
    ASSERT_EQ(label_infos2[0].get_transitions().size(), 1);
    ASSERT_EQ(label_infos2[0].get_transitions()[0].src, 1);
    ASSERT_EQ(
        label_infos2[0].get_transitions()[0].targets,
        (std::vector{0, 1}));

    ASSERT_EQ(label_infos2[1].get_cost(), 0);
    ASSERT_EQ(label_infos2[1].get_probabilities(), (std::vector{1_vt}));
    ASSERT_EQ(label_infos2[1].get_label_group(), std::vector{1});
    ASSERT_EQ(label_infos2[1].get_transitions().size(), 2);
    ASSERT_EQ(label_infos2[1].get_transitions()[0].src, 0);
    ASSERT_EQ(label_infos2[1].get_transitions()[0].targets, (std::vector{1}));
    ASSERT_EQ(label_infos2[1].get_transitions()[1].src, 1);
    ASSERT_EQ(label_infos2[1].get_transitions()[1].targets, (std::vector{1}));
}
