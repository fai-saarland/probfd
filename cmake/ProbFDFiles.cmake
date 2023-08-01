fast_downward_plugin(
    NAME MDP
    HELP "Core source files for supporting MDPs"
    SOURCES
        # Main
        probfd/command_line

        # Tasks
        probfd/probabilistic_task

        probfd/task_utils/task_properties

        # Basic types
        probfd/evaluation_result
        probfd/interval
        probfd/value_type
        
        # Engine interfaces
        probfd/state_evaluator
        probfd/cost_function
        probfd/caching_task_state_space
        probfd/task_state_space
        probfd/progress_report
        probfd/quotient_system

        # Engines
        probfd/engines/utils

        # Cost Models
        probfd/cost_model
        probfd/cost_models/maxprob_cost_model
        probfd/cost_models/ssp_cost_model

        # Constant evaluator (default)
        probfd/heuristics/constant_evaluator

        # Utility
        probfd/utils/guards.cc

        probfd/solver_interface
        
        probfd/solvers/mdp_solver
    DEPENDS SUCCESSOR_GENERATOR
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME CORE_PROBABILISTIC_TASKS
    HELP "Core probabilistic task transformations"
    SOURCES
        probfd/tasks/cost_adapted_task
        probfd/tasks/delegating_task
        probfd/tasks/root_task
        probfd/tasks/all_outcomes_determinization
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME EXTRA_PROBABILISTIC_TASKS
    HELP "Additional probabilistic task transformations"
    SOURCES
        probfd/tasks/domain_abstracted_task
        probfd/tasks/domain_abstracted_task_factory
        probfd/tasks/modified_goals_task
        probfd/tasks/modified_operator_costs_task
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME BISIMULATION_CORE
    HELP "bisimulation_core"
    SOURCES
        probfd/bisimulation/bisimilar_state_space
        probfd/bisimulation/engine_interfaces
    DEPENDS MDP
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME ACYCLIC_VALUE_ITERATION_SOLVER
    HELP "acyclic_vi"
    SOURCES
        probfd/solvers/acyclic_vi
    DEPENDS MDP
)

fast_downward_plugin(
    NAME TOPOLOGICAL_VALUE_ITERATION_SOLVER
    HELP "topological_vi"
    SOURCES
        probfd/solvers/topological_vi
    DEPENDS MDP
)

fast_downward_plugin(
    NAME INTERVAL_ITERATION_SOLVER
    HELP "interval_iteration"
    SOURCES
        probfd/solvers/interval_iteration
    DEPENDS MDP
)

fast_downward_plugin(
    NAME IDUAL_SOLVER
    HELP "idual & i2dual solvers"
    SOURCES
        probfd/solvers/idual
        probfd/solvers/i2dual
    DEPENDS MDP LP_SOLVER OCCUPATION_MEASURE_HEURISTICS
)

fast_downward_plugin(
    NAME BISIMULATION_BASED_SOLVER
    HELP "bisimulation_vi"
    SOURCES
        probfd/solvers/bisimulation_vi
    DEPENDS BISIMULATION_CORE
)

fast_downward_plugin(
    NAME MDP_HEURISTIC_SEARCH_BASE
    HELP "mdp heuristic search core"
    SOURCES
        # Engine interface subcategories
        probfd/engine_interfaces/subcategory

        # Open Lists
        probfd/open_lists/fifo_open_list_factory
        probfd/open_lists/lifo_open_list_factory

        probfd/open_lists/subcategory

        # Transition Samplers
        probfd/successor_sampler
        probfd/successor_samplers/arbitrary_selector
        probfd/successor_samplers/most_likely_selector
        probfd/successor_samplers/uniform_successor_sampler
        probfd/successor_samplers/vbiased_successor_sampler
        probfd/successor_samplers/vdiff_successor_sampler

        probfd/successor_samplers/arbitrary_selector_factory
        probfd/successor_samplers/most_likely_selector_factory
        probfd/successor_samplers/uniform_successor_sampler_factory
        probfd/successor_samplers/random_successor_sampler_factory
        probfd/successor_samplers/vbiased_successor_sampler_factory
        probfd/successor_samplers/vdiff_successor_sampler_factory

        probfd/successor_samplers/subcategory

        # Policy Tiebreakers
        probfd/policy_pickers/operator_id_tiebreaker
        probfd/policy_pickers/random_tiebreaker
        probfd/policy_pickers/vdiff_tiebreaker

        probfd/policy_pickers/arbitrary_tiebreaker_factory
        probfd/policy_pickers/operator_id_tiebreaker_factory
        probfd/policy_pickers/random_tiebreaker_factory
        probfd/policy_pickers/vdiff_tiebreaker_factory

        probfd/policy_pickers/subcategory

        # Successor Sorters
        probfd/transition_sorters/vdiff_sorter
        probfd/transition_sorters/vdiff_sorter_factory
        probfd/transition_sorters/subcategory

        # Base heuristic search solver
        probfd/solvers/mdp_heuristic_search
    DEPENDENCY_ONLY
    DEPENDS MDP
)

fast_downward_plugin(
    NAME AO_SEARCH
    HELP "aostar implementations"
    SOURCES
        probfd/solvers/aostar
        probfd/solvers/exhaustive_ao
    DEPENDS MDP_HEURISTIC_SEARCH_BASE BISIMULATION_CORE
)

fast_downward_plugin(
    NAME EXHDFS
    HELP "exhaustive heuristic depth-first search"
    SOURCES
        probfd/solvers/exhaustive_dfs
    DEPENDS MDP_HEURISTIC_SEARCH_BASE
)

fast_downward_plugin(
    NAME LRTDP_SOLVER
    HELP "lrtdp"
    SOURCES
        probfd/solvers/lrtdp
    DEPENDS MDP_HEURISTIC_SEARCH_BASE BISIMULATION_CORE
)

fast_downward_plugin(
    NAME TRAP_AWARE_LRTDP_SOLVER
    HELP "Trap-Aware LRTDP (TALRTDP)"
    SOURCES
        probfd/solvers/talrtdp
    DEPENDS MDP_HEURISTIC_SEARCH_BASE
)

fast_downward_plugin(
    NAME TRAP_AWARE_DFHS_SOLVER
    HELP "Trap-Aware DFHS (TADFHS)"
    SOURCES
        probfd/solvers/tadfhs
    DEPENDS MDP_HEURISTIC_SEARCH_BASE
)

fast_downward_plugin(
    NAME TRAP_AWARE_TOPOLOGICAL_VALUE_ITERATION_SOLVER
    HELP "Trap-Aware Topological Value Iteration"
    SOURCES
        probfd/solvers/ta_topological_vi
    DEPENDS MDP
)

fast_downward_plugin(
    NAME HDFS_SOLVERS
    HELP "heuristic depth-first search"
    SOURCES
        probfd/solvers/hdfs
    DEPENDS MDP_HEURISTIC_SEARCH_BASE BISIMULATION_CORE
)

fast_downward_plugin(
    NAME TASK_DEPENDENT_HEURISTIC
    HELP "Heuristics depending on the input task"
    SOURCES
        probfd/heuristics/task_dependent_heuristic
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME DEADEND_PRUNING_HEURISTIC
    HELP "Dead-end pruning heuristic"
    SOURCES
        probfd/heuristics/dead_end_pruning
    DEPENDS SUCCESSOR_GENERATOR TASK_DEPENDENT_HEURISTIC
)

fast_downward_plugin(
    NAME DETERMINIZATION_HEURISTIC
    HELP "All-outcomes determinization heuristic"
    SOURCES
        probfd/heuristics/determinization_cost
    DEPENDS SUCCESSOR_GENERATOR TASK_DEPENDENT_HEURISTIC
)

fast_downward_plugin(
    NAME LP_BASED_HEURISTICS
    HELP "Lp-based heuristic"
    SOURCES
        probfd/heuristics/lp_heuristic
    DEPENDS MDP LP_SOLVER TASK_DEPENDENT_HEURISTIC
)

fast_downward_plugin(
    NAME OCCUPATION_MEASURE_HEURISTICS
    HELP "Occupation measure heuristics"
    SOURCES
        probfd/heuristics/occupation_measures/constraint_generator
        probfd/heuristics/occupation_measures/hpom_constraints
        probfd/heuristics/occupation_measures/hroc_constraints
        probfd/heuristics/occupation_measures/higher_order_hpom_constraints
        probfd/heuristics/occupation_measures/occupation_measure_heuristic
        probfd/heuristics/occupation_measures/pho_constraints
        probfd/heuristics/occupation_measures/subcategory
    DEPENDS MDP LP_BASED_HEURISTICS
)

fast_downward_plugin(
    NAME PROBABILITY_AWARE_PDBS
    HELP "Probability-aware PDBs base classes"
    SOURCES
        probfd/heuristics/pdbs/engine_interfaces
        probfd/heuristics/pdbs/match_tree
        probfd/heuristics/pdbs/probability_aware_pattern_database
        probfd/heuristics/pdbs/projection_operator
        probfd/heuristics/pdbs/projection_state_space
        probfd/heuristics/pdbs/state_rank
        probfd/heuristics/pdbs/state_ranking_function
    DEPENDS PDBS MDP SUCCESSOR_GENERATOR TASK_DEPENDENT_HEURISTIC
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME PPDBS_PATTERN_GENERATORS
    HELP "Base classes for pattern collection generation for PPDBs"
    SOURCES
        probfd/heuristics/pdbs/pattern_information
        probfd/heuristics/pdbs/pattern_generator
        probfd/heuristics/pdbs/pattern_collection_information
        probfd/heuristics/pdbs/pattern_collection_generator
        probfd/heuristics/pdbs/pattern_collection_generator_multiple

        probfd/heuristics/pdbs/subcollection_finder_factory
        probfd/heuristics/pdbs/subcollection_finder
        probfd/heuristics/pdbs/max_orthogonal_finder_factory
        probfd/heuristics/pdbs/max_orthogonal_finder
        probfd/heuristics/pdbs/trivial_finder_factory
        probfd/heuristics/pdbs/trivial_finder
        probfd/heuristics/pdbs/fully_additive_finder_factory
        probfd/heuristics/pdbs/fully_additive_finder
        probfd/heuristics/pdbs/trivial_finder
        probfd/heuristics/pdbs/subcollections

        probfd/heuristics/pdbs/utils
    DEPENDS PROBABILITY_AWARE_PDBS CAUSAL_GRAPH MAX_CLIQUES
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME PPDBS_CLASSICAL_GENERATOR
    HELP "Classical pattern collection generator adapter"
    SOURCES
        probfd/heuristics/pdbs/pattern_collection_generator_classical
    DEPENDS PPDBS_PATTERN_GENERATORS
)

fast_downward_plugin(
    NAME PPDBS_HILLCLIMBING_GENERATOR
    HELP "Hillclimbing pattern collection generator for PPDBs"
    SOURCES
        probfd/heuristics/pdbs/pattern_collection_generator_hillclimbing
    DEPENDS PPDBS_PATTERN_GENERATORS
)

fast_downward_plugin(
    NAME PPDBS_CEGAR
    HELP "Disjoint CEGAR pattern collection generator for PPDBs"
    SOURCES
        probfd/heuristics/pdbs/cegar/cegar
        probfd/heuristics/pdbs/cegar/bfs_flaw_finder
        probfd/heuristics/pdbs/cegar/pucs_flaw_finder
        probfd/heuristics/pdbs/cegar/sampling_flaw_finder
        probfd/heuristics/pdbs/cegar/flaw_finding_strategy
    DEPENDS PPDBS_PATTERN_GENERATORS
)

fast_downward_plugin(
    NAME PPDBS_DISJOINT_CEGAR_GENERATOR
    HELP "Disjoint CEGAR pattern collection generator for PPDBs"
    SOURCES
        probfd/heuristics/pdbs/pattern_collection_generator_disjoint_cegar
    DEPENDS PPDBS_CEGAR
)

fast_downward_plugin(
    NAME PPDBS_MULTIPLE_CEGAR_GENERATOR
    HELP "Multiple CEGAR pattern collection generator for PPDBs"
    SOURCES
        probfd/heuristics/pdbs/pattern_collection_generator_multiple_cegar
    DEPENDS PPDBS_CEGAR
)

fast_downward_plugin(
    NAME PROBABILITY_AWARE_PDB_HEURISTIC
    HELP "Probability-aware PDB heuristic"
    SOURCES
        probfd/heuristics/pdbs/probability_aware_pdb_heuristic
    DEPENDS PROBABILITY_AWARE_PDBS PPDBS_PATTERN_GENERATORS
)

fast_downward_plugin(
    NAME SCP_PDB_HEURISTIC
    HELP "Saturated Cost-Partitioning heuristic for probability-aware PDBs"
    SOURCES
        probfd/heuristics/cost_partitioning/scp_heuristic

    DEPENDS PROBABILITY_AWARE_PDBS PPDBS_PATTERN_GENERATORS
)

fast_downward_plugin(
    NAME UCP_PDB_HEURISTIC
    HELP "Uniform Cost-Partitioning heuristic for probability-aware PDBs"
    SOURCES
        probfd/heuristics/cost_partitioning/ucp_heuristic

    DEPENDS PROBABILITY_AWARE_PDBS PPDBS_PATTERN_GENERATORS
)

fast_downward_plugin(
    NAME GZOCP_PDB_HEURISTIC
    HELP "Greedy Zero-One Cost-Partitioning heuristic for probability-aware PDBs"
    SOURCES
        probfd/heuristics/cost_partitioning/gzocp_heuristic

    DEPENDS PROBABILITY_AWARE_PDBS PPDBS_PATTERN_GENERATORS
)

fast_downward_plugin(
    NAME PROBABILISTIC_CARTESIAN
    HELP "Plugin containing the code for CEGAR heuristics"
    SOURCES
        probfd/heuristics/cartesian/abstract_state
        probfd/heuristics/cartesian/abstraction
        probfd/heuristics/cartesian/adaptive_flaw_generator
        probfd/heuristics/cartesian/additive_cartesian_heuristic
        probfd/heuristics/cartesian/astar_trace_generator
        probfd/heuristics/cartesian/cartesian_heuristic_function
        probfd/heuristics/cartesian/cegar
        probfd/heuristics/cartesian/cost_saturation
        probfd/heuristics/cartesian/complete_policy_flaw_finder
        probfd/heuristics/cartesian/distances
        probfd/heuristics/cartesian/engine_interfaces
        probfd/heuristics/cartesian/flaw
        probfd/heuristics/cartesian/flaw_generator
        probfd/heuristics/cartesian/ilao_policy_generator
        probfd/heuristics/cartesian/policy_based_flaw_generator
        probfd/heuristics/cartesian/probabilistic_transition_system
        probfd/heuristics/cartesian/split_selector
        probfd/heuristics/cartesian/subtask_generators
        probfd/heuristics/cartesian/trace_based_flaw_generator
        probfd/heuristics/cartesian/utils
    DEPENDS CEGAR ADDITIVE_HEURISTIC EXTRA_PROBABILISTIC_TASKS
)

fast_downward_add_plugin_sources(PROBFD_SOURCES)

list(REVERSE PROBFD_SOURCES)
