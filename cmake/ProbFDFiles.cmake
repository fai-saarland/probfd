create_library(
    NAME probfd_core
    SOURCES
        # Evaluators
        probfd/evaluator

        # Tasks
        probfd/probabilistic_task
        probfd/task_proxy

        probfd/task_utils/task_properties

        # Basic types
        probfd/evaluation_result
        probfd/interval
        probfd/value_type

        # MDP interfaces
        probfd/state_evaluator
        probfd/cost_function
        probfd/caching_task_state_space
        probfd/task_state_space
        probfd/progress_report
        probfd/quotient_system

        # Algorithms
        probfd/algorithms/utils

        # Cost Functions
        probfd/task_cost_function

        # Task Evaluator Factories
        probfd/task_evaluator_factory

        # Constant evaluator (default)
        probfd/heuristics/constant_evaluator

        # Task Utils
        probfd/task_utils/causal_graph
        probfd/task_utils/sampling

        # Utility
        probfd/utils/guards
        probfd/utils/not_implemented

        probfd/solver_interface

        probfd/solvers/mdp_solver
        probfd/bisimulation/bisimilar_state_space
        probfd/solvers/bisimulation_heuristic_search_algorithm
    DEPENDS
        core_sources
        core_tasks
        utils
        probabilistic_successor_generator
        mas_heuristic
)

create_library(
    NAME probabilistic_successor_generator
    SOURCES
        probfd/task_utils/probabilistic_successor_generator
        probfd/task_utils/probabilistic_successor_generator_factory
        probfd/task_utils/probabilistic_successor_generator_internals
    DEPENDS
        task_properties
)

create_library(
    NAME core_probabilistic_tasks
    SOURCES
        probfd/tasks/cost_adapted_task
        probfd/tasks/determinization_task
        probfd/tasks/delegating_task
        probfd/tasks/root_task
    DEPENDS
        probfd_core
)

create_library(
    NAME extra_probabilistic_tasks
    SOURCES
        probfd/tasks/domain_abstracted_task
        probfd/tasks/domain_abstracted_task_factory
        probfd/tasks/modified_goals_task
        probfd/tasks/modified_operator_costs_task
)

create_library(
    NAME i2dual_solver
    SOURCES
        probfd/algorithms/i2dual
    DEPENDS
        probfd_core
        lp_solver
        occupation_measures
)

create_library(
    NAME mdp_heuristic_search_base
    SOURCES
        # Transition Samplers
        probfd/successor_sampler

        # Policy Tiebreakers
        probfd/policy_pickers/operator_id_tiebreaker

        # Successor Sorters
        probfd/transition_sorters/vdiff_sorter

        # Base heuristic search solver
        probfd/solvers/mdp_heuristic_search
    DEPENDS
        probfd_core
)

create_library(
    NAME task_dependent_heuristic
    SOURCES
        probfd/heuristics/task_dependent_heuristic
)

create_library(
    NAME deadend_pruning_heuristic
    SOURCES
        probfd/heuristics/dead_end_pruning
    DEPENDS
        successor_generator
        task_dependent_heuristic
)

create_library(
    NAME determinization_heuristic
    SOURCES
        probfd/heuristics/determinization_cost
    DEPENDS
        successor_generator
        task_dependent_heuristic
)

create_library(
    NAME lp_based_heuristic
    SOURCES
        probfd/heuristics/lp_heuristic
    DEPENDS
        probfd_core
        lp_solver
        task_dependent_heuristic
)

create_library(
    NAME occupation_measures
    SOURCES
        probfd/occupation_measures/constraint_generator
        probfd/occupation_measures/hpom_constraints
        probfd/occupation_measures/hroc_constraints
        probfd/occupation_measures/higher_order_hpom_constraints
        probfd/occupation_measures/pho_constraints
    DEPENDS
        probfd_core
        lp_based_heuristic
)

create_library(
    NAME occupation_measure_heuristic
    SOURCES
        probfd/heuristics/occupation_measure_heuristic
    DEPENDS
        occupation_measures
)

create_library(
    NAME probability_aware_pdbs
    SOURCES
        probfd/pdbs/assignment_enumerator
        probfd/pdbs/evaluators
        probfd/pdbs/match_tree
        probfd/pdbs/probability_aware_pattern_database
        probfd/pdbs/projection_operator
        probfd/pdbs/projection_state_space
        probfd/pdbs/projection_transformation
        probfd/pdbs/saturation
        probfd/pdbs/state_ranking_function
        probfd/pdbs/utils
    DEPENDS
        pdbs
        probfd_core
        task_dependent_heuristic
        lp_solver
)

create_library(
    NAME padbs_pattern_generators
    SOURCES
        probfd/pdbs/pattern_information
        probfd/pdbs/pattern_generator
        probfd/pdbs/pattern_collection_information
        probfd/pdbs/pattern_collection_generator
        probfd/pdbs/pattern_collection_generator_multiple

        probfd/pdbs/subcollection_finder_factory
        probfd/pdbs/subcollection_finder
        probfd/pdbs/max_orthogonal_finder_factory
        probfd/pdbs/max_orthogonal_finder
        probfd/pdbs/trivial_finder_factory
        probfd/pdbs/trivial_finder
        probfd/pdbs/fully_additive_finder_factory
        probfd/pdbs/fully_additive_finder
        probfd/pdbs/trivial_finder
        probfd/pdbs/subcollections
    DEPENDS
        probability_aware_pdbs
        max_cliques
)

create_library(
    NAME papdbs_classical_generator
    SOURCES
        probfd/pdbs/pattern_collection_generator_classical
    DEPENDS
        padbs_pattern_generators
)

create_library(
    NAME papdbs_systematic_generator
    SOURCES
        probfd/pdbs/pattern_collection_generator_systematic
    DEPENDS
        padbs_pattern_generators
)

create_library(
    NAME papdbs_hillclimbing_generator
    SOURCES
        probfd/pdbs/pattern_collection_generator_hillclimbing
    DEPENDS
        padbs_pattern_generators
)

create_library(
    NAME papdbs_cegar
    SOURCES
        probfd/pdbs/cegar/single_cegar
        probfd/pdbs/cegar/cegar
        probfd/pdbs/cegar/bfs_flaw_finder
        probfd/pdbs/cegar/pucs_flaw_finder
        probfd/pdbs/cegar/sampling_flaw_finder
        probfd/pdbs/cegar/flaw_finding_strategy
    DEPENDS
        padbs_pattern_generators
)

create_library(
    NAME papdbs_disjoint_cegar_generator
    SOURCES
        probfd/pdbs/pattern_collection_generator_disjoint_cegar
    DEPENDS
        papdbs_cegar
)

create_library(
    NAME papdbs_multiple_cegar_generator
    SOURCES
        probfd/pdbs/pattern_collection_generator_multiple_cegar
    DEPENDS
        papdbs_cegar
)

create_library(
    NAME probability_aware_pdb_heuristic
    SOURCES
        probfd/heuristics/probability_aware_pdb_heuristic
    DEPENDS
        probability_aware_pdbs
        padbs_pattern_generators
)

create_library(
    NAME scp_pdb_heuristic
    SOURCES
        probfd/heuristics/scp_heuristic
    DEPENDS
        probability_aware_pdbs
        padbs_pattern_generators
)

create_library(
    NAME ucp_pdb_heuristic
    SOURCES
        probfd/heuristics/ucp_heuristic
    DEPENDS
        probability_aware_pdbs padbs_pattern_generators
)

create_library(
    NAME gzocp_pdb_heuristic
    SOURCES
        probfd/heuristics/gzocp_heuristic
    DEPENDS
        probability_aware_pdbs padbs_pattern_generators
)

create_library(
    NAME pa_cartesian_abstractions
    SOURCES
        probfd/cartesian_abstractions/abstract_state
        probfd/cartesian_abstractions/adaptive_flaw_generator
        probfd/cartesian_abstractions/astar_trace_generator
        probfd/cartesian_abstractions/cartesian_abstraction
        probfd/cartesian_abstractions/cartesian_heuristic_function
        probfd/cartesian_abstractions/cegar
        probfd/cartesian_abstractions/cost_saturation
        probfd/cartesian_abstractions/complete_policy_flaw_finder
        probfd/cartesian_abstractions/evaluators
        probfd/cartesian_abstractions/flaw
        probfd/cartesian_abstractions/ilao_policy_generator
        probfd/cartesian_abstractions/policy_based_flaw_generator
        probfd/cartesian_abstractions/probabilistic_transition_system
        probfd/cartesian_abstractions/split_selector
        probfd/cartesian_abstractions/subtask_generators
        probfd/cartesian_abstractions/trace_based_flaw_generator
        probfd/cartesian_abstractions/utils
    DEPENDS
        cartesian_abstractions
        additive_heuristic
        extra_probabilistic_tasks
        task_dependent_heuristic
)

create_library(
    NAME probability_aware_cartesian_abstraction_heuristic
    SOURCES
        probfd/heuristics/additive_cartesian_heuristic
    DEPENDS
        pa_cartesian_abstractions
)