(define (domain roverdom--s852565--m25--r2--w6--o3--c1--g4--p0--P1--Z200--X200--C10)
(:requirements :typing)
(:types horizon-value - object rover waypoint store camera mode lander objective road)

(:predicates (horizon ?h - horizon-value) (horizon-decrement ?h0 ?h1 - horizon-value)  (at ?x - rover ?y - waypoint)
              (at_lander ?x - lander ?y - waypoint)
              (can_traverse ?r - rover ?x - waypoint ?y - waypoint)
              (equipped_for_soil_analysis ?r - rover)
              (equipped_for_rock_analysis ?r - rover)
              (equipped_for_imaging ?r - rover)
              (empty ?s - store)
              (have_rock_analysis ?r - rover ?w - waypoint)
              (have_soil_analysis ?r - rover ?w - waypoint)
              (full ?s - store)
              (calibrated ?c - camera ?r - rover)
              (supports ?c - camera ?m - mode)
              (available ?r - rover)
              (visible ?w - waypoint ?p - waypoint)
              (have_image ?r - rover ?o - objective ?m - mode)
              (communicated_soil_data ?w - waypoint)
              (communicated_rock_data ?w - waypoint)
              (communicated_image_data ?o - objective ?m - mode)
              (at_soil_sample ?w - waypoint)
              (at_rock_sample ?w - waypoint)
              (visible_from ?o - objective ?w - waypoint)
              (store_of ?s - store ?r - rover)
              (calibration_target ?i - camera ?o - objective)
              (on_board ?i - camera ?r - rover)
              (channel_free ?l - lander)
              (road_isunknown ?r - road)
              (road_isfree ?r - road)
              (road_isblocked ?r - road)
)

(:functions (total-cost))
(:constants road0 road1 road2 road3 road4 road5 road6 road7 road8 road9 road10 road11 road12 - road
            waypoint0 waypoint1 waypoint2 waypoint3 waypoint4 waypoint5 - waypoint)

(:action sample_soil
    :parameters (?hcur ?hnew - horizon-value ?x - rover ?s - store ?p - waypoint)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at ?x ?p) (at_soil_sample ?p) (equipped_for_soil_analysis ?x) (store_of ?s ?x) (empty ?s))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (empty ?s)) (full ?s) (have_soil_analysis ?x ?p) (not (at_soil_sample ?p)))
)

(:action sample_rock
    :parameters (?hcur ?hnew - horizon-value ?x - rover ?s - store ?p - waypoint)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew)  (at ?x ?p) (at_rock_sample ?p) (equipped_for_rock_analysis ?x) (store_of ?s ?x)(empty ?s))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (empty ?s)) (full ?s) (have_rock_analysis ?x ?p) (not (at_rock_sample ?p)))
)

(:action drop
    :parameters (?hcur ?hnew - horizon-value ?x - rover ?y - store)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (store_of ?y ?x) (full ?y))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (full ?y)) (empty ?y))
)

(:action calibrate
    :parameters (?hcur ?hnew - horizon-value ?r - rover ?i - camera ?t - objective ?w - waypoint)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (equipped_for_imaging ?r) (calibration_target ?i ?t) (at ?r ?w) (visible_from ?t ?w) (on_board ?i ?r))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (calibrated ?i ?r))
)

(:action take_image
    :parameters (?hcur ?hnew - horizon-value ?r - rover ?p - waypoint ?o - objective ?i - camera ?m - mode)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (calibrated ?i ?r) (on_board ?i ?r) (equipped_for_imaging ?r) (supports ?i ?m) (visible_from ?o ?p) (at ?r ?p))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (have_image ?r ?o ?m)(not (calibrated ?i ?r)))
)

(:action communicate_soil_data
    :parameters (?hcur ?hnew - horizon-value ?r - rover ?l - lander ?p - waypoint ?x - waypoint ?y - waypoint)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at ?r ?x)(at_lander ?l ?y)(have_soil_analysis ?r ?p)  (visible ?x ?y)(available ?r)(channel_free ?l))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (communicated_soil_data ?p))
)

(:action communicate_rock_data
    :parameters (?hcur ?hnew - horizon-value ?r - rover ?l - lander ?p - waypoint ?x - waypoint ?y - waypoint)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at ?r ?x)(at_lander ?l ?y)(have_rock_analysis ?r ?p) (visible ?x ?y)(available ?r)(channel_free ?l))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (communicated_rock_data ?p))
)

(:action communicate_image_data
    :parameters (?hcur ?hnew - horizon-value ?r - rover ?l - lander ?o - objective ?m - mode ?x - waypoint ?y - waypoint)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at ?r ?x)(at_lander ?l ?y)(have_image ?r ?o ?m)(visible ?x ?y)(available ?r)(channel_free ?l))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (communicated_image_data ?o ?m))
)

(:action try-navigate-l0-l1
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint0 waypoint1) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint1) (road_isunknown road0))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road0)) (probabilistic 
        200/1000 (and (road_isblocked road0))
        800/1000 (and (road_isfree road0) (not (at ?x waypoint0)) (at ?x waypoint1))
    ))
)

(:action navigate-l0-l1
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint0 waypoint1) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint1) (road_isfree road0))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint0)) (at ?x waypoint1))
)

(:action try-navigate-l1-l0
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint1 waypoint0) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint0) (road_isunknown road0))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road0)) (probabilistic 
        200/1000 (and (road_isblocked road0))
        800/1000 (and (road_isfree road0) (not (at ?x waypoint1)) (at ?x waypoint0))
    ))
)

(:action navigate-l1-l0
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint1 waypoint0) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint0) (road_isfree road0))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint1)) (at ?x waypoint0))
)

(:action try-navigate-l0-l2
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint0 waypoint2) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint2) (road_isunknown road1))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road1)) (probabilistic 
        200/1000 (and (road_isblocked road1))
        800/1000 (and (road_isfree road1) (not (at ?x waypoint0)) (at ?x waypoint2))
    ))
)

(:action navigate-l0-l2
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint0 waypoint2) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint2) (road_isfree road1))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint0)) (at ?x waypoint2))
)

(:action try-navigate-l2-l0
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint2 waypoint0) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint0) (road_isunknown road1))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road1)) (probabilistic 
        200/1000 (and (road_isblocked road1))
        800/1000 (and (road_isfree road1) (not (at ?x waypoint2)) (at ?x waypoint0))
    ))
)

(:action navigate-l2-l0
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint2 waypoint0) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint0) (road_isfree road1))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint2)) (at ?x waypoint0))
)

(:action try-navigate-l0-l3
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint0 waypoint3) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint3) (road_isunknown road2))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road2)) (probabilistic 
        200/1000 (and (road_isblocked road2))
        800/1000 (and (road_isfree road2) (not (at ?x waypoint0)) (at ?x waypoint3))
    ))
)

(:action navigate-l0-l3
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint0 waypoint3) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint3) (road_isfree road2))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint0)) (at ?x waypoint3))
)

(:action try-navigate-l3-l0
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint3 waypoint0) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint0) (road_isunknown road2))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road2)) (probabilistic 
        200/1000 (and (road_isblocked road2))
        800/1000 (and (road_isfree road2) (not (at ?x waypoint3)) (at ?x waypoint0))
    ))
)

(:action navigate-l3-l0
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint3 waypoint0) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint0) (road_isfree road2))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint3)) (at ?x waypoint0))
)

(:action try-navigate-l0-l4
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint0 waypoint4) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint4) (road_isunknown road3))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road3)) (probabilistic 
        200/1000 (and (road_isblocked road3))
        800/1000 (and (road_isfree road3) (not (at ?x waypoint0)) (at ?x waypoint4))
    ))
)

(:action navigate-l0-l4
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint0 waypoint4) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint4) (road_isfree road3))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint0)) (at ?x waypoint4))
)

(:action try-navigate-l4-l0
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint4 waypoint0) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint0) (road_isunknown road3))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road3)) (probabilistic 
        200/1000 (and (road_isblocked road3))
        800/1000 (and (road_isfree road3) (not (at ?x waypoint4)) (at ?x waypoint0))
    ))
)

(:action navigate-l4-l0
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint4 waypoint0) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint0) (road_isfree road3))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint4)) (at ?x waypoint0))
)

(:action try-navigate-l0-l5
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint0 waypoint5) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint5) (road_isunknown road4))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road4)) (probabilistic 
        200/1000 (and (road_isblocked road4))
        800/1000 (and (road_isfree road4) (not (at ?x waypoint0)) (at ?x waypoint5))
    ))
)

(:action navigate-l0-l5
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint0 waypoint5) (available ?x) (at ?x waypoint0) (visible waypoint0 waypoint5) (road_isfree road4))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint0)) (at ?x waypoint5))
)

(:action try-navigate-l5-l0
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint5 waypoint0) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint0) (road_isunknown road4))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road4)) (probabilistic 
        200/1000 (and (road_isblocked road4))
        800/1000 (and (road_isfree road4) (not (at ?x waypoint5)) (at ?x waypoint0))
    ))
)

(:action navigate-l5-l0
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint5 waypoint0) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint0) (road_isfree road4))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint5)) (at ?x waypoint0))
)

(:action try-navigate-l1-l2
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint1 waypoint2) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint2) (road_isunknown road5))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road5)) (probabilistic 
        200/1000 (and (road_isblocked road5))
        800/1000 (and (road_isfree road5) (not (at ?x waypoint1)) (at ?x waypoint2))
    ))
)

(:action navigate-l1-l2
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint1 waypoint2) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint2) (road_isfree road5))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint1)) (at ?x waypoint2))
)

(:action try-navigate-l2-l1
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint2 waypoint1) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint1) (road_isunknown road5))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road5)) (probabilistic 
        200/1000 (and (road_isblocked road5))
        800/1000 (and (road_isfree road5) (not (at ?x waypoint2)) (at ?x waypoint1))
    ))
)

(:action navigate-l2-l1
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint2 waypoint1) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint1) (road_isfree road5))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint2)) (at ?x waypoint1))
)

(:action try-navigate-l1-l3
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint1 waypoint3) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint3) (road_isunknown road6))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road6)) (probabilistic 
        200/1000 (and (road_isblocked road6))
        800/1000 (and (road_isfree road6) (not (at ?x waypoint1)) (at ?x waypoint3))
    ))
)

(:action navigate-l1-l3
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint1 waypoint3) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint3) (road_isfree road6))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint1)) (at ?x waypoint3))
)

(:action try-navigate-l3-l1
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint3 waypoint1) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint1) (road_isunknown road6))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road6)) (probabilistic 
        200/1000 (and (road_isblocked road6))
        800/1000 (and (road_isfree road6) (not (at ?x waypoint3)) (at ?x waypoint1))
    ))
)

(:action navigate-l3-l1
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint3 waypoint1) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint1) (road_isfree road6))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint3)) (at ?x waypoint1))
)

(:action try-navigate-l1-l5
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint1 waypoint5) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint5) (road_isunknown road7))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road7)) (probabilistic 
        200/1000 (and (road_isblocked road7))
        800/1000 (and (road_isfree road7) (not (at ?x waypoint1)) (at ?x waypoint5))
    ))
)

(:action navigate-l1-l5
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint1 waypoint5) (available ?x) (at ?x waypoint1) (visible waypoint1 waypoint5) (road_isfree road7))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint1)) (at ?x waypoint5))
)

(:action try-navigate-l5-l1
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint5 waypoint1) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint1) (road_isunknown road7))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road7)) (probabilistic 
        200/1000 (and (road_isblocked road7))
        800/1000 (and (road_isfree road7) (not (at ?x waypoint5)) (at ?x waypoint1))
    ))
)

(:action navigate-l5-l1
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint5 waypoint1) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint1) (road_isfree road7))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint5)) (at ?x waypoint1))
)

(:action try-navigate-l2-l4
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint2 waypoint4) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint4) (road_isunknown road8))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road8)) (probabilistic 
        200/1000 (and (road_isblocked road8))
        800/1000 (and (road_isfree road8) (not (at ?x waypoint2)) (at ?x waypoint4))
    ))
)

(:action navigate-l2-l4
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint2 waypoint4) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint4) (road_isfree road8))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint2)) (at ?x waypoint4))
)

(:action try-navigate-l4-l2
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint4 waypoint2) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint2) (road_isunknown road8))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road8)) (probabilistic 
        200/1000 (and (road_isblocked road8))
        800/1000 (and (road_isfree road8) (not (at ?x waypoint4)) (at ?x waypoint2))
    ))
)

(:action navigate-l4-l2
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint4 waypoint2) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint2) (road_isfree road8))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint4)) (at ?x waypoint2))
)

(:action try-navigate-l2-l5
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint2 waypoint5) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint5) (road_isunknown road9))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road9)) (probabilistic 
        200/1000 (and (road_isblocked road9))
        800/1000 (and (road_isfree road9) (not (at ?x waypoint2)) (at ?x waypoint5))
    ))
)

(:action navigate-l2-l5
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint2 waypoint5) (available ?x) (at ?x waypoint2) (visible waypoint2 waypoint5) (road_isfree road9))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint2)) (at ?x waypoint5))
)

(:action try-navigate-l5-l2
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint5 waypoint2) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint2) (road_isunknown road9))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road9)) (probabilistic 
        200/1000 (and (road_isblocked road9))
        800/1000 (and (road_isfree road9) (not (at ?x waypoint5)) (at ?x waypoint2))
    ))
)

(:action navigate-l5-l2
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint5 waypoint2) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint2) (road_isfree road9))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint5)) (at ?x waypoint2))
)

(:action try-navigate-l3-l4
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint3 waypoint4) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint4) (road_isunknown road10))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road10)) (probabilistic 
        200/1000 (and (road_isblocked road10))
        800/1000 (and (road_isfree road10) (not (at ?x waypoint3)) (at ?x waypoint4))
    ))
)

(:action navigate-l3-l4
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint3 waypoint4) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint4) (road_isfree road10))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint3)) (at ?x waypoint4))
)

(:action try-navigate-l4-l3
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint4 waypoint3) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint3) (road_isunknown road10))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road10)) (probabilistic 
        200/1000 (and (road_isblocked road10))
        800/1000 (and (road_isfree road10) (not (at ?x waypoint4)) (at ?x waypoint3))
    ))
)

(:action navigate-l4-l3
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint4 waypoint3) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint3) (road_isfree road10))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint4)) (at ?x waypoint3))
)

(:action try-navigate-l3-l5
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint3 waypoint5) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint5) (road_isunknown road11))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road11)) (probabilistic 
        200/1000 (and (road_isblocked road11))
        800/1000 (and (road_isfree road11) (not (at ?x waypoint3)) (at ?x waypoint5))
    ))
)

(:action navigate-l3-l5
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint3 waypoint5) (available ?x) (at ?x waypoint3) (visible waypoint3 waypoint5) (road_isfree road11))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint3)) (at ?x waypoint5))
)

(:action try-navigate-l5-l3
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint5 waypoint3) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint3) (road_isunknown road11))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road11)) (probabilistic 
        200/1000 (and (road_isblocked road11))
        800/1000 (and (road_isfree road11) (not (at ?x waypoint5)) (at ?x waypoint3))
    ))
)

(:action navigate-l5-l3
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint5 waypoint3) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint3) (road_isfree road11))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint5)) (at ?x waypoint3))
)

(:action try-navigate-l4-l5
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint4 waypoint5) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint5) (road_isunknown road12))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road12)) (probabilistic 
        200/1000 (and (road_isblocked road12))
        800/1000 (and (road_isfree road12) (not (at ?x waypoint4)) (at ?x waypoint5))
    ))
)

(:action navigate-l4-l5
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint4 waypoint5) (available ?x) (at ?x waypoint4) (visible waypoint4 waypoint5) (road_isfree road12))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint4)) (at ?x waypoint5))
)

(:action try-navigate-l5-l4
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint5 waypoint4) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint4) (road_isunknown road12))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road_isunknown road12)) (probabilistic 
        200/1000 (and (road_isblocked road12))
        800/1000 (and (road_isfree road12) (not (at ?x waypoint5)) (at ?x waypoint4))
    ))
)

(:action navigate-l5-l4
    :parameters (?hcur ?hnew - horizon-value ?x - rover)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (can_traverse ?x waypoint5 waypoint4) (available ?x) (at ?x waypoint5) (visible waypoint5 waypoint4) (road_isfree road12))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (at ?x waypoint5)) (at ?x waypoint4))
)

)
