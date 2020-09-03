(define (problem roverprob--s265340--m25--r2--w3--o2--c2--g2--p0--P1--Z200--X200--C10) (:domain roverdom--s265340--m25--r2--w3--o2--c2--g2--p0--P1--Z200--X200--C10)
(:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 - horizon-value
	general - Lander
	colour high_res low_res - Mode
	rover0 rover1 - Rover
	rover0store rover1store - Store
	camera0 camera1 camera2 camera3 - Camera
	objective0 objective1 - Objective
	)
(:init (horizon horzn10) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9)
    (road_isunknown road0)
    (road_isunknown road1)
    (road_isunknown road2)
	(visible waypoint0 waypoint2)
	(visible waypoint2 waypoint0)
	(visible waypoint1 waypoint0)
	(visible waypoint0 waypoint1)
	(visible waypoint2 waypoint1)
	(visible waypoint1 waypoint2)
	(at_soil_sample waypoint0)
	(at_rock_sample waypoint0)
	(at_soil_sample waypoint1)
	(at_rock_sample waypoint1)
	(at_lander general waypoint1)
	(channel_free general)
	(at rover0 waypoint2)
	(available rover0)
	(store_of rover0store rover0)
	(empty rover0store)
	(equipped_for_soil_analysis rover0)
	(equipped_for_rock_analysis rover0)
	(equipped_for_imaging rover0)
	(can_traverse rover0 waypoint0 waypoint2)
	(can_traverse rover0 waypoint2 waypoint0)
	(can_traverse rover0 waypoint1 waypoint0)
	(can_traverse rover0 waypoint0 waypoint1)
	(can_traverse rover0 waypoint2 waypoint1)
	(can_traverse rover0 waypoint1 waypoint2)
	(on_board camera0 rover0)
	(calibration_target camera0 objective0 )
	(calibration_target camera0 objective1 )
	(supports camera0 high_res)
	(supports camera0 low_res)
	(on_board camera1 rover0)
	(calibration_target camera1 objective0 )
	(calibration_target camera1 objective1 )
	(supports camera1 low_res)
	(on_board camera2 rover1)
	(calibration_target camera2 objective0 )
	(calibration_target camera2 objective1 )
	(supports camera2 high_res)
	(supports camera2 low_res)
	(on_board camera3 rover1)
	(calibration_target camera3 objective0 )
	(calibration_target camera3 objective1 )
	(supports camera3 low_res)
	(visible_from objective0 waypoint0)
	(visible_from objective1 waypoint1)
	(visible_from objective1 waypoint2)
)

(:goal (and
(communicated_soil_data waypoint0)
(communicated_rock_data waypoint0)
	)
)
(:metric minimize (total-cost))
)
