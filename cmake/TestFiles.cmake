create_test_library(
    NAME fp_tests
    HELP "Floating Point Tests"
    SOURCES
        tests/fp_tests
    DEPENDS
        mdp
        core_probabilistic_tasks
    CORE_LIBRARY
)

create_test_library(
    NAME interval_tests
    HELP "Interval Tests"
    SOURCES
        tests/interval_tests
    DEPENDS
        mdp
        core_probabilistic_tasks
)

create_test_library(
    NAME test_utils
    HELP "Interval Tests"
    SOURCES
        tests/tasks/blocksworld
    DEPENDS
        mdp
        core_probabilistic_tasks
)

create_test_library(
    NAME engine_tests
    HELP "Engine Tests"
    SOURCES
        tests/engine_tests
    DEPENDS
        test_utils
)

create_test_library(
    NAME cartesian_tests
    HELP "Cartesian Abstractions Tests"
    SOURCES
        tests/heuristics/cartesian_tests
    DEPENDS
        pa_cartesian_abstractions
        test_utils
)

create_test_library(
    NAME task_tests
    HELP "Task Tests"
    SOURCES
        tests/task_tests
    DEPENDS
        test_utils
)

create_test_library(
    NAME pdb_tests
    HELP "PDB Tests"
    SOURCES
        tests/pdb_tests
    DEPENDS
        test_utils
        probability_aware_pdbs
        mdp
)

create_test_library(
    NAME mns_tests
    HELP "MnS Tests"
    SOURCES
        tests/mns_tests
    DEPENDS
        test_utils
        pa_mas
        mdp
)