create_probfd_library(
    NAME mdp
    HELP "Core source files for supporting MDPs"
    SOURCES
    # Main
    probfd/command_line

    # Evaluators
    probfd/evaluator

    # Tasks
    probfd/probabilistic_task

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
    probfd/maxprob_cost_function
    probfd/ssp_cost_function

    # Task Cost Function Factories
    probfd/task_cost_function_factory

    # Task Evaluator Factories
    probfd/task_evaluator_factory

    # Constant evaluator (default)
    probfd/heuristics/constant_evaluator

    # Utility
    probfd/utils/guards

    probfd/solver_interface

    probfd/solvers/mdp_solver
    DEPENDS
    core_sources
    core_tasks
    plugins
    utils
    parser
    probabilistic_successor_generator
    CORE_LIBRARY
)

create_probfd_library(
    NAME probabilistic_successor_generator
    HELP "Probabilistic Successor generator"
    SOURCES
    probfd/task_utils/probabilistic_successor_generator
    probfd/task_utils/probabilistic_successor_generator_factory
    probfd/task_utils/probabilistic_successor_generator_internals
    DEPENDS task_properties
    DEPENDENCY_ONLY
)

create_probfd_library(
    NAME core_probabilistic_tasks
    HELP "Core probabilistic task transformations"
    SOURCES
    probfd/tasks/cost_adapted_task
    probfd/tasks/delegating_task
    probfd/tasks/root_task
    probfd/tasks/all_outcomes_determinization
    CORE_LIBRARY
)

create_probfd_library(
    NAME extra_probabilistic_tasks
    HELP "Additional probabilistic task transformations"
    SOURCES
    probfd/tasks/domain_abstracted_task
    probfd/tasks/domain_abstracted_task_factory
    probfd/tasks/modified_goals_task
    probfd/tasks/modified_operator_costs_task
    CORE_LIBRARY
)

create_probfd_library(
    NAME bisimulation_core
    HELP "bisimulation_core"
    SOURCES
    probfd/bisimulation/bisimilar_state_space
    probfd/bisimulation/evaluators
    DEPENDS
    mdp
    mas_heuristic
    DEPENDENCY_ONLY
)

create_probfd_library(
    NAME acyclic_value_iteration_solver
    HELP "acyclic_vi"
    SOURCES
    probfd/solvers/acyclic_vi
    DEPENDS mdp
)

create_probfd_library(
    NAME topological_value_iteration_solver
    HELP "topological_vi"
    SOURCES
    probfd/solvers/topological_vi
    DEPENDS mdp
)

create_probfd_library(
    NAME interval_iteration_solver
    HELP "interval_iteration"
    SOURCES
    probfd/solvers/interval_iteration
    DEPENDS mdp
)

create_probfd_library(
    NAME idual_solver
    HELP "idual solver"
    SOURCES
    probfd/solvers/idual
    DEPENDS mdp lp_solver
)

create_probfd_library(
    NAME i2dual_solver
    HELP "i2dual solvers"
    SOURCES
    probfd/algorithms/i2dual
    probfd/solvers/i2dual
    DEPENDS mdp lp_solver occupation_measures
)

create_probfd_library(
    NAME bisimulation_based_solver
    HELP "bisimulation_vi"
    SOURCES
    probfd/solvers/bisimulation_vi
    DEPENDS bisimulation_core
)

create_probfd_library(
    NAME mdp_heuristic_search_base
    HELP "mdp heuristic search core"
    SOURCES
    # Open Lists
    probfd/open_lists/subcategory

    # Transition Samplers
    probfd/successor_sampler
    probfd/successor_samplers/subcategory

    # Policy Tiebreakers
    probfd/policy_pickers/operator_id_tiebreaker
    probfd/policy_pickers/subcategory

    # Successor Sorters
    probfd/transition_sorters/vdiff_sorter
    probfd/transition_sorters/subcategory

    # Base heuristic search solver
    probfd/solvers/mdp_heuristic_search
    DEPENDENCY_ONLY
    DEPENDS mdp
)

create_probfd_library(
    NAME ao_search
    HELP "aostar implementations"
    SOURCES
    probfd/solvers/aostar
    probfd/solvers/exhaustive_ao
    DEPENDS mdp_heuristic_search_base bisimulation_core
)

create_probfd_library(
    NAME exhaustive_dfhs
    HELP "exhaustive depth-first heuristic search"
    SOURCES
    probfd/solvers/exhaustive_dfs
    DEPENDS mdp_heuristic_search_base
)

create_probfd_library(
    NAME lrtdp_solver
    HELP "lrtdp"
    SOURCES
    probfd/solvers/lrtdp
    DEPENDS mdp_heuristic_search_base bisimulation_core
)

create_probfd_library(
    NAME trap_aware_lrtdp_solver
    HELP "Trap-Aware LRTDP (TALRTDP)"
    SOURCES
    probfd/solvers/talrtdp
    DEPENDS mdp_heuristic_search_base
)

create_probfd_library(
    NAME trap_aware_dfhs_solver
    HELP "Trap-Aware DFHS (TADFHS)"
    SOURCES
    probfd/solvers/tadfhs
    DEPENDS mdp_heuristic_search_base
)

create_probfd_library(
    NAME trap_aware_topological_value_iteration_solver
    HELP "Trap-Aware Topological Value Iteration"
    SOURCES
    probfd/solvers/ta_topological_vi
    DEPENDS mdp
)

create_probfd_library(
    NAME dfhs_solver
    HELP "depth-first heuristic search"
    SOURCES
    probfd/solvers/hdfs
    DEPENDS mdp_heuristic_search_base bisimulation_core
)

create_probfd_library(
    NAME task_dependent_heuristic
    HELP "Heuristics depending on the input task"
    SOURCES
    probfd/heuristics/task_dependent_heuristic
    DEPENDENCY_ONLY
)

create_probfd_library(
    NAME deadend_pruning_heuristic
    HELP "Dead-end pruning heuristic"
    SOURCES
    probfd/heuristics/dead_end_pruning
    DEPENDS successor_generator task_dependent_heuristic
)

create_probfd_library(
    NAME determinization_heuristic
    HELP "All-outcomes determinization heuristic"
    SOURCES
    probfd/heuristics/determinization_cost
    DEPENDS successor_generator task_dependent_heuristic
)

create_probfd_library(
    NAME lp_based_heuristic
    HELP "LP-based heuristic"
    SOURCES
    probfd/heuristics/lp_heuristic
    DEPENDS mdp lp_solver task_dependent_heuristic
)

create_probfd_library(
    NAME occupation_measures
    HELP "Occupation measures"
    SOURCES
    probfd/occupation_measures/constraint_generator
    probfd/occupation_measures/hpom_constraints
    probfd/occupation_measures/hroc_constraints
    probfd/occupation_measures/higher_order_hpom_constraints
    probfd/occupation_measures/pho_constraints
    probfd/occupation_measures/subcategory
    DEPENDS mdp lp_based_heuristic
)

create_probfd_library(
    NAME occupation_measure_heuristic
    HELP "Occupation measure heuristic"
    SOURCES
    probfd/heuristics/occupation_measure_heuristic
    DEPENDS occupation_measures
)

create_probfd_library(
    NAME probability_aware_pdbs
    HELP "Probability-aware PDBs base classes"
    SOURCES
    probfd/pdbs/assignment_enumerator
    probfd/pdbs/distances
    probfd/pdbs/evaluators
    probfd/pdbs/match_tree
    probfd/pdbs/policy_extraction
    probfd/pdbs/probability_aware_pattern_database
    probfd/pdbs/projection_operator
    probfd/pdbs/projection_state_space
    probfd/pdbs/saturation
    probfd/pdbs/state_ranking_function
    probfd/pdbs/utils
    DEPENDS pdbs mdp task_dependent_heuristic
    DEPENDENCY_ONLY
)

create_probfd_library(
    NAME padbs_pattern_generators
    HELP "Base classes for pattern collection generation for PPDBs"
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

    DEPENDS probability_aware_pdbs max_cliques
    DEPENDENCY_ONLY
)

create_probfd_library(
    NAME papdbs_classical_generator
    HELP "Classical pattern collection generator adapter"
    SOURCES
    probfd/pdbs/pattern_collection_generator_classical
    DEPENDS padbs_pattern_generators
)

create_probfd_library(
    NAME papdbs_hillclimbing_generator
    HELP "Hillclimbing pattern collection generator for PPDBs"
    SOURCES
    probfd/pdbs/pattern_collection_generator_hillclimbing
    DEPENDS padbs_pattern_generators
)

create_probfd_library(
    NAME papdbs_cegar
    HELP "Disjoint CEGAR pattern collection generator for PPDBs"
    SOURCES
    probfd/pdbs/cegar/single_cegar
    probfd/pdbs/cegar/cegar
    probfd/pdbs/cegar/bfs_flaw_finder
    probfd/pdbs/cegar/pucs_flaw_finder
    probfd/pdbs/cegar/sampling_flaw_finder
    probfd/pdbs/cegar/flaw_finding_strategy
    DEPENDS padbs_pattern_generators
)

create_probfd_library(
    NAME papdbs_disjoint_cegar_generator
    HELP "Disjoint CEGAR pattern collection generator for PPDBs"
    SOURCES
    probfd/pdbs/pattern_collection_generator_disjoint_cegar
    DEPENDS papdbs_cegar
)

create_probfd_library(
    NAME papdbs_multiple_cegar_generator
    HELP "Multiple CEGAR pattern collection generator for PPDBs"
    SOURCES
    probfd/pdbs/pattern_collection_generator_multiple_cegar
    DEPENDS papdbs_cegar
)

create_probfd_library(
    NAME probability_aware_pdb_heuristic
    HELP "Probability-aware PDB heuristic"
    SOURCES
    probfd/heuristics/probability_aware_pdb_heuristic
    DEPENDS probability_aware_pdbs padbs_pattern_generators
)

create_probfd_library(
    NAME scp_pdb_heuristic
    HELP "Saturated Cost-Partitioning heuristic for probability-aware PDBs"
    SOURCES
    probfd/heuristics/scp_heuristic

    DEPENDS probability_aware_pdbs padbs_pattern_generators
)

create_probfd_library(
    NAME ucp_pdb_heuristic
    HELP "Uniform Cost-Partitioning heuristic for probability-aware PDBs"
    SOURCES
    probfd/heuristics/ucp_heuristic

    DEPENDS probability_aware_pdbs padbs_pattern_generators
)

create_probfd_library(
    NAME gzocp_pdb_heuristic
    HELP "Greedy Zero-One Cost-Partitioning heuristic for probability-aware PDBs"
    SOURCES
    probfd/heuristics/gzocp_heuristic

    DEPENDS probability_aware_pdbs padbs_pattern_generators
)

create_probfd_library(
    NAME pa_cartesian_abstractions
    HELP "The code for probability-aware cartesian abstractions"
    SOURCES
    probfd/cartesian_abstractions/abstract_state
    probfd/cartesian_abstractions/abstraction
    probfd/cartesian_abstractions/adaptive_flaw_generator
    probfd/cartesian_abstractions/astar_trace_generator
    probfd/cartesian_abstractions/cartesian_heuristic_function
    probfd/cartesian_abstractions/cegar
    probfd/cartesian_abstractions/cost_saturation
    probfd/cartesian_abstractions/complete_policy_flaw_finder
    probfd/cartesian_abstractions/distances
    probfd/cartesian_abstractions/evaluators
    probfd/cartesian_abstractions/flaw
    probfd/cartesian_abstractions/flaw_generator
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

create_probfd_library(
    NAME probability_aware_cartesian_abstraction_heuristic
    HELP "Probability-aware Cartesian Abstraction heuristic"
    SOURCES
    probfd/heuristics/additive_cartesian_heuristic
    DEPENDS pa_cartesian_abstractions
)