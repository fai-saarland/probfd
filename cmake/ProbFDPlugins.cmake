create_library(
    NAME probfd_core_plugin
    HELP "Enables core ProbFD plugin"
    SOURCES
        probfd/cli/solver_interface
        probfd/cli/task_heuristic_factory
    DEPENDS
        probfd_core
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME core_probabilistic_task_plugin
    HELP "Enables core probabilistic task transformation plugin"
    SOURCES
        probfd/cli/tasks/root_task
    DEPENDS
        probfd_core
        core_probabilistic_tasks
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME task_state_space_factory_category
    HELP "Category plugin for task state space factories"
    SOURCES
        probfd/cli/task_state_space_factory_category
    DEPENDS
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME task_state_space_factory_features
    HELP "Enables all task state space factory features"
    SOURCES
        probfd/cli/task_state_space_factory_features
    DEPENDS
        task_state_space_factory_category
        probfd_core
        parser
        plugins
        logging_options
    TARGET
        probfd
)

create_library(
    NAME mdp_solver_options
    HELP "Enables plugin options for MDP solvers"
    SOURCES
        probfd/cli/solvers/mdp_solver
        probfd/cli/solvers/statistical_mdp_algorithm_factory_category
    DEPENDS
        probfd_core
        parser
        plugins
        task_state_space_factory_category
        logging_options
    TARGET
        probfd
)

create_library(
    NAME mdp_heuristic_search_base_options
    HELP "Enables the mdp heuristic search core plugin options"
    SOURCES
        probfd/cli/solvers/mdp_heuristic_search
    DEPENDS
        mdp_heuristic_search_base
        mdp_solver_options
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME acyclic_value_iteration_solver_plugin
    HELP "Enables the acyclic value iteration solver plugin"
    SOURCES
        probfd/cli/solvers/acyclic_vi
    DEPENDS
        mdp_solver_options
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME ao_star_plugin
    HELP "Enables the AO* solver plugin"
    SOURCES
        probfd/cli/solvers/aostar
    DEPENDS
        mdp_heuristic_search_base_options
        probfd_core
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME bisimulation_based_solver_plugin
    HELP "Enables the bisimulation-based value iteration solver plugin"
    SOURCES
        probfd/cli/solvers/bisimulation_vi
    DEPENDS
        probfd_core
        mdp_solver_options
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME dfhs_solver_plugin
    HELP "Enables the depth-first heuristic search solver plugin"
    SOURCES
        probfd/cli/solvers/depth_first_heuristic_search
    DEPENDS
        mdp_heuristic_search_base_options
        probfd_core
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME exhaustive_ao_star_plugin
    HELP "Enables the exhaustive AO* solver plugin"
    SOURCES
        probfd/cli/solvers/exhaustive_ao
    DEPENDS
        mdp_heuristic_search_base_options
        probfd_core
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME exhaustive_dfhs_plugin
    HELP "Enables the exhaustive depth-first heuristic search solver plugin"
    SOURCES
        probfd/cli/solvers/exhaustive_dfs
    DEPENDS
        mdp_heuristic_search_base_options
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME idual_solver_plugin
    HELP "Enables the i-dual solver plugin"
    SOURCES
        probfd/cli/solvers/idual
    DEPENDS
        mdp_solver_options
        lp_solver_options
        lp_solver
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME i2dual_solver_plugin
    HELP "Enables the i^2-dual solver plugin"
    SOURCES
        probfd/cli/solvers/i2dual
    DEPENDS
        i2dual_solver
        lp_solver_options
        mdp_solver_options
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME interval_iteration_solver_plugin
    HELP "Enables the interval iteration solver plugin"
    SOURCES
        probfd/cli/solvers/interval_iteration
    DEPENDS
        mdp_solver_options
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME lrtdp_solver_plugin
    HELP "Enables the labeled real-time dynamic programming solver plugin"
    SOURCES
        probfd/cli/solvers/lrtdp
    DEPENDS
        mdp_heuristic_search_base_options
        probfd_core
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME trap_aware_dfhs_solver_plugin
    HELP "Enables the trap-Aware DFHS solver plugin"
    SOURCES
        probfd/cli/solvers/ta_depth_first_heuristic_search
    DEPENDS
        mdp_heuristic_search_base_options
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME trap_aware_lrtdp_solver_plugin
    HELP "Enables the trap-Aware LRTDP solver plugin"
    SOURCES
        probfd/cli/solvers/ta_lrtdp
    DEPENDS
        mdp_heuristic_search_base_options
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME trap_aware_topological_value_iteration_solver_plugin
    HELP "Enables the trap-aware topological value iteration solver plugin"
    SOURCES
        probfd/cli/solvers/ta_topological_vi
    DEPENDS
        mdp_solver_options
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME task_dependent_heuristic_options
    SOURCES
        probfd/cli/heuristics/task_dependent_heuristic_options
    DEPENDS
        task_dependent_heuristic
        parser
        plugins
        logging_options
)

create_library(
    NAME topological_value_iteration_solver_plugin
    HELP "Enables the topological value iteration solver plugin"
    SOURCES
        probfd/cli/solvers/topological_vi
    DEPENDS
        mdp_solver_options
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME papdbs_pattern_generators_plugin
    HELP "Enables the base plugin for pattern collection generation for PPDBs"
    SOURCES
        probfd/cli/pdbs/pattern_generator
        probfd/cli/pdbs/pattern_generator_category
        probfd/cli/pdbs/pattern_collection_generator
        probfd/cli/pdbs/pattern_collection_generator_category
        probfd/cli/pdbs/pattern_collection_generator_multiple

        probfd/cli/pdbs/subcollection_finder_factory
        probfd/cli/pdbs/max_orthogonal_finder_factory
        probfd/cli/pdbs/trivial_finder_factory
        probfd/cli/pdbs/fully_additive_finder_factory
    DEPENDS
        padbs_pattern_generators
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME papdbs_classical_generator_plugin
    HELP "Enables the classical pattern collection generator adapter plugin"
    SOURCES
        probfd/cli/pdbs/pattern_collection_generator_classical
    DEPENDS
        pattern_generator_category
        papdbs_pattern_generators_plugin
        papdbs_classical_generator
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME papdbs_systematic_generator_plugin
    HELP "Enables the systematic pattern collection generator plugin"
    SOURCES
        probfd/cli/pdbs/pattern_collection_generator_systematic
    DEPENDS
        papdbs_systematic_generator
        papdbs_pattern_generators_plugin
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME papdbs_hillclimbing_generator_plugin
    HELP "Enables the hillclimbing pattern collection generator for PPDBs
    plugin"
    SOURCES
        probfd/cli/pdbs/pattern_collection_generator_hillclimbing
    DEPENDS
        papdbs_hillclimbing_generator
        papdbs_pattern_generators_plugin
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME papdbs_cegar_plugin
    HELP "Enables the disjoint CEGAR pattern collection generator for PPDBs
    plugin"
    SOURCES
        probfd/cli/pdbs/cegar_options
        probfd/cli/pdbs/cegar/bfs_flaw_finder
        probfd/cli/pdbs/cegar/pucs_flaw_finder
        probfd/cli/pdbs/cegar/sampling_flaw_finder
        probfd/cli/pdbs/cegar/flaw_finding_strategy
    DEPENDS
        papdbs_cegar
        papdbs_pattern_generators_plugin
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME papdbs_disjoint_cegar_generator_plugin
    HELP "Enables the disjoint CEGAR pattern collection generator for PPDBs
    plugin"
    SOURCES
        probfd/cli/pdbs/pattern_collection_generator_disjoint_cegar
    DEPENDS
        papdbs_disjoint_cegar_generator
        papdbs_pattern_generators_plugin
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME papdbs_multiple_cegar_generator_plugin
    HELP "Enables the multiple CEGAR pattern collection generator for PPDBs
    plugin"
    SOURCES
        probfd/cli/pdbs/pattern_collection_generator_multiple_cegar
    DEPENDS
        papdbs_multiple_cegar_generator
        papdbs_pattern_generators_plugin
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pa_cartesian_abstractions_plugin
    HELP "Enables the probability-aware Cartesian Abstractions plugin"
    SOURCES
        probfd/cli/cartesian_abstractions/adaptive_flaw_generator
        probfd/cli/cartesian_abstractions/flaw_generator
        probfd/cli/cartesian_abstractions/trace_based_flaw_generator
        probfd/cli/cartesian_abstractions/policy_based_flaw_generator
        probfd/cli/cartesian_abstractions/split_selector
        probfd/cli/cartesian_abstractions/subtask_generators
    DEPENDS
        pa_cartesian_abstractions
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME probability_aware_cartesian_abstraction_heuristic_plugin
    HELP "Enables the probability-aware Cartesian Abstraction heuristic plugin"
    SOURCES
        probfd/cli/heuristics/additive_cartesian_heuristic
    DEPENDS
        evaluator_category
        task_dependent_heuristic_options
        probability_aware_cartesian_abstraction_heuristic
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME constant_heuristic_plugin
    HELP "Enables the constant heuristic plugin"
    SOURCES
        probfd/cli/heuristics/constant_heuristic
    DEPENDS
        evaluator_category
        probfd_core
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME dead_end_pruning_heuristic_plugin
    HELP "Enables the dead-end pruning heuristic plugin"
    SOURCES
        probfd/cli/heuristics/dead_end_pruning_heuristic
    DEPENDS
        evaluator_category
        deadend_pruning_heuristic
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME determinization_heuristic_plugin
    HELP "Enables the determinization heuristic plugin"
    SOURCES
        probfd/cli/heuristics/determinization_cost_heuristic
    DEPENDS
        evaluator_category
        determinization_heuristic
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME gzocp_heuristic_plugin
    HELP "Enables the PDB Greedy Zero-One Cost-Partitioning heuristic plugin"
    SOURCES
        probfd/cli/heuristics/gzocp_heuristic
    DEPENDS
        gzocp_pdb_heuristic
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME papdb_heuristics_plugin
    HELP "Enables the probability-aware PDB heuristic plugin"
    SOURCES
        probfd/cli/heuristics/probability_aware_pdb_heuristic
    DEPENDS
        probability_aware_pdb_heuristic
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME scp_heuristic_plugin
    HELP "Enables the PDB saturated cost-partitioning heuristic plugin"
    SOURCES
        probfd/cli/heuristics/scp_heuristic
    DEPENDS
        scp_pdb_heuristic
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME ucp_heuristic_plugin
    HELP "Enables the PDB uniform cost-partitioning heuristic plugin"
    SOURCES
        probfd/cli/heuristics/ucp_heuristic
    DEPENDS
        ucp_pdb_heuristic
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME occupation_measure_heuristics_plugin
    HELP "Enables the Occupation measure heuristics plugin"
    SOURCES
        probfd/cli/occupation_measures/constraint_generator_factory
        probfd/cli/occupation_measures/subcategory
    DEPENDS
        occupation_measure_heuristic
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pa_merge_and_shrink_algorithm_options
    HELP "Enables the Merge-and-shrink algorithm options"
    SOURCES
        probfd/cli/merge_and_shrink/merge_and_shrink_algorithm_options
    TARGET
        probfd
)

create_library(
    NAME pmerge_strategy_factory_category
    HELP "Enables merge strategy factory category plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_strategy_factory_category
    DEPENDS
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_strategy_factory_options
    HELP "Enables the base merge strategy factory options"
    SOURCES
        probfd/cli/merge_and_shrink/merge_strategy_factory_options
    DEPENDS
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_strategy_factory_precomputed
    HELP "Enables precomputed merge strategy factory plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_strategy_factory_precomputed
    DEPENDS
        pmerge_strategy_factory_category
        pmerge_strategy_factory_options
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_strategy_factory_sccs
    HELP "Enables merge strategy factory SCC plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_strategy_factory_sccs
    DEPENDS
        pmerge_strategy_factory_category
        pmerge_strategy_factory_options
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_strategy_factory_stateless
    HELP "Enables stateless merge strategy factory plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_strategy_factory_stateless
    DEPENDS
        pmerge_strategy_factory_category
        pmerge_strategy_factory_options
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_selector_category
    HELP "Enables merge selector category plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_selector_category
    DEPENDS
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_selector_score_based_filtering_feature
    HELP "Enables merge selector score-based plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_selector_score_based_filtering
    DEPENDS
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_scoring_function_category
    HELP "Enables merge scoring function category plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_scoring_function_category
    DEPENDS
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_scoring_function_dfp_feature
    HELP "Enables merge scoring function dfp plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_scoring_function_dfp
    DEPENDS
        pmerge_scoring_function_category
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_scoring_function_goal_relevance_feature
    HELP "Enables merge scoring function goal relevance plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_scoring_function_goal_relevance
    DEPENDS
        pmerge_scoring_function_category
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_scoring_function_miasm_feature
    HELP "Enables merge scoring function miasm plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_scoring_function_miasm
    DEPENDS
        pmerge_scoring_function_category
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_scoring_function_single_random_feature
    HELP "Enables merge scoring function single random plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_scoring_function_single_random
    DEPENDS
        pmerge_scoring_function_category
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_scoring_function_total_order_feature
    HELP "Enables merge scoring function total order plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_scoring_function_total_order
    DEPENDS
        pmerge_scoring_function_category
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_tree_factory_category
    HELP "Enables merge tree factory category plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_tree_factory_category
    DEPENDS
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_tree_factory_options
    HELP "Enables merge tree factory options"
    SOURCES
        probfd/cli/merge_and_shrink/merge_tree_factory_options
    DEPENDS
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pmerge_tree_factory_linear
    HELP "Enables linear merge tree factory plugin"
    SOURCES
        probfd/cli/merge_and_shrink/merge_tree_factory_linear
    DEPENDS
        pmerge_tree_factory_category
        pmerge_tree_factory_options
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pshrink_strategy_category
    HELP "Enables shrink strategy category plugin"
    SOURCES
        probfd/cli/merge_and_shrink/shrink_strategy_category
    DEPENDS
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pshrink_strategy_bucket_based_options
    HELP "Enables bucket-based shrink strategy options"
    SOURCES
        probfd/cli/merge_and_shrink/shrink_strategy_bucket_based_options
    DEPENDS
        pa_merge_and_shrink
        pshrink_strategy_category
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pshrink_strategy_equal_distance_feature
    HELP "Enables equal distance shrink strategy plugin"
    SOURCES
        probfd/cli/merge_and_shrink/shrink_strategy_equal_distance
    DEPENDS
        pa_merge_and_shrink
        pshrink_strategy_category
        pshrink_strategy_bucket_based_options
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pshrink_strategy_random_feature
    HELP "Enables random shrink strategy plugin"
    SOURCES
        probfd/cli/merge_and_shrink/shrink_strategy_random
    DEPENDS
        pa_merge_and_shrink
        pshrink_strategy_category
        pshrink_strategy_bucket_based_options
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pshrink_strategy_bisimulation_feature
    HELP "Enables bisimulation shrink strategy plugin"
    SOURCES
        probfd/cli/merge_and_shrink/shrink_strategy_bisimulation
    DEPENDS
        pa_merge_and_shrink
        pshrink_strategy_category
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pshrink_strategy_probabilistic_bisimulation_feature
    HELP "Enables probabilistic bisimulation shrink strategy plugin"
    SOURCES
        probfd/cli/merge_and_shrink/shrink_strategy_probabilistic_bisimulation
    DEPENDS
        pa_merge_and_shrink
        pshrink_strategy_category
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME prune_strategy_category
    HELP "Enables prune strategy category plugin"
    SOURCES
        probfd/cli/merge_and_shrink/prune_strategy_category
    DEPENDS
        pa_merge_and_shrink
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME prune_alive_feature
    HELP "Enables alive prune strategy plugin"
    SOURCES
        probfd/cli/merge_and_shrink/prune_strategy_alive
    DEPENDS
        pa_merge_and_shrink
        prune_strategy_category
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME prune_solvable_feature
    HELP "Enables solvable prune strategy plugin"
    SOURCES
        probfd/cli/merge_and_shrink/prune_strategy_solvable
    DEPENDS
        pa_merge_and_shrink
        prune_strategy_category
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME prune_identity_feature
    HELP "Enables identity prune strategy plugin"
    SOURCES
        probfd/cli/merge_and_shrink/prune_strategy_identity
    DEPENDS
        pa_merge_and_shrink
        prune_strategy_category
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME pa_merge_and_shrink_heuristic_feature
    HELP "Enables the Merge-and-shrink heuristic plugin"
    SOURCES
        probfd/cli/heuristics/merge_and_shrink_heuristic
    DEPENDS
        probability_aware_mas_heuristic
        pa_merge_and_shrink_algorithm_options
    TARGET
        probfd
)

create_library(
    NAME open_list_plugin
    HELP "Enables the open list plugin"
    SOURCES
        probfd/cli/open_lists/subcategory
    DEPENDS
        mdp_heuristic_search_base
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME successor_sampler_plugin
    HELP "Enables the successor sampler plugin"
    SOURCES
        probfd/cli/successor_samplers/subcategory
    DEPENDS
        mdp_heuristic_search_base
        rng_options
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME policy_picker_plugin
    HELP "Enables the policy pickers plugin"
    SOURCES
        probfd/cli/policy_pickers/subcategory
    DEPENDS
        mdp_heuristic_search_base
        parser
        plugins
    TARGET
        probfd
)

create_library(
    NAME transition_sorter_plugin
    HELP "Enables the transition sorter plugin"
    SOURCES
        probfd/cli/transition_sorters/subcategory
    DEPENDS
        mdp_heuristic_search_base
        parser
        plugins
    TARGET
        probfd
)