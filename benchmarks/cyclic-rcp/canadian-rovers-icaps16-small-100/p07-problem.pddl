(define (problem roverprob--s598364--m25--r2--w3--o3--c2--g5--p0--P1--Z200--X200--C10) (:domain roverdom--s598364--m25--r2--w3--o3--c2--g5--p0--P1--Z200--X200--C10) (:objects general - Lander colour high_res low_res - Mode rover0 rover1 - Rover rover0store rover1store - Store camera0 camera1 camera2 camera3 - Camera objective0 objective1 objective2 - Objective) (:init (road_isunknown road0) (road_isunknown road1) (road_isunknown road2) (visible waypoint0 waypoint1) (visible waypoint1 waypoint0) (visible waypoint1 waypoint2) (visible waypoint2 waypoint1) (visible waypoint2 waypoint0) (visible waypoint0 waypoint2) (at_soil_sample waypoint0) (at_rock_sample waypoint0) (at_soil_sample waypoint1) (at_rock_sample waypoint2) (at_lander general waypoint1) (channel_free general) (at rover0 waypoint0) (available rover0) (store_of rover0store rover0) (empty rover0store) (equipped_for_soil_analysis rover0) (equipped_for_rock_analysis rover0) (equipped_for_imaging rover0) (can_traverse rover0 waypoint0 waypoint1) (can_traverse rover0 waypoint1 waypoint0) (can_traverse rover0 waypoint1 waypoint2) (can_traverse rover0 waypoint2 waypoint1) (can_traverse rover0 waypoint2 waypoint0) (can_traverse rover0 waypoint0 waypoint2) (on_board camera0 rover0) (calibration_target camera0 objective0) (calibration_target camera0 objective1) (calibration_target camera0 objective2) (supports camera0 low_res) (on_board camera1 rover0) (calibration_target camera1 objective0) (calibration_target camera1 objective1) (calibration_target camera1 objective2) (supports camera1 colour) (supports camera1 low_res) (on_board camera2 rover1) (calibration_target camera2 objective0) (calibration_target camera2 objective1) (calibration_target camera2 objective2) (supports camera2 low_res) (on_board camera3 rover1) (calibration_target camera3 objective0) (calibration_target camera3 objective1) (calibration_target camera3 objective2) (supports camera3 colour) (supports camera3 low_res) (visible_from objective0 waypoint0) (visible_from objective1 waypoint2) (visible_from objective1 waypoint1) (visible_from objective2 waypoint2) (diff e0 e0 e0) (diff e1 e0 e1) (diff e1 e1 e0) (diff e2 e0 e2) (diff e2 e1 e1) (diff e2 e2 e0) (diff e3 e0 e3) (diff e3 e1 e2) (diff e3 e2 e1) (diff e3 e3 e0) (diff e4 e0 e4) (diff e4 e1 e3) (diff e4 e2 e2) (diff e4 e3 e1) (diff e4 e4 e0) (diff e5 e0 e5) (diff e5 e1 e4) (diff e5 e2 e3) (diff e5 e3 e2) (diff e5 e4 e1) (diff e5 e5 e0) (diff e6 e0 e6) (diff e6 e1 e5) (diff e6 e2 e4) (diff e6 e3 e3) (diff e6 e4 e2) (diff e6 e5 e1) (diff e6 e6 e0) (diff e7 e0 e7) (diff e7 e1 e6) (diff e7 e2 e5) (diff e7 e3 e4) (diff e7 e4 e3) (diff e7 e5 e2) (diff e7 e6 e1) (diff e7 e7 e0) (diff e8 e0 e8) (diff e8 e1 e7) (diff e8 e2 e6) (diff e8 e3 e5) (diff e8 e4 e4) (diff e8 e5 e3) (diff e8 e6 e2) (diff e8 e7 e1) (diff e8 e8 e0) (diff e9 e0 e9) (diff e9 e1 e8) (diff e9 e2 e7) (diff e9 e3 e6) (diff e9 e4 e5) (diff e9 e5 e4) (diff e9 e6 e3) (diff e9 e7 e2) (diff e9 e8 e1) (diff e9 e9 e0) (diff e10 e0 e10) (diff e10 e1 e9) (diff e10 e2 e8) (diff e10 e3 e7) (diff e10 e4 e6) (diff e10 e5 e5) (diff e10 e6 e4) (diff e10 e7 e3) (diff e10 e8 e2) (diff e10 e9 e1) (diff e10 e10 e0) (diff e11 e0 e11) (diff e11 e1 e10) (diff e11 e2 e9) (diff e11 e3 e8) (diff e11 e4 e7) (diff e11 e5 e6) (diff e11 e6 e5) (diff e11 e7 e4) (diff e11 e8 e3) (diff e11 e9 e2) (diff e11 e10 e1) (diff e11 e11 e0) (diff e12 e0 e12) (diff e12 e1 e11) (diff e12 e2 e10) (diff e12 e3 e9) (diff e12 e4 e8) (diff e12 e5 e7) (diff e12 e6 e6) (diff e12 e7 e5) (diff e12 e8 e4) (diff e12 e9 e3) (diff e12 e10 e2) (diff e12 e11 e1) (diff e12 e12 e0) (diff e13 e0 e13) (diff e13 e1 e12) (diff e13 e2 e11) (diff e13 e3 e10) (diff e13 e4 e9) (diff e13 e5 e8) (diff e13 e6 e7) (diff e13 e7 e6) (diff e13 e8 e5) (diff e13 e9 e4) (diff e13 e10 e3) (diff e13 e11 e2) (diff e13 e12 e1) (diff e13 e13 e0) (diff e14 e0 e14) (diff e14 e1 e13) (diff e14 e2 e12) (diff e14 e3 e11) (diff e14 e4 e10) (diff e14 e5 e9) (diff e14 e6 e8) (diff e14 e7 e7) (diff e14 e8 e6) (diff e14 e9 e5) (diff e14 e10 e4) (diff e14 e11 e3) (diff e14 e12 e2) (diff e14 e13 e1) (diff e14 e14 e0) (diff e15 e0 e15) (diff e15 e1 e14) (diff e15 e2 e13) (diff e15 e3 e12) (diff e15 e4 e11) (diff e15 e5 e10) (diff e15 e6 e9) (diff e15 e7 e8) (diff e15 e8 e7) (diff e15 e9 e6) (diff e15 e10 e5) (diff e15 e11 e4) (diff e15 e12 e3) (diff e15 e13 e2) (diff e15 e14 e1) (diff e15 e15 e0) (diff e16 e0 e16) (diff e16 e1 e15) (diff e16 e2 e14) (diff e16 e3 e13) (diff e16 e4 e12) (diff e16 e5 e11) (diff e16 e6 e10) (diff e16 e7 e9) (diff e16 e8 e8) (diff e16 e9 e7) (diff e16 e10 e6) (diff e16 e11 e5) (diff e16 e12 e4) (diff e16 e13 e3) (diff e16 e14 e2) (diff e16 e15 e1) (diff e16 e16 e0) (diff e17 e0 e17) (diff e17 e1 e16) (diff e17 e2 e15) (diff e17 e3 e14) (diff e17 e4 e13) (diff e17 e5 e12) (diff e17 e6 e11) (diff e17 e7 e10) (diff e17 e8 e9) (diff e17 e9 e8) (diff e17 e10 e7) (diff e17 e11 e6) (diff e17 e12 e5) (diff e17 e13 e4) (diff e17 e14 e3) (diff e17 e15 e2) (diff e17 e16 e1) (diff e17 e17 e0) (diff e18 e0 e18) (diff e18 e1 e17) (diff e18 e2 e16) (diff e18 e3 e15) (diff e18 e4 e14) (diff e18 e5 e13) (diff e18 e6 e12) (diff e18 e7 e11) (diff e18 e8 e10) (diff e18 e9 e9) (diff e18 e10 e8) (diff e18 e11 e7) (diff e18 e12 e6) (diff e18 e13 e5) (diff e18 e14 e4) (diff e18 e15 e3) (diff e18 e16 e2) (diff e18 e17 e1) (diff e18 e18 e0) (diff e19 e0 e19) (diff e19 e1 e18) (diff e19 e2 e17) (diff e19 e3 e16) (diff e19 e4 e15) (diff e19 e5 e14) (diff e19 e6 e13) (diff e19 e7 e12) (diff e19 e8 e11) (diff e19 e9 e10) (diff e19 e10 e9) (diff e19 e11 e8) (diff e19 e12 e7) (diff e19 e13 e6) (diff e19 e14 e5) (diff e19 e15 e4) (diff e19 e16 e3) (diff e19 e17 e2) (diff e19 e18 e1) (diff e19 e19 e0) (available_energy rover0 e19)) (:goal (and (communicated_image_data objective0 low_res) (communicated_image_data objective2 low_res) (communicated_image_data objective1 low_res) (communicated_image_data objective1 colour) (communicated_image_data objective2 colour))) (:metric minimize (total-cost)))