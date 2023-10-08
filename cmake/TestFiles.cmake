create_test_library(
    NAME fp_tests
    HELP "Floating Point Tests"
    SOURCES
        fp_tests
    DEPENDS
        mdp
    DEPENDENCY_ONLY
)

create_test_library(
    NAME interval_tests
    HELP "Interval Tests"
    SOURCES
        interval_tests
    DEPENDS
        mdp
    DEPENDENCY_ONLY
)

create_test_library(
    NAME test_utils
    HELP "Interval Tests"
    SOURCES
        tasks/blocksworld
        verification/policy_verification
    DEPENDS
        mdp
    DEPENDENCY_ONLY
)

create_test_library(
    NAME engine_tests
    HELP "Engine Tests"
    SOURCES
        engine_tests
    DEPENDS
        test_utils
    DEPENDENCY_ONLY
)

create_test_library(
    NAME cartesian_tests
    HELP "Cartesian Abstractions Tests"
    SOURCES
        heuristics/cartesian_tests
    DEPENDS
        pa_cartesian_abstractions
        test_utils
    DEPENDENCY_ONLY
)

create_test_library(
    NAME task_tests
    HELP "Task Tests"
    SOURCES
        task_tests
    DEPENDS
        test_utils
    DEPENDENCY_ONLY
)