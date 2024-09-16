create_probfd_library(
    NAME probfd_core
    HELP "Core source files of probabilistic Fast Downward"
    SOURCES
        # Main
        probfd/command_line

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
        probfd/solvers/bisimulation_heuristic_search_algorithm
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
    NAME probfd_core_plugins
    HELP "Core ProbFD plugins"
    SOURCES
        probfd_plugins/probabilistic_task
        probfd_plugins/solver_interface
        probfd_plugins/task_evaluator_factory
    DEPENDS
        probfd_core
)

create_probfd_library(
    NAME probabilistic_successor_generator
    HELP "Probabilistic Successor generator"
    SOURCES
        probfd/task_utils/probabilistic_successor_generator
        probfd/task_utils/probabilistic_successor_generator_factory
        probfd/task_utils/probabilistic_successor_generator_internals
    DEPENDS
        task_properties
    DEPENDENCY_ONLY
)

create_probfd_library(
    NAME core_probabilistic_tasks
    HELP "Core probabilistic tasks"
    SOURCES
        probfd/tasks/cost_adapted_task
        probfd/tasks/determinization_task
        probfd/tasks/delegating_task
        probfd/tasks/root_task
    CORE_LIBRARY
)

create_probfd_library(
    NAME core_probabilistic_task_plugins
    HELP "Core probabilistic task transformation plugins"
    SOURCES
        probfd_plugins/tasks/root_task
    DEPENDS
        core_probabilistic_tasks
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
    DEPENDS
        probfd_core
        mas_heuristic
    DEPENDENCY_ONLY
)

create_probfd_library(
    NAME mdp_solver_options
    HELP "Plugin options for MDP solvers"
    SOURCES
        probfd_plugins/solvers/mdp_solver
    DEPENDS
        probfd_core
)

create_probfd_library(
    NAME acyclic_value_iteration_solver_plugin
    HELP "Acyclic value iteration solver plugin"
    SOURCES
        probfd_plugins/solvers/acyclic_vi
    DEPENDS
       mdp_solver_options
)

create_probfd_library(
    NAME topological_value_iteration_solver_plugin
    HELP "Topological value iteration solver plugin"
    SOURCES
        probfd_plugins/solvers/topological_vi
    DEPENDS
        mdp_solver_options
)

create_probfd_library(
    NAME trap_aware_topological_value_iteration_solver_plugin
    HELP "Trap-aware topological value iteration solver plugin"
    SOURCES
        probfd_plugins/solvers/ta_topological_vi
    DEPENDS
        mdp_solver_options
)

create_probfd_library(
    NAME interval_iteration_solver_plugin
    HELP "Interval iteration solver plugin"
    SOURCES
        probfd_plugins/solvers/interval_iteration
    DEPENDS
        mdp_solver_options
)

create_probfd_library(
    NAME idual_solver_plugin
    HELP "i-dual solver plugin"
    SOURCES
        probfd_plugins/solvers/idual
    DEPENDS
        mdp_solver_options
        lp_solver
)

create_probfd_library(
    NAME i2dual_solver
    HELP "i^2-dual solver"
    SOURCES
        probfd/algorithms/i2dual
    DEPENDS
        probfd_core
        lp_solver
        occupation_measures
)

create_probfd_library(
    NAME i2dual_solver_plugin
    HELP "i^2-dual solver plugin"
    SOURCES
        probfd_plugins/solvers/i2dual
    DEPENDS
        i2dual_solver
        mdp_solver_options
)

create_probfd_library(
    NAME bisimulation_based_solver_plugin
    HELP "Bisimulation-based value iteration solver plugin"
    SOURCES
        probfd_plugins/solvers/bisimulation_vi
    DEPENDS
        bisimulation_core
        mdp_solver_options
)

create_probfd_library(
    NAME mdp_heuristic_search_base
    HELP "mdp heuristic search core"
    SOURCES
        # Transition Samplers
        probfd/successor_sampler

        # Policy Tiebreakers
        probfd/policy_pickers/operator_id_tiebreaker

        # Successor Sorters
        probfd/transition_sorters/vdiff_sorter

        # Base heuristic search solver
        probfd/solvers/mdp_heuristic_search
    DEPENDENCY_ONLY
    DEPENDS
        probfd_core
)

create_probfd_library(
    NAME mdp_heuristic_search_base_options
    HELP "mdp heuristic search core plugin options"
    SOURCES
        probfd_plugins/solvers/mdp_heuristic_search
    DEPENDENCY_ONLY
    DEPENDS
        mdp_heuristic_search_base
        mdp_solver_options
)

create_probfd_library(
    NAME ao_search_plugins
    HELP "AO* solver variant plugins"
    SOURCES
        probfd_plugins/solvers/aostar
        probfd_plugins/solvers/exhaustive_ao
    DEPENDS
        mdp_heuristic_search_base_options
        bisimulation_core
)

create_probfd_library(
    NAME exhaustive_dfhs_plugin
    HELP "Exhaustive depth-first heuristic search solver plugin"
    SOURCES
        probfd_plugins/solvers/exhaustive_dfs
    DEPENDS
        mdp_heuristic_search_base_options
)

create_probfd_library(
    NAME lrtdp_solver_plugin
    HELP "Labeled real-time dynamic programming solver plugin"
    SOURCES
        probfd_plugins/solvers/lrtdp
    DEPENDS
        mdp_heuristic_search_base_options
        bisimulation_core
)

create_probfd_library(
    NAME trap_aware_lrtdp_solver_plugin
    HELP "Trap-Aware LRTDP solver plugin"
    SOURCES
        probfd_plugins/solvers/talrtdp
    DEPENDS
        mdp_heuristic_search_base_options
)

create_probfd_library(
    NAME trap_aware_dfhs_solver_plugins
    HELP "Trap-Aware DFHS solver plugins"
    SOURCES
        probfd_plugins/solvers/ta_depth_first_heuristic_search
    DEPENDS
        mdp_heuristic_search_base_options
)

create_probfd_library(
    NAME dfhs_solver_plugins
    HELP "depth-first heuristic search solver plugins"
    SOURCES
        probfd_plugins/solvers/depth_first_heuristic_search
    DEPENDS
        mdp_heuristic_search_base_options
        bisimulation_core
)

create_probfd_library(
    NAME task_dependent_heuristic
    HELP "Heuristics depending on the input task"
    SOURCES
        probfd/heuristics/task_dependent_heuristic
    DEPENDENCY_ONLY
)

create_probfd_library(
    NAME task_dependent_heuristic_plugin
    HELP "Planning Task heuristics plugin options"
    SOURCES
        probfd_plugins/heuristics/task_dependent_heuristic
    DEPENDS
        task_dependent_heuristic
)

create_probfd_library(
    NAME deadend_pruning_heuristic
    HELP "Dead-end pruning heuristic"
    SOURCES
        probfd/heuristics/dead_end_pruning
    DEPENDS
        successor_generator
        task_dependent_heuristic
)

create_probfd_library(
    NAME determinization_heuristic
    HELP "All-outcomes determinization heuristic"
    SOURCES
        probfd/heuristics/determinization_cost
    DEPENDS
        successor_generator
        task_dependent_heuristic
)

create_probfd_library(
    NAME lp_based_heuristic
    HELP "LP-based heuristic"
    SOURCES
        probfd/heuristics/lp_heuristic
    DEPENDS
        probfd_core
        lp_solver
        task_dependent_heuristic
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
    DEPENDS
        probfd_core
        lp_based_heuristic
)

create_probfd_library(
    NAME occupation_measure_heuristic
    HELP "Occupation measure heuristic"
    SOURCES
        probfd/heuristics/occupation_measure_heuristic
    DEPENDS
        occupation_measures
)

create_probfd_library(
    NAME probability_aware_pdbs
    HELP "Probability-aware PDBs base classes"
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
    DEPENDS
        probability_aware_pdbs
        max_cliques
    DEPENDENCY_ONLY
)

create_probfd_library(
    NAME padbs_pattern_generators_plugins
    HELP "Base plugins for classes of pattern collection generation for PPDBs"
    SOURCES
        probfd_plugins/pdbs/pattern_generator
        probfd_plugins/pdbs/pattern_collection_generator
        probfd_plugins/pdbs/pattern_collection_generator_multiple

        probfd_plugins/pdbs/subcollection_finder_factory
        probfd_plugins/pdbs/max_orthogonal_finder_factory
        probfd_plugins/pdbs/trivial_finder_factory
        probfd_plugins/pdbs/fully_additive_finder_factory
    DEPENDS
        padbs_pattern_generators
    DEPENDENCY_ONLY
)

create_probfd_library(
    NAME papdbs_classical_generator
    HELP "Classical pattern collection generator adapter"
    SOURCES
        probfd/pdbs/pattern_collection_generator_classical
    DEPENDS
        padbs_pattern_generators
)

create_probfd_library(
    NAME papdbs_classical_generator_plugin
    HELP "Classical pattern collection generator adapter plugin"
    SOURCES
        probfd_plugins/pdbs/pattern_collection_generator_classical
    DEPENDS
        papdbs_classical_generator
        padbs_pattern_generators_plugins
)

create_probfd_library(
    NAME papdbs_systematic_generator
    HELP "Systematic pattern collection generator"
    SOURCES
        probfd/pdbs/pattern_collection_generator_systematic
    DEPENDS
        padbs_pattern_generators
)

create_probfd_library(
    NAME papdbs_systematic_generator_plugin
    HELP "Systematic pattern collection generator plugin"
    SOURCES
        probfd_plugins/pdbs/pattern_collection_generator_systematic
    DEPENDS
        papdbs_systematic_generator
        padbs_pattern_generators_plugins
)

create_probfd_library(
    NAME papdbs_hillclimbing_generator
    HELP "Hillclimbing pattern collection generator for PPDBs"
    SOURCES
        probfd/pdbs/pattern_collection_generator_hillclimbing
    DEPENDS
        padbs_pattern_generators
)

create_probfd_library(
    NAME papdbs_hillclimbing_generator_plugin
    HELP "Hillclimbing pattern collection generator for PPDBs plugin"
    SOURCES
        probfd_plugins/pdbs/pattern_collection_generator_hillclimbing
    DEPENDS
        papdbs_hillclimbing_generator
        padbs_pattern_generators_plugins
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
    DEPENDS
        padbs_pattern_generators
)

create_probfd_library(
    NAME papdbs_cegar_plugin
    HELP "Disjoint CEGAR pattern collection generator for PPDBs plugin"
    SOURCES
        probfd_plugins/pdbs/cegar_options
        probfd_plugins/pdbs/cegar/bfs_flaw_finder
        probfd_plugins/pdbs/cegar/pucs_flaw_finder
        probfd_plugins/pdbs/cegar/sampling_flaw_finder
        probfd_plugins/pdbs/cegar/flaw_finding_strategy
    DEPENDS
        papdbs_cegar
        padbs_pattern_generators_plugins
)

create_probfd_library(
    NAME papdbs_disjoint_cegar_generator
    HELP "Disjoint CEGAR pattern collection generator for PPDBs"
    SOURCES
        probfd/pdbs/pattern_collection_generator_disjoint_cegar
    DEPENDS
        papdbs_cegar
)

create_probfd_library(
    NAME papdbs_disjoint_cegar_generator_plugin
    HELP "Disjoint CEGAR pattern collection generator for PPDBs plugin"
    SOURCES
        probfd_plugins/pdbs/pattern_collection_generator_disjoint_cegar
    DEPENDS
        papdbs_disjoint_cegar_generator
        padbs_pattern_generators_plugins
)

create_probfd_library(
    NAME papdbs_multiple_cegar_generator
    HELP "Multiple CEGAR pattern collection generator for PPDBs"
    SOURCES
        probfd/pdbs/pattern_collection_generator_multiple_cegar
    DEPENDS
        papdbs_cegar
)

create_probfd_library(
    NAME papdbs_multiple_cegar_generator_plugin
    HELP "Multiple CEGAR pattern collection generator for PPDBs plugin"
    SOURCES
        probfd_plugins/pdbs/pattern_collection_generator_multiple_cegar
    DEPENDS
        papdbs_multiple_cegar_generator
        padbs_pattern_generators_plugins
)

create_probfd_library(
    NAME probability_aware_pdb_heuristic
    HELP "Probability-aware PDB heuristic"
    SOURCES
        probfd/heuristics/probability_aware_pdb_heuristic
    DEPENDS
        probability_aware_pdbs
        padbs_pattern_generators
)

create_probfd_library(
    NAME scp_pdb_heuristic
    HELP "Saturated Cost-Partitioning heuristic for probability-aware PDBs"
    SOURCES
        probfd/heuristics/scp_heuristic
    DEPENDS
        probability_aware_pdbs
        padbs_pattern_generators
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

create_probfd_library(
    NAME probability_aware_cartesian_abstraction_heuristic
    HELP "Probability-aware Cartesian Abstraction heuristic"
    SOURCES
    probfd/heuristics/additive_cartesian_heuristic
    DEPENDS pa_cartesian_abstractions
)

create_probfd_library(
    NAME pa_cartesian_abstractions_plugins
    HELP "Probability-aware Cartesian Abstractions plugins"
    SOURCES
        probfd_plugins/cartesian_abstractions/adaptive_flaw_generator
        probfd_plugins/cartesian_abstractions/flaw_generator
        probfd_plugins/cartesian_abstractions/policy_based_flaw_generator
        probfd_plugins/cartesian_abstractions/split_selector
        probfd_plugins/cartesian_abstractions/subtask_generators
    DEPENDS
        pa_cartesian_abstractions
)

create_probfd_library(
    NAME probability_aware_cartesian_abstraction_heuristic_plugin
    HELP "Probability-aware Cartesian Abstraction heuristic plugins"
    SOURCES
        probfd_plugins/heuristics/additive_cartesian_heuristic
    DEPENDS
        probability_aware_cartesian_abstraction_heuristic
)

create_probfd_library(
    NAME constant_evaluator_plugin
    HELP "Constant evaluator plugin"
    SOURCES
        probfd_plugins/heuristics/constant_evaluator
    DEPENDS
        probfd_core
)

create_probfd_library(
    NAME dead_end_pruning_heuristic_plugins
    HELP "Dead-end pruning heuristic plugin"
    SOURCES
        probfd_plugins/heuristics/dead_end_pruning
    DEPENDS
        deadend_pruning_heuristic
)

create_probfd_library(
    NAME determinization_heuristic_plugin
    HELP "Determinization heuristic plugin"
    SOURCES
        probfd_plugins/heuristics/determinization_cost
    DEPENDS
        determinization_heuristic
)

create_probfd_library(
    NAME gzocp_heuristic_plugin
    HELP "PDB Greedy Zero-One Cost-Partitioning heuristic plugin"
    SOURCES
        probfd_plugins/heuristics/gzocp_heuristic
    DEPENDS
        gzocp_pdb_heuristic
)

create_probfd_library(
    NAME papdb_heuristics_plugin
    HELP "Probability-Aware PDB heuristic plugin"
    SOURCES
        probfd_plugins/heuristics/probability_aware_pdb_heuristic
    DEPENDS
        probability_aware_pdb_heuristic
)

create_probfd_library(
    NAME scp_heuristic_plugin
    HELP "PDB saturated cost-partitioning heuristic plugin"
    SOURCES
        probfd_plugins/heuristics/scp_heuristic
    DEPENDS
        scp_pdb_heuristic
)

create_probfd_library(
    NAME ucp_heuristic_plugin
    HELP "PDB uniform cost-partitioning heuristic plugin"
    SOURCES
        probfd_plugins/heuristics/ucp_heuristic
    DEPENDS
        ucp_pdb_heuristic
)

create_probfd_library(
    NAME occupation_measure_heuristics_plugins
    HELP "Occupation measure heuristics plugins"
    SOURCES
        probfd_plugins/occupation_measures/subcategory
    DEPENDS
        occupation_measures
)

create_probfd_library(
    NAME open_list_plugins
    HELP "Open list plugins"
    SOURCES
        probfd_plugins/open_lists/subcategory
    DEPENDS
        mdp_heuristic_search_base
)

create_probfd_library(
    NAME successor_sampler_plugins
    HELP "Successor sampler plugins"
    SOURCES
        probfd_plugins/successor_samplers/subcategory
    DEPENDS
        mdp_heuristic_search_base
)

create_probfd_library(
    NAME policy_picker_plugins
    HELP "Policy pickers plugins"
    SOURCES
        probfd_plugins/policy_pickers/subcategory
    DEPENDS
        mdp_heuristic_search_base
)

create_probfd_library(
    NAME transition_sorter_plugins
    HELP "Transition sorter plugins"
    SOURCES
        probfd_plugins/transition_sorters/subcategory
    DEPENDS
        mdp_heuristic_search_base
)