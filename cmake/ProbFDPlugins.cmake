create_library(
    NAME probfd_core_plugin
    HELP "Enables core ProbFD plugin"
    SOURCES
        probfd_plugins/probabilistic_task
        probfd_plugins/solver_interface
        probfd_plugins/task_evaluator_factory
    DEPENDS
        probfd_core
    TARGET probfd
)

create_library(
    NAME core_probabilistic_task_plugin
    HELP "Enables core probabilistic task transformation plugin"
    SOURCES
        probfd_plugins/tasks/root_task
    DEPENDS
        probfd_core
        core_probabilistic_tasks
    TARGET probfd
)

create_library(
    NAME mdp_solver_options
    HELP "Enables plugin options for MDP solvers"
    SOURCES
        probfd_plugins/solvers/mdp_solver
    DEPENDS
        probfd_core
    TARGET probfd
)

create_library(
    NAME acyclic_value_iteration_solver_plugin
    HELP "Enables the acyclic value iteration solver plugin"
    SOURCES
        probfd_plugins/solvers/acyclic_vi
    DEPENDS
       mdp_solver_options
    TARGET probfd
)

create_library(
    NAME topological_value_iteration_solver_plugin
    HELP "Enables the topological value iteration solver plugin"
    SOURCES
        probfd_plugins/solvers/topological_vi
    DEPENDS
        mdp_solver_options
    TARGET probfd
)

create_library(
    NAME trap_aware_topological_value_iteration_solver_plugin
    HELP "Enables the trap-aware topological value iteration solver plugin"
    SOURCES
        probfd_plugins/solvers/ta_topological_vi
    DEPENDS
        mdp_solver_options
    TARGET probfd
)

create_library(
    NAME interval_iteration_solver_plugin
    HELP "Enables the interval iteration solver plugin"
    SOURCES
        probfd_plugins/solvers/interval_iteration
    DEPENDS
        mdp_solver_options
    TARGET probfd
)

create_library(
    NAME idual_solver_plugin
    HELP "Enables the i-dual solver plugin"
    SOURCES
        probfd_plugins/solvers/idual
    DEPENDS
        mdp_solver_options
        lp_solver
    TARGET probfd
)

create_library(
    NAME i2dual_solver_plugin
    HELP "Enables the i^2-dual solver plugin"
    SOURCES
        probfd_plugins/solvers/i2dual
    DEPENDS
        i2dual_solver
        mdp_solver_options
    TARGET probfd
)

create_library(
    NAME bisimulation_based_solver_plugin
    HELP "Enables the bisimulation-based value iteration solver plugin"
    SOURCES
        probfd_plugins/solvers/bisimulation_vi
    DEPENDS
        probfd_core
        mdp_solver_options
    TARGET probfd
)

create_library(
    NAME mdp_heuristic_search_base_options
    HELP "Enables the mdp heuristic search core plugin options"
    SOURCES
        probfd_plugins/solvers/mdp_heuristic_search
    DEPENDS
        mdp_heuristic_search_base
        mdp_solver_options
    TARGET probfd
)

create_library(
    NAME ao_search_plugin
    HELP "Enables the AO* solver variant plugin"
    SOURCES
        probfd_plugins/solvers/aostar
        probfd_plugins/solvers/exhaustive_ao
    DEPENDS
        mdp_heuristic_search_base_options
        probfd_core
    TARGET probfd
)

create_library(
    NAME exhaustive_dfhs_plugin
    HELP "Enables the exhaustive depth-first heuristic search solver plugin"
    SOURCES
        probfd_plugins/solvers/exhaustive_dfs
    DEPENDS
        mdp_heuristic_search_base_options
    TARGET probfd
)

create_library(
    NAME lrtdp_solver_plugin
    HELP "Enables the labeled real-time dynamic programming solver plugin"
    SOURCES
        probfd_plugins/solvers/lrtdp
    DEPENDS
        mdp_heuristic_search_base_options
        probfd_core
    TARGET probfd
)

create_library(
    NAME trap_aware_lrtdp_solver_plugin
    HELP "Enables the trap-Aware LRTDP solver plugin"
    SOURCES
        probfd_plugins/solvers/talrtdp
    DEPENDS
        mdp_heuristic_search_base_options
    TARGET probfd
)

create_library(
    NAME trap_aware_dfhs_solver_plugin
    HELP "Enables the trap-Aware DFHS solver plugin"
    SOURCES
        probfd_plugins/solvers/ta_depth_first_heuristic_search
    DEPENDS
        mdp_heuristic_search_base_options
    TARGET probfd
)

create_library(
    NAME dfhs_solver_plugin
    HELP "Enables the depth-first heuristic search solver plugin"
    SOURCES
        probfd_plugins/solvers/depth_first_heuristic_search
    DEPENDS
        mdp_heuristic_search_base_options
        probfd_core
    TARGET probfd
)

create_library(
    NAME task_dependent_heuristic_plugin
    SOURCES
        probfd_plugins/heuristics/task_dependent_heuristic
    DEPENDS
        task_dependent_heuristic
)

create_library(
    NAME padbs_pattern_generators_plugin
    HELP "Enables the base plugin for pattern collection generation for PPDBs"
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
    TARGET
        probfd
)

create_library(
    NAME papdbs_classical_generator_plugin
    HELP "Enables the classical pattern collection generator adapter plugin"
    SOURCES
        probfd_plugins/pdbs/pattern_collection_generator_classical
    DEPENDS
        papdbs_classical_generator
        padbs_pattern_generators_plugin
    TARGET probfd
)

create_library(
    NAME papdbs_systematic_generator_plugin
    HELP "Enables the systematic pattern collection generator plugin"
    SOURCES
        probfd_plugins/pdbs/pattern_collection_generator_systematic
    DEPENDS
        papdbs_systematic_generator
        padbs_pattern_generators_plugin
    TARGET probfd
)

create_library(
    NAME papdbs_hillclimbing_generator_plugin
    HELP "Enables the hillclimbing pattern collection generator for PPDBs
    plugin"
    SOURCES
        probfd_plugins/pdbs/pattern_collection_generator_hillclimbing
    DEPENDS
        papdbs_hillclimbing_generator
        padbs_pattern_generators_plugin
    TARGET probfd
)

create_library(
    NAME papdbs_cegar_plugin
    HELP "Enables the disjoint CEGAR pattern collection generator for PPDBs
    plugin"
    SOURCES
        probfd_plugins/pdbs/cegar_options
        probfd_plugins/pdbs/cegar/bfs_flaw_finder
        probfd_plugins/pdbs/cegar/pucs_flaw_finder
        probfd_plugins/pdbs/cegar/sampling_flaw_finder
        probfd_plugins/pdbs/cegar/flaw_finding_strategy
    DEPENDS
        papdbs_cegar
        padbs_pattern_generators_plugin
    TARGET probfd
)

create_library(
    NAME papdbs_disjoint_cegar_generator_plugin
    HELP "Enables the disjoint CEGAR pattern collection generator for PPDBs
    plugin"
    SOURCES
        probfd_plugins/pdbs/pattern_collection_generator_disjoint_cegar
    DEPENDS
        papdbs_disjoint_cegar_generator
        padbs_pattern_generators_plugin
    TARGET probfd
)

create_library(
    NAME papdbs_multiple_cegar_generator_plugin
    HELP "Enables the multiple CEGAR pattern collection generator for PPDBs
    plugin"
    SOURCES
        probfd_plugins/pdbs/pattern_collection_generator_multiple_cegar
    DEPENDS
        papdbs_multiple_cegar_generator
        padbs_pattern_generators_plugin
    TARGET probfd
)

create_library(
    NAME pa_cartesian_abstractions_plugin
    HELP "Enables the probability-aware Cartesian Abstractions plugin"
    SOURCES
        probfd_plugins/cartesian_abstractions/adaptive_flaw_generator
        probfd_plugins/cartesian_abstractions/flaw_generator
        probfd_plugins/cartesian_abstractions/policy_based_flaw_generator
        probfd_plugins/cartesian_abstractions/split_selector
        probfd_plugins/cartesian_abstractions/subtask_generators
    DEPENDS
        pa_cartesian_abstractions
    TARGET probfd
)

create_library(
    NAME probability_aware_cartesian_abstraction_heuristic_plugin
    HELP "Enables the probability-aware Cartesian Abstraction heuristic plugin"
    SOURCES
        probfd_plugins/heuristics/additive_cartesian_heuristic
    DEPENDS
        task_dependent_heuristic_plugin
        probability_aware_cartesian_abstraction_heuristic
    TARGET probfd
)

create_library(
    NAME constant_evaluator_plugin
    HELP "Enables the constant evaluator plugin"
    SOURCES
        probfd_plugins/heuristics/constant_evaluator
    DEPENDS
        probfd_core
    TARGET probfd
)

create_library(
    NAME dead_end_pruning_heuristic_plugin
    HELP "Enables the dead-end pruning heuristic plugin"
    SOURCES
        probfd_plugins/heuristics/dead_end_pruning
    DEPENDS
        deadend_pruning_heuristic
    TARGET probfd
)

create_library(
    NAME determinization_heuristic_plugin
    HELP "Enables the determinization heuristic plugin"
    SOURCES
        probfd_plugins/heuristics/determinization_cost
    DEPENDS
        determinization_heuristic
    TARGET probfd
)

create_library(
    NAME gzocp_heuristic_plugin
    HELP "Enables the PDB Greedy Zero-One Cost-Partitioning heuristic plugin"
    SOURCES
        probfd_plugins/heuristics/gzocp_heuristic
    DEPENDS
        gzocp_pdb_heuristic
    TARGET probfd
)

create_library(
    NAME papdb_heuristics_plugin
    HELP "Enables the probability-aware PDB heuristic plugin"
    SOURCES
        probfd_plugins/heuristics/probability_aware_pdb_heuristic
    DEPENDS
        probability_aware_pdb_heuristic
    TARGET probfd
)

create_library(
    NAME scp_heuristic_plugin
    HELP "Enables the PDB saturated cost-partitioning heuristic plugin"
    SOURCES
        probfd_plugins/heuristics/scp_heuristic
    DEPENDS
        scp_pdb_heuristic
    TARGET probfd
)

create_library(
    NAME ucp_heuristic_plugin
    HELP "Enables the PDB uniform cost-partitioning heuristic plugin"
    SOURCES
        probfd_plugins/heuristics/ucp_heuristic
    DEPENDS
        ucp_pdb_heuristic
    TARGET probfd
)

create_library(
    NAME occupation_measure_heuristics_plugin
    HELP "Enables the Occupation measure heuristics plugin"
    SOURCES
        probfd_plugins/occupation_measures/subcategory
    DEPENDS
        occupation_measure_heuristic
    TARGET probfd
)

create_library(
    NAME open_list_plugin
    HELP "Enables the open list plugin"
    SOURCES
        probfd_plugins/open_lists/subcategory
    DEPENDS
        mdp_heuristic_search_base
    TARGET probfd
)

create_library(
    NAME successor_sampler_plugin
    HELP "Enables the successor sampler plugin"
    SOURCES
        probfd_plugins/successor_samplers/subcategory
    DEPENDS
        mdp_heuristic_search_base
    TARGET probfd
)

create_library(
    NAME policy_picker_plugin
    HELP "Enables the policy pickers plugin"
    SOURCES
        probfd_plugins/policy_pickers/subcategory
    DEPENDS
        mdp_heuristic_search_base
    TARGET probfd
)

create_library(
    NAME transition_sorter_plugin
    HELP "Enables the transition sorter plugin"
    SOURCES
        probfd_plugins/transition_sorters/subcategory
    DEPENDS
        mdp_heuristic_search_base
    TARGET probfd
)