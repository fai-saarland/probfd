create_library(
    NAME fp_tests
    HELP "Enables floating point tests"
    SOURCES
        tests/fp_tests
    DEPENDS
        GTest::gtest
        probfd_core
        core_probabilistic_tasks
    TARGET probfd_tests
)

create_library(
    NAME interval_tests
    HELP "Enables interval tests"
    SOURCES
        tests/interval_tests
    DEPENDS
        GTest::gtest
        probfd_core
        core_probabilistic_tasks
    TARGET probfd_tests
)

create_library(
    NAME test_utils
    SOURCES
        tests/tasks/blocksworld
    DEPENDS
        GTest::gtest
        probfd_core
        core_probabilistic_tasks
)

create_library(
    NAME engine_tests
    HELP "Enables search algorithm tests"
    SOURCES
        tests/engine_tests
    DEPENDS
        GTest::gtest
        test_utils
    TARGET probfd_tests
)

create_library(
    NAME cartesian_tests
    HELP "Enables cartesian abstraction tests"
    SOURCES
        tests/heuristics/cartesian_tests
    DEPENDS
        GTest::gtest
        pa_cartesian_abstractions
        test_utils
    TARGET probfd_tests
)

create_library(
    NAME pdb_tests
    HELP "Enables probabilistic pattern databases tests"
    SOURCES
        tests/pdb_tests
    DEPENDS
        GTest::gtest
        test_utils
        probability_aware_pdbs
        probfd_core
    TARGET probfd_tests
)

create_library(
    NAME mns_tests
    HELP "MnS Tests"
    SOURCES
        tests/mns_tests
    DEPENDS
        GTest::gtest
        test_utils
        pa_merge_and_shrink
        probfd_core
        # TODO remove
        plugins
        parser
        logging_options
        rng_options
    TARGET
        probfd_tests
)