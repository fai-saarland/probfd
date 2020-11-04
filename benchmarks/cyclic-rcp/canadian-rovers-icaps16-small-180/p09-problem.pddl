(define (problem roverprob--s178341--m25--r2--w3--o3--c2--g6--p0--P1--Z200--X200--C10) (:domain roverdom--s178341--m25--r2--w3--o3--c2--g6--p0--P1--Z200--X200--C10) (:objects general - Lander colour high_res low_res - Mode rover0 rover1 - Rover rover0store rover1store - Store camera0 camera1 camera2 camera3 - Camera objective0 objective1 objective2 - Objective) (:init (road_isunknown road0) (road_isunknown road1) (road_isunknown road2) (visible waypoint0 waypoint1) (visible waypoint1 waypoint0) (visible waypoint0 waypoint2) (visible waypoint2 waypoint0) (visible waypoint2 waypoint1) (visible waypoint1 waypoint2) (at_soil_sample waypoint0) (at_rock_sample waypoint0) (at_soil_sample waypoint1) (at_rock_sample waypoint1) (at_soil_sample waypoint2) (at_rock_sample waypoint2) (at_lander general waypoint1) (channel_free general) (at rover0 waypoint2) (available rover0) (store_of rover0store rover0) (empty rover0store) (equipped_for_soil_analysis rover0) (equipped_for_rock_analysis rover0) (equipped_for_imaging rover0) (can_traverse rover0 waypoint0 waypoint1) (can_traverse rover0 waypoint1 waypoint0) (can_traverse rover0 waypoint0 waypoint2) (can_traverse rover0 waypoint2 waypoint0) (can_traverse rover0 waypoint2 waypoint1) (can_traverse rover0 waypoint1 waypoint2) (on_board camera0 rover0) (calibration_target camera0 objective0) (calibration_target camera0 objective1) (calibration_target camera0 objective2) (supports camera0 high_res) (on_board camera1 rover0) (calibration_target camera1 objective0) (calibration_target camera1 objective1) (calibration_target camera1 objective2) (supports camera1 colour) (on_board camera2 rover1) (calibration_target camera2 objective0) (calibration_target camera2 objective1) (calibration_target camera2 objective2) (supports camera2 high_res) (on_board camera3 rover1) (calibration_target camera3 objective0) (calibration_target camera3 objective1) (calibration_target camera3 objective2) (supports camera3 colour) (visible_from objective0 waypoint0) (visible_from objective1 waypoint1) (visible_from objective1 waypoint2) (visible_from objective2 waypoint0) (visible_from objective2 waypoint1) (diff e0 e0 e0) (diff e1 e0 e1) (diff e1 e1 e0) (diff e2 e0 e2) (diff e2 e1 e1) (diff e2 e2 e0) (diff e3 e0 e3) (diff e3 e1 e2) (diff e3 e2 e1) (diff e3 e3 e0) (available_energy rover0 e3)) (:goal (and (communicated_soil_data waypoint0) (communicated_soil_data waypoint2) (communicated_rock_data waypoint0) (communicated_image_data objective1 colour) (communicated_image_data objective0 colour) (communicated_image_data objective2 high_res))) (:metric minimize (total-cost)))