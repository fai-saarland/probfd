(define (domain roverdom--s717101--m25--r2--w6--o3--c1--g4--p0--P1--Z200--X200--C10) (:requirements :typing) (:types rover waypoint store camera mode lander objective road energy) (:predicates (at ?x - rover ?y - waypoint) (at_lander ?x - lander ?y - waypoint) (can_traverse ?r - rover ?x - waypoint ?y - waypoint) (equipped_for_soil_analysis ?r - rover) (equipped_for_rock_analysis ?r - rover) (equipped_for_imaging ?r - rover) (empty ?s - store) (have_rock_analysis ?r - rover ?w - waypoint) (have_soil_analysis ?r - rover ?w - waypoint) (full ?s - store) (calibrated ?c - camera ?r - rover) (supports ?c - camera ?m - mode) (available ?r - rover) (visible ?w - waypoint ?p - waypoint) (have_image ?r - rover ?o - objective ?m - mode) (communicated_soil_data ?w - waypoint) (communicated_rock_data ?w - waypoint) (communicated_image_data ?o - objective ?m - mode) (at_soil_sample ?w - waypoint) (at_rock_sample ?w - waypoint) (visible_from ?o - objective ?w - waypoint) (store_of ?s - store ?r - rover) (calibration_target ?i - camera ?o - objective) (on_board ?i - camera ?r - rover) (channel_free ?l - lander) (road_isunknown ?r - road) (road_isfree ?r - road) (road_isblocked ?r - road) (available_energy ?x - rover ?e - energy) (diff ?e0 ?e1 ?e2 - energy)) (:functions (total-cost)) (:constants road0 road1 road2 road3 road4 road5 road6 road7 road8 road9 road10 road11 - road waypoint0 waypoint1 waypoint2 waypoint3 waypoint4 waypoint5 - waypoint e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 e10 e11 e12 e13 e14 e15 e16 e17 e18 e19 e20 e21 e22 e23 e24 e25 e26 e27 e28 e29 e30 e31 e32 e33 e34 e35 e36 e37 e38 e39 e40 e41 e42 e43 e44 e45 e46 e47 e48 e49 e50 e51 e52 e53 e54 e55 e56 e57 - energy) (:action sample_soil :parameters (?x - rover ?s - store ?p - waypoint) :precondition (and (at ?x ?p) (at_soil_sample ?p) (equipped_for_soil_analysis ?x) (store_of ?s ?x) (empty ?s)) :effect (and (increase (total-cost) 1) (not (empty ?s)) (full ?s) (have_soil_analysis ?x ?p) (not (at_soil_sample ?p)))) (:action sample_rock :parameters (?x - rover ?s - store ?p - waypoint) :precondition (and (at ?x ?p) (at_rock_sample ?p) (equipped_for_rock_analysis ?x) (store_of ?s ?x) (empty ?s)) :effect (and (increase (total-cost) 1) (not (empty ?s)) (full ?s) (have_rock_analysis ?x ?p) (not (at_rock_sample ?p)))) (:action drop :parameters (?x - rover ?y - store) :precondition (and (store_of ?y ?x) (full ?y)) :effect (and (increase (total-cost) 1) (not (full ?y)) (empty ?y))) (:action calibrate :parameters (?r - rover ?i - camera ?t - objective ?w - waypoint) :precondition (and (equipped_for_imaging ?r) (calibration_target ?i ?t) (at ?r ?w) (visible_from ?t ?w) (on_board ?i ?r)) :effect (and (increase (total-cost) 1) (calibrated ?i ?r))) (:action take_image :parameters (?r - rover ?p - waypoint ?o - objective ?i - camera ?m - mode) :precondition (and (calibrated ?i ?r) (on_board ?i ?r) (equipped_for_imaging ?r) (supports ?i ?m) (visible_from ?o ?p) (at ?r ?p)) :effect (and (increase (total-cost) 1) (have_image ?r ?o ?m) (not (calibrated ?i ?r)))) (:action communicate_soil_data :parameters (?r - rover ?l - lander ?p - waypoint ?x - waypoint ?y - waypoint) :precondition (and (at ?r ?x) (at_lander ?l ?y) (have_soil_analysis ?r ?p) (visible ?x ?y) (available ?r) (channel_free ?l)) :effect (and (increase (total-cost) 1) (communicated_soil_data ?p))) (:action communicate_rock_data :parameters (?r - rover ?l - lander ?p - waypoint ?x - waypoint ?y - waypoint) :precondition (and (at ?r ?x) (at_lander ?l ?y) (have_rock_analysis ?r ?p) (visible ?x ?y) (available ?r) (channel_free ?l)) :effect (and (increase (total-cost) 1) (communicated_rock_data ?p))) (:action communicate_image_data :parameters (?r - rover ?l - lander ?o - objective ?m - mode ?x - waypoint ?y - waypoint) :precondition (and (at ?r ?x) (at_lander ?l ?y) (have_image ?r ?o ?m) (visible ?x ?y) (available ?r) (channel_free ?l)) :effect (and (increase (total-cost) 1) (communicated_image_data ?o ?m))) (:action try-navigate-l0-l1 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint0 waypoint1) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint1) (road_isunknown road0) (available_energy ?x ?olde) (diff ?olde e18 ?newe)) :effect (and (increase (total-cost) 18) (not (road_isunknown road0)) (probabilistic 200/1000 (and (road_isblocked road0)) 800/1000 (and (road_isfree road0) (not (at ?x waypoint0)) (at ?x waypoint1) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l0-l1 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint0 waypoint1) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint1) (road_isfree road0) (available_energy ?x ?olde) (diff ?olde e18 ?newe)) :effect (and (increase (total-cost) 18) (not (at ?x waypoint0)) (at ?x waypoint1) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l1-l0 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint1 waypoint0) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint0) (road_isunknown road0) (available_energy ?x ?olde) (diff ?olde e18 ?newe)) :effect (and (increase (total-cost) 18) (not (road_isunknown road0)) (probabilistic 200/1000 (and (road_isblocked road0)) 800/1000 (and (road_isfree road0) (not (at ?x waypoint1)) (at ?x waypoint0) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l1-l0 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint1 waypoint0) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint0) (road_isfree road0) (available_energy ?x ?olde) (diff ?olde e18 ?newe)) :effect (and (increase (total-cost) 18) (not (at ?x waypoint1)) (at ?x waypoint0) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l0-l2 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint0 waypoint2) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint2) (road_isunknown road1) (available_energy ?x ?olde) (diff ?olde e14 ?newe)) :effect (and (increase (total-cost) 14) (not (road_isunknown road1)) (probabilistic 200/1000 (and (road_isblocked road1)) 800/1000 (and (road_isfree road1) (not (at ?x waypoint0)) (at ?x waypoint2) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l0-l2 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint0 waypoint2) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint2) (road_isfree road1) (available_energy ?x ?olde) (diff ?olde e14 ?newe)) :effect (and (increase (total-cost) 14) (not (at ?x waypoint0)) (at ?x waypoint2) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l2-l0 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint2 waypoint0) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint0) (road_isunknown road1) (available_energy ?x ?olde) (diff ?olde e14 ?newe)) :effect (and (increase (total-cost) 14) (not (road_isunknown road1)) (probabilistic 200/1000 (and (road_isblocked road1)) 800/1000 (and (road_isfree road1) (not (at ?x waypoint2)) (at ?x waypoint0) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l2-l0 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint2 waypoint0) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint0) (road_isfree road1) (available_energy ?x ?olde) (diff ?olde e14 ?newe)) :effect (and (increase (total-cost) 14) (not (at ?x waypoint2)) (at ?x waypoint0) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l0-l3 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint0 waypoint3) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint3) (road_isunknown road2) (available_energy ?x ?olde) (diff ?olde e6 ?newe)) :effect (and (increase (total-cost) 6) (not (road_isunknown road2)) (probabilistic 200/1000 (and (road_isblocked road2)) 800/1000 (and (road_isfree road2) (not (at ?x waypoint0)) (at ?x waypoint3) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l0-l3 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint0 waypoint3) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint3) (road_isfree road2) (available_energy ?x ?olde) (diff ?olde e6 ?newe)) :effect (and (increase (total-cost) 6) (not (at ?x waypoint0)) (at ?x waypoint3) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l3-l0 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint3 waypoint0) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint0) (road_isunknown road2) (available_energy ?x ?olde) (diff ?olde e6 ?newe)) :effect (and (increase (total-cost) 6) (not (road_isunknown road2)) (probabilistic 200/1000 (and (road_isblocked road2)) 800/1000 (and (road_isfree road2) (not (at ?x waypoint3)) (at ?x waypoint0) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l3-l0 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint3 waypoint0) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint0) (road_isfree road2) (available_energy ?x ?olde) (diff ?olde e6 ?newe)) :effect (and (increase (total-cost) 6) (not (at ?x waypoint3)) (at ?x waypoint0) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l0-l4 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint0 waypoint4) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint4) (road_isunknown road3) (available_energy ?x ?olde) (diff ?olde e24 ?newe)) :effect (and (increase (total-cost) 24) (not (road_isunknown road3)) (probabilistic 200/1000 (and (road_isblocked road3)) 800/1000 (and (road_isfree road3) (not (at ?x waypoint0)) (at ?x waypoint4) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l0-l4 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint0 waypoint4) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint4) (road_isfree road3) (available_energy ?x ?olde) (diff ?olde e24 ?newe)) :effect (and (increase (total-cost) 24) (not (at ?x waypoint0)) (at ?x waypoint4) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l4-l0 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint4 waypoint0) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint0) (road_isunknown road3) (available_energy ?x ?olde) (diff ?olde e24 ?newe)) :effect (and (increase (total-cost) 24) (not (road_isunknown road3)) (probabilistic 200/1000 (and (road_isblocked road3)) 800/1000 (and (road_isfree road3) (not (at ?x waypoint4)) (at ?x waypoint0) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l4-l0 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint4 waypoint0) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint0) (road_isfree road3) (available_energy ?x ?olde) (diff ?olde e24 ?newe)) :effect (and (increase (total-cost) 24) (not (at ?x waypoint4)) (at ?x waypoint0) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l0-l5 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint0 waypoint5) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint5) (road_isunknown road4) (available_energy ?x ?olde) (diff ?olde e10 ?newe)) :effect (and (increase (total-cost) 10) (not (road_isunknown road4)) (probabilistic 200/1000 (and (road_isblocked road4)) 800/1000 (and (road_isfree road4) (not (at ?x waypoint0)) (at ?x waypoint5) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l0-l5 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint0 waypoint5) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint5) (road_isfree road4) (available_energy ?x ?olde) (diff ?olde e10 ?newe)) :effect (and (increase (total-cost) 10) (not (at ?x waypoint0)) (at ?x waypoint5) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l5-l0 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint5 waypoint0) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint0) (road_isunknown road4) (available_energy ?x ?olde) (diff ?olde e10 ?newe)) :effect (and (increase (total-cost) 10) (not (road_isunknown road4)) (probabilistic 200/1000 (and (road_isblocked road4)) 800/1000 (and (road_isfree road4) (not (at ?x waypoint5)) (at ?x waypoint0) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l5-l0 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint5 waypoint0) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint0) (road_isfree road4) (available_energy ?x ?olde) (diff ?olde e10 ?newe)) :effect (and (increase (total-cost) 10) (not (at ?x waypoint5)) (at ?x waypoint0) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l1-l4 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint1 waypoint4) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint4) (road_isunknown road5) (available_energy ?x ?olde) (diff ?olde e7 ?newe)) :effect (and (increase (total-cost) 7) (not (road_isunknown road5)) (probabilistic 200/1000 (and (road_isblocked road5)) 800/1000 (and (road_isfree road5) (not (at ?x waypoint1)) (at ?x waypoint4) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l1-l4 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint1 waypoint4) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint4) (road_isfree road5) (available_energy ?x ?olde) (diff ?olde e7 ?newe)) :effect (and (increase (total-cost) 7) (not (at ?x waypoint1)) (at ?x waypoint4) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l4-l1 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint4 waypoint1) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint1) (road_isunknown road5) (available_energy ?x ?olde) (diff ?olde e7 ?newe)) :effect (and (increase (total-cost) 7) (not (road_isunknown road5)) (probabilistic 200/1000 (and (road_isblocked road5)) 800/1000 (and (road_isfree road5) (not (at ?x waypoint4)) (at ?x waypoint1) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l4-l1 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint4 waypoint1) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint1) (road_isfree road5) (available_energy ?x ?olde) (diff ?olde e7 ?newe)) :effect (and (increase (total-cost) 7) (not (at ?x waypoint4)) (at ?x waypoint1) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l1-l5 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint1 waypoint5) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint5) (road_isunknown road6) (available_energy ?x ?olde) (diff ?olde e19 ?newe)) :effect (and (increase (total-cost) 19) (not (road_isunknown road6)) (probabilistic 200/1000 (and (road_isblocked road6)) 800/1000 (and (road_isfree road6) (not (at ?x waypoint1)) (at ?x waypoint5) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l1-l5 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint1 waypoint5) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint5) (road_isfree road6) (available_energy ?x ?olde) (diff ?olde e19 ?newe)) :effect (and (increase (total-cost) 19) (not (at ?x waypoint1)) (at ?x waypoint5) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l5-l1 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint5 waypoint1) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint1) (road_isunknown road6) (available_energy ?x ?olde) (diff ?olde e19 ?newe)) :effect (and (increase (total-cost) 19) (not (road_isunknown road6)) (probabilistic 200/1000 (and (road_isblocked road6)) 800/1000 (and (road_isfree road6) (not (at ?x waypoint5)) (at ?x waypoint1) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l5-l1 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint5 waypoint1) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint1) (road_isfree road6) (available_energy ?x ?olde) (diff ?olde e19 ?newe)) :effect (and (increase (total-cost) 19) (not (at ?x waypoint5)) (at ?x waypoint1) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l2-l3 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint2 waypoint3) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint3) (road_isunknown road7) (available_energy ?x ?olde) (diff ?olde e10 ?newe)) :effect (and (increase (total-cost) 10) (not (road_isunknown road7)) (probabilistic 200/1000 (and (road_isblocked road7)) 800/1000 (and (road_isfree road7) (not (at ?x waypoint2)) (at ?x waypoint3) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l2-l3 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint2 waypoint3) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint3) (road_isfree road7) (available_energy ?x ?olde) (diff ?olde e10 ?newe)) :effect (and (increase (total-cost) 10) (not (at ?x waypoint2)) (at ?x waypoint3) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l3-l2 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint3 waypoint2) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint2) (road_isunknown road7) (available_energy ?x ?olde) (diff ?olde e10 ?newe)) :effect (and (increase (total-cost) 10) (not (road_isunknown road7)) (probabilistic 200/1000 (and (road_isblocked road7)) 800/1000 (and (road_isfree road7) (not (at ?x waypoint3)) (at ?x waypoint2) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l3-l2 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint3 waypoint2) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint2) (road_isfree road7) (available_energy ?x ?olde) (diff ?olde e10 ?newe)) :effect (and (increase (total-cost) 10) (not (at ?x waypoint3)) (at ?x waypoint2) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l2-l4 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint2 waypoint4) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint4) (road_isunknown road8) (available_energy ?x ?olde) (diff ?olde e22 ?newe)) :effect (and (increase (total-cost) 22) (not (road_isunknown road8)) (probabilistic 200/1000 (and (road_isblocked road8)) 800/1000 (and (road_isfree road8) (not (at ?x waypoint2)) (at ?x waypoint4) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l2-l4 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint2 waypoint4) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint4) (road_isfree road8) (available_energy ?x ?olde) (diff ?olde e22 ?newe)) :effect (and (increase (total-cost) 22) (not (at ?x waypoint2)) (at ?x waypoint4) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l4-l2 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint4 waypoint2) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint2) (road_isunknown road8) (available_energy ?x ?olde) (diff ?olde e22 ?newe)) :effect (and (increase (total-cost) 22) (not (road_isunknown road8)) (probabilistic 200/1000 (and (road_isblocked road8)) 800/1000 (and (road_isfree road8) (not (at ?x waypoint4)) (at ?x waypoint2) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l4-l2 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint4 waypoint2) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint2) (road_isfree road8) (available_energy ?x ?olde) (diff ?olde e22 ?newe)) :effect (and (increase (total-cost) 22) (not (at ?x waypoint4)) (at ?x waypoint2) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l3-l4 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint3 waypoint4) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint4) (road_isunknown road9) (available_energy ?x ?olde) (diff ?olde e16 ?newe)) :effect (and (increase (total-cost) 16) (not (road_isunknown road9)) (probabilistic 200/1000 (and (road_isblocked road9)) 800/1000 (and (road_isfree road9) (not (at ?x waypoint3)) (at ?x waypoint4) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l3-l4 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint3 waypoint4) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint4) (road_isfree road9) (available_energy ?x ?olde) (diff ?olde e16 ?newe)) :effect (and (increase (total-cost) 16) (not (at ?x waypoint3)) (at ?x waypoint4) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l4-l3 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint4 waypoint3) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint3) (road_isunknown road9) (available_energy ?x ?olde) (diff ?olde e16 ?newe)) :effect (and (increase (total-cost) 16) (not (road_isunknown road9)) (probabilistic 200/1000 (and (road_isblocked road9)) 800/1000 (and (road_isfree road9) (not (at ?x waypoint4)) (at ?x waypoint3) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l4-l3 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint4 waypoint3) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint3) (road_isfree road9) (available_energy ?x ?olde) (diff ?olde e16 ?newe)) :effect (and (increase (total-cost) 16) (not (at ?x waypoint4)) (at ?x waypoint3) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l3-l5 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint3 waypoint5) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint5) (road_isunknown road10) (available_energy ?x ?olde) (diff ?olde e14 ?newe)) :effect (and (increase (total-cost) 14) (not (road_isunknown road10)) (probabilistic 200/1000 (and (road_isblocked road10)) 800/1000 (and (road_isfree road10) (not (at ?x waypoint3)) (at ?x waypoint5) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l3-l5 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint3 waypoint5) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint5) (road_isfree road10) (available_energy ?x ?olde) (diff ?olde e14 ?newe)) :effect (and (increase (total-cost) 14) (not (at ?x waypoint3)) (at ?x waypoint5) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l5-l3 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint5 waypoint3) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint3) (road_isunknown road10) (available_energy ?x ?olde) (diff ?olde e14 ?newe)) :effect (and (increase (total-cost) 14) (not (road_isunknown road10)) (probabilistic 200/1000 (and (road_isblocked road10)) 800/1000 (and (road_isfree road10) (not (at ?x waypoint5)) (at ?x waypoint3) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l5-l3 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint5 waypoint3) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint3) (road_isfree road10) (available_energy ?x ?olde) (diff ?olde e14 ?newe)) :effect (and (increase (total-cost) 14) (not (at ?x waypoint5)) (at ?x waypoint3) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l4-l5 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint4 waypoint5) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint5) (road_isunknown road11) (available_energy ?x ?olde) (diff ?olde e24 ?newe)) :effect (and (increase (total-cost) 24) (not (road_isunknown road11)) (probabilistic 200/1000 (and (road_isblocked road11)) 800/1000 (and (road_isfree road11) (not (at ?x waypoint4)) (at ?x waypoint5) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l4-l5 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint4 waypoint5) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint5) (road_isfree road11) (available_energy ?x ?olde) (diff ?olde e24 ?newe)) :effect (and (increase (total-cost) 24) (not (at ?x waypoint4)) (at ?x waypoint5) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))) (:action try-navigate-l5-l4 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint5 waypoint4) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint4) (road_isunknown road11) (available_energy ?x ?olde) (diff ?olde e24 ?newe)) :effect (and (increase (total-cost) 24) (not (road_isunknown road11)) (probabilistic 200/1000 (and (road_isblocked road11)) 800/1000 (and (road_isfree road11) (not (at ?x waypoint5)) (at ?x waypoint4) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))) (:action navigate-l5-l4 :parameters (?x - rover ?olde ?newe - energy) :precondition (and (can_traverse ?x waypoint5 waypoint4) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint4) (road_isfree road11) (available_energy ?x ?olde) (diff ?olde e24 ?newe)) :effect (and (increase (total-cost) 24) (not (at ?x waypoint5)) (at ?x waypoint4) (not (available_energy ?x ?olde)) (available_energy ?x ?newe))))