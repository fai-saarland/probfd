(define (problem roverprob--s488595--m25--r2--w6--o3--c1--g4--p0--P1--Z200--X200--C10) (:domain roverdom--s488595--m25--r2--w6--o3--c1--g4--p0--P1--Z200--X200--C10)
(:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 horzn21 horzn22 horzn23 horzn24 horzn25 horzn26 horzn27 horzn28 - horizon-value
	general - Lander
	colour high_res low_res - Mode
	rover0 rover1 - Rover
	rover0store rover1store - Store
	camera0 camera1 - Camera
	objective0 objective1 objective2 - Objective
	)
(:init (horizon horzn28) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (horizon-decrement horzn15 horzn14) (horizon-decrement horzn16 horzn15) (horizon-decrement horzn17 horzn16) (horizon-decrement horzn18 horzn17) (horizon-decrement horzn19 horzn18) (horizon-decrement horzn20 horzn19) (horizon-decrement horzn21 horzn20) (horizon-decrement horzn22 horzn21) (horizon-decrement horzn23 horzn22) (horizon-decrement horzn24 horzn23) (horizon-decrement horzn25 horzn24) (horizon-decrement horzn26 horzn25) (horizon-decrement horzn27 horzn26) (horizon-decrement horzn28 horzn27)
    (road_isunknown road0)
    (road_isunknown road1)
    (road_isunknown road2)
    (road_isunknown road3)
    (road_isunknown road4)
    (road_isunknown road5)
    (road_isunknown road6)
    (road_isunknown road7)
    (road_isunknown road8)
    (road_isunknown road9)
    (road_isunknown road10)
	(visible waypoint1 waypoint2)
	(visible waypoint2 waypoint1)
	(visible waypoint1 waypoint3)
	(visible waypoint3 waypoint1)
	(visible waypoint1 waypoint4)
	(visible waypoint4 waypoint1)
	(visible waypoint2 waypoint0)
	(visible waypoint0 waypoint2)
	(visible waypoint2 waypoint3)
	(visible waypoint3 waypoint2)
	(visible waypoint2 waypoint4)
	(visible waypoint4 waypoint2)
	(visible waypoint2 waypoint5)
	(visible waypoint5 waypoint2)
	(visible waypoint3 waypoint0)
	(visible waypoint0 waypoint3)
	(visible waypoint3 waypoint4)
	(visible waypoint4 waypoint3)
	(visible waypoint4 waypoint5)
	(visible waypoint5 waypoint4)
	(visible waypoint5 waypoint3)
	(visible waypoint3 waypoint5)
	(at_soil_sample waypoint1)
	(at_rock_sample waypoint2)
	(at_rock_sample waypoint3)
	(at_rock_sample waypoint4)
	(at_soil_sample waypoint5)
	(at_rock_sample waypoint5)
	(at_lander general waypoint4)
	(channel_free general)
	(at rover0 waypoint1)
	(available rover0)
	(store_of rover0store rover0)
	(empty rover0store)
	(equipped_for_soil_analysis rover0)
	(equipped_for_rock_analysis rover0)
	(equipped_for_imaging rover0)
	(can_traverse rover0 waypoint1 waypoint2)
	(can_traverse rover0 waypoint2 waypoint1)
	(can_traverse rover0 waypoint1 waypoint3)
	(can_traverse rover0 waypoint3 waypoint1)
	(can_traverse rover0 waypoint1 waypoint4)
	(can_traverse rover0 waypoint4 waypoint1)
	(can_traverse rover0 waypoint2 waypoint0)
	(can_traverse rover0 waypoint0 waypoint2)
	(can_traverse rover0 waypoint2 waypoint3)
	(can_traverse rover0 waypoint3 waypoint2)
	(can_traverse rover0 waypoint2 waypoint4)
	(can_traverse rover0 waypoint4 waypoint2)
	(can_traverse rover0 waypoint2 waypoint5)
	(can_traverse rover0 waypoint5 waypoint2)
	(can_traverse rover0 waypoint3 waypoint0)
	(can_traverse rover0 waypoint0 waypoint3)
	(can_traverse rover0 waypoint3 waypoint4)
	(can_traverse rover0 waypoint4 waypoint3)
	(can_traverse rover0 waypoint4 waypoint5)
	(can_traverse rover0 waypoint5 waypoint4)
	(can_traverse rover0 waypoint5 waypoint3)
	(can_traverse rover0 waypoint3 waypoint5)
	(on_board camera0 rover0)
	(calibration_target camera0 objective0 )
	(calibration_target camera0 objective1 )
	(calibration_target camera0 objective2 )
	(supports camera0 high_res)
	(on_board camera1 rover1)
	(calibration_target camera1 objective0 )
	(calibration_target camera1 objective1 )
	(calibration_target camera1 objective2 )
	(supports camera1 high_res)
	(visible_from objective0 waypoint4)
	(visible_from objective0 waypoint3)
	(visible_from objective0 waypoint0)
	(visible_from objective1 waypoint1)
	(visible_from objective1 waypoint3)
	(visible_from objective1 waypoint5)
	(visible_from objective1 waypoint2)
	(visible_from objective2 waypoint3)
	(visible_from objective2 waypoint1)
	(visible_from objective2 waypoint4)
	(visible_from objective2 waypoint5)
)

(:goal (and
(communicated_rock_data waypoint2)
(communicated_rock_data waypoint3)
(communicated_rock_data waypoint5)
(communicated_image_data objective1 high_res)
	)
)
(:metric minimize (total-cost))
)
