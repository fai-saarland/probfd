(define (problem roverprob--s265340--m25--r2--w3--o2--c2--g2--p0--P1--Z200--X200--C10) (:domain roverdom--s265340--m25--r2--w3--o2--c2--g2--p0--P1--Z200--X200--C10) (:objects general - Lander colour high_res low_res - Mode rover0 rover1 - Rover rover0store rover1store - Store camera0 camera1 camera2 camera3 - Camera objective0 objective1 - Objective) (:init (road_isunknown road0) (road_isunknown road1) (road_isunknown road2) (visible waypoint0 waypoint2) (visible waypoint2 waypoint0) (visible waypoint1 waypoint0) (visible waypoint0 waypoint1) (visible waypoint2 waypoint1) (visible waypoint1 waypoint2) (at_soil_sample waypoint0) (at_rock_sample waypoint0) (at_soil_sample waypoint1) (at_rock_sample waypoint1) (at_lander general waypoint1) (channel_free general) (at rover0 waypoint2) (available rover0) (store_of rover0store rover0) (empty rover0store) (equipped_for_soil_analysis rover0) (equipped_for_rock_analysis rover0) (equipped_for_imaging rover0) (can_traverse rover0 waypoint0 waypoint2) (can_traverse rover0 waypoint2 waypoint0) (can_traverse rover0 waypoint1 waypoint0) (can_traverse rover0 waypoint0 waypoint1) (can_traverse rover0 waypoint2 waypoint1) (can_traverse rover0 waypoint1 waypoint2) (on_board camera0 rover0) (calibration_target camera0 objective0) (calibration_target camera0 objective1) (supports camera0 high_res) (supports camera0 low_res) (on_board camera1 rover0) (calibration_target camera1 objective0) (calibration_target camera1 objective1) (supports camera1 low_res) (on_board camera2 rover1) (calibration_target camera2 objective0) (calibration_target camera2 objective1) (supports camera2 high_res) (supports camera2 low_res) (on_board camera3 rover1) (calibration_target camera3 objective0) (calibration_target camera3 objective1) (supports camera3 low_res) (visible_from objective0 waypoint0) (visible_from objective1 waypoint1) (visible_from objective1 waypoint2) (diff e0 e0 e0) (diff e1 e0 e1) (diff e1 e1 e0) (diff e2 e0 e2) (diff e2 e1 e1) (diff e2 e2 e0) (diff e3 e0 e3) (diff e3 e1 e2) (diff e3 e2 e1) (diff e3 e3 e0) (diff e4 e0 e4) (diff e4 e1 e3) (diff e4 e2 e2) (diff e4 e3 e1) (diff e4 e4 e0) (diff e5 e0 e5) (diff e5 e1 e4) (diff e5 e2 e3) (diff e5 e3 e2) (diff e5 e4 e1) (diff e5 e5 e0) (diff e6 e0 e6) (diff e6 e1 e5) (diff e6 e2 e4) (diff e6 e3 e3) (diff e6 e4 e2) (diff e6 e5 e1) (diff e6 e6 e0) (diff e7 e0 e7) (diff e7 e1 e6) (diff e7 e2 e5) (diff e7 e3 e4) (diff e7 e4 e3) (diff e7 e5 e2) (diff e7 e6 e1) (diff e7 e7 e0) (diff e8 e0 e8) (diff e8 e1 e7) (diff e8 e2 e6) (diff e8 e3 e5) (diff e8 e4 e4) (diff e8 e5 e3) (diff e8 e6 e2) (diff e8 e7 e1) (diff e8 e8 e0) (diff e9 e0 e9) (diff e9 e1 e8) (diff e9 e2 e7) (diff e9 e3 e6) (diff e9 e4 e5) (diff e9 e5 e4) (diff e9 e6 e3) (diff e9 e7 e2) (diff e9 e8 e1) (diff e9 e9 e0) (diff e10 e0 e10) (diff e10 e1 e9) (diff e10 e2 e8) (diff e10 e3 e7) (diff e10 e4 e6) (diff e10 e5 e5) (diff e10 e6 e4) (diff e10 e7 e3) (diff e10 e8 e2) (diff e10 e9 e1) (diff e10 e10 e0) (diff e11 e0 e11) (diff e11 e1 e10) (diff e11 e2 e9) (diff e11 e3 e8) (diff e11 e4 e7) (diff e11 e5 e6) (diff e11 e6 e5) (diff e11 e7 e4) (diff e11 e8 e3) (diff e11 e9 e2) (diff e11 e10 e1) (diff e11 e11 e0) (diff e12 e0 e12) (diff e12 e1 e11) (diff e12 e2 e10) (diff e12 e3 e9) (diff e12 e4 e8) (diff e12 e5 e7) (diff e12 e6 e6) (diff e12 e7 e5) (diff e12 e8 e4) (diff e12 e9 e3) (diff e12 e10 e2) (diff e12 e11 e1) (diff e12 e12 e0) (diff e13 e0 e13) (diff e13 e1 e12) (diff e13 e2 e11) (diff e13 e3 e10) (diff e13 e4 e9) (diff e13 e5 e8) (diff e13 e6 e7) (diff e13 e7 e6) (diff e13 e8 e5) (diff e13 e9 e4) (diff e13 e10 e3) (diff e13 e11 e2) (diff e13 e12 e1) (diff e13 e13 e0) (diff e14 e0 e14) (diff e14 e1 e13) (diff e14 e2 e12) (diff e14 e3 e11) (diff e14 e4 e10) (diff e14 e5 e9) (diff e14 e6 e8) (diff e14 e7 e7) (diff e14 e8 e6) (diff e14 e9 e5) (diff e14 e10 e4) (diff e14 e11 e3) (diff e14 e12 e2) (diff e14 e13 e1) (diff e14 e14 e0) (available_energy rover0 e14)) (:goal (and (communicated_soil_data waypoint0) (communicated_rock_data waypoint0))) (:metric minimize (total-cost)))