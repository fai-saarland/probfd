(define (problem roverprob--s323227--m25--r2--w3--o2--c2--g4--p0--P1--Z200--X200--C10) (:domain roverdom--s323227--m25--r2--w3--o2--c2--g4--p0--P1--Z200--X200--C10) (:objects general - Lander colour high_res low_res - Mode rover0 rover1 - Rover rover0store rover1store - Store camera0 camera1 camera2 camera3 - Camera objective0 objective1 - Objective) (:init (road_isunknown road0) (road_isunknown road1) (road_isunknown road2) (visible waypoint0 waypoint1) (visible waypoint1 waypoint0) (visible waypoint0 waypoint2) (visible waypoint2 waypoint0) (visible waypoint2 waypoint1) (visible waypoint1 waypoint2) (at_soil_sample waypoint0) (at_rock_sample waypoint0) (at_soil_sample waypoint1) (at_rock_sample waypoint1) (at_soil_sample waypoint2) (at_lander general waypoint1) (channel_free general) (at rover0 waypoint0) (available rover0) (store_of rover0store rover0) (empty rover0store) (equipped_for_soil_analysis rover0) (equipped_for_rock_analysis rover0) (equipped_for_imaging rover0) (can_traverse rover0 waypoint0 waypoint1) (can_traverse rover0 waypoint1 waypoint0) (can_traverse rover0 waypoint0 waypoint2) (can_traverse rover0 waypoint2 waypoint0) (can_traverse rover0 waypoint2 waypoint1) (can_traverse rover0 waypoint1 waypoint2) (on_board camera0 rover0) (calibration_target camera0 objective0) (calibration_target camera0 objective1) (supports camera0 colour) (on_board camera1 rover0) (calibration_target camera1 objective0) (calibration_target camera1 objective1) (supports camera1 low_res) (on_board camera2 rover1) (calibration_target camera2 objective0) (calibration_target camera2 objective1) (supports camera2 colour) (on_board camera3 rover1) (calibration_target camera3 objective0) (calibration_target camera3 objective1) (supports camera3 low_res) (visible_from objective0 waypoint1) (visible_from objective0 waypoint0) (visible_from objective1 waypoint2) (diff e0 e0 e0) (diff e1 e0 e1) (diff e1 e1 e0) (diff e2 e0 e2) (diff e2 e1 e1) (diff e2 e2 e0) (diff e3 e0 e3) (diff e3 e1 e2) (diff e3 e2 e1) (diff e3 e3 e0) (diff e4 e0 e4) (diff e4 e1 e3) (diff e4 e2 e2) (diff e4 e3 e1) (diff e4 e4 e0) (diff e5 e0 e5) (diff e5 e1 e4) (diff e5 e2 e3) (diff e5 e3 e2) (diff e5 e4 e1) (diff e5 e5 e0) (diff e6 e0 e6) (diff e6 e1 e5) (diff e6 e2 e4) (diff e6 e3 e3) (diff e6 e4 e2) (diff e6 e5 e1) (diff e6 e6 e0) (diff e7 e0 e7) (diff e7 e1 e6) (diff e7 e2 e5) (diff e7 e3 e4) (diff e7 e4 e3) (diff e7 e5 e2) (diff e7 e6 e1) (diff e7 e7 e0) (diff e8 e0 e8) (diff e8 e1 e7) (diff e8 e2 e6) (diff e8 e3 e5) (diff e8 e4 e4) (diff e8 e5 e3) (diff e8 e6 e2) (diff e8 e7 e1) (diff e8 e8 e0) (diff e9 e0 e9) (diff e9 e1 e8) (diff e9 e2 e7) (diff e9 e3 e6) (diff e9 e4 e5) (diff e9 e5 e4) (diff e9 e6 e3) (diff e9 e7 e2) (diff e9 e8 e1) (diff e9 e9 e0) (diff e10 e0 e10) (diff e10 e1 e9) (diff e10 e2 e8) (diff e10 e3 e7) (diff e10 e4 e6) (diff e10 e5 e5) (diff e10 e6 e4) (diff e10 e7 e3) (diff e10 e8 e2) (diff e10 e9 e1) (diff e10 e10 e0) (diff e11 e0 e11) (diff e11 e1 e10) (diff e11 e2 e9) (diff e11 e3 e8) (diff e11 e4 e7) (diff e11 e5 e6) (diff e11 e6 e5) (diff e11 e7 e4) (diff e11 e8 e3) (diff e11 e9 e2) (diff e11 e10 e1) (diff e11 e11 e0) (diff e12 e0 e12) (diff e12 e1 e11) (diff e12 e2 e10) (diff e12 e3 e9) (diff e12 e4 e8) (diff e12 e5 e7) (diff e12 e6 e6) (diff e12 e7 e5) (diff e12 e8 e4) (diff e12 e9 e3) (diff e12 e10 e2) (diff e12 e11 e1) (diff e12 e12 e0) (diff e13 e0 e13) (diff e13 e1 e12) (diff e13 e2 e11) (diff e13 e3 e10) (diff e13 e4 e9) (diff e13 e5 e8) (diff e13 e6 e7) (diff e13 e7 e6) (diff e13 e8 e5) (diff e13 e9 e4) (diff e13 e10 e3) (diff e13 e11 e2) (diff e13 e12 e1) (diff e13 e13 e0) (diff e14 e0 e14) (diff e14 e1 e13) (diff e14 e2 e12) (diff e14 e3 e11) (diff e14 e4 e10) (diff e14 e5 e9) (diff e14 e6 e8) (diff e14 e7 e7) (diff e14 e8 e6) (diff e14 e9 e5) (diff e14 e10 e4) (diff e14 e11 e3) (diff e14 e12 e2) (diff e14 e13 e1) (diff e14 e14 e0) (diff e15 e0 e15) (diff e15 e1 e14) (diff e15 e2 e13) (diff e15 e3 e12) (diff e15 e4 e11) (diff e15 e5 e10) (diff e15 e6 e9) (diff e15 e7 e8) (diff e15 e8 e7) (diff e15 e9 e6) (diff e15 e10 e5) (diff e15 e11 e4) (diff e15 e12 e3) (diff e15 e13 e2) (diff e15 e14 e1) (diff e15 e15 e0) (diff e16 e0 e16) (diff e16 e1 e15) (diff e16 e2 e14) (diff e16 e3 e13) (diff e16 e4 e12) (diff e16 e5 e11) (diff e16 e6 e10) (diff e16 e7 e9) (diff e16 e8 e8) (diff e16 e9 e7) (diff e16 e10 e6) (diff e16 e11 e5) (diff e16 e12 e4) (diff e16 e13 e3) (diff e16 e14 e2) (diff e16 e15 e1) (diff e16 e16 e0) (diff e17 e0 e17) (diff e17 e1 e16) (diff e17 e2 e15) (diff e17 e3 e14) (diff e17 e4 e13) (diff e17 e5 e12) (diff e17 e6 e11) (diff e17 e7 e10) (diff e17 e8 e9) (diff e17 e9 e8) (diff e17 e10 e7) (diff e17 e11 e6) (diff e17 e12 e5) (diff e17 e13 e4) (diff e17 e14 e3) (diff e17 e15 e2) (diff e17 e16 e1) (diff e17 e17 e0) (diff e18 e0 e18) (diff e18 e1 e17) (diff e18 e2 e16) (diff e18 e3 e15) (diff e18 e4 e14) (diff e18 e5 e13) (diff e18 e6 e12) (diff e18 e7 e11) (diff e18 e8 e10) (diff e18 e9 e9) (diff e18 e10 e8) (diff e18 e11 e7) (diff e18 e12 e6) (diff e18 e13 e5) (diff e18 e14 e4) (diff e18 e15 e3) (diff e18 e16 e2) (diff e18 e17 e1) (diff e18 e18 e0) (diff e19 e0 e19) (diff e19 e1 e18) (diff e19 e2 e17) (diff e19 e3 e16) (diff e19 e4 e15) (diff e19 e5 e14) (diff e19 e6 e13) (diff e19 e7 e12) (diff e19 e8 e11) (diff e19 e9 e10) (diff e19 e10 e9) (diff e19 e11 e8) (diff e19 e12 e7) (diff e19 e13 e6) (diff e19 e14 e5) (diff e19 e15 e4) (diff e19 e16 e3) (diff e19 e17 e2) (diff e19 e18 e1) (diff e19 e19 e0) (diff e20 e0 e20) (diff e20 e1 e19) (diff e20 e2 e18) (diff e20 e3 e17) (diff e20 e4 e16) (diff e20 e5 e15) (diff e20 e6 e14) (diff e20 e7 e13) (diff e20 e8 e12) (diff e20 e9 e11) (diff e20 e10 e10) (diff e20 e11 e9) (diff e20 e12 e8) (diff e20 e13 e7) (diff e20 e14 e6) (diff e20 e15 e5) (diff e20 e16 e4) (diff e20 e17 e3) (diff e20 e18 e2) (diff e20 e19 e1) (diff e20 e20 e0) (diff e21 e0 e21) (diff e21 e1 e20) (diff e21 e2 e19) (diff e21 e3 e18) (diff e21 e4 e17) (diff e21 e5 e16) (diff e21 e6 e15) (diff e21 e7 e14) (diff e21 e8 e13) (diff e21 e9 e12) (diff e21 e10 e11) (diff e21 e11 e10) (diff e21 e12 e9) (diff e21 e13 e8) (diff e21 e14 e7) (diff e21 e15 e6) (diff e21 e16 e5) (diff e21 e17 e4) (diff e21 e18 e3) (diff e21 e19 e2) (diff e21 e20 e1) (diff e21 e21 e0) (diff e22 e0 e22) (diff e22 e1 e21) (diff e22 e2 e20) (diff e22 e3 e19) (diff e22 e4 e18) (diff e22 e5 e17) (diff e22 e6 e16) (diff e22 e7 e15) (diff e22 e8 e14) (diff e22 e9 e13) (diff e22 e10 e12) (diff e22 e11 e11) (diff e22 e12 e10) (diff e22 e13 e9) (diff e22 e14 e8) (diff e22 e15 e7) (diff e22 e16 e6) (diff e22 e17 e5) (diff e22 e18 e4) (diff e22 e19 e3) (diff e22 e20 e2) (diff e22 e21 e1) (diff e22 e22 e0) (diff e23 e0 e23) (diff e23 e1 e22) (diff e23 e2 e21) (diff e23 e3 e20) (diff e23 e4 e19) (diff e23 e5 e18) (diff e23 e6 e17) (diff e23 e7 e16) (diff e23 e8 e15) (diff e23 e9 e14) (diff e23 e10 e13) (diff e23 e11 e12) (diff e23 e12 e11) (diff e23 e13 e10) (diff e23 e14 e9) (diff e23 e15 e8) (diff e23 e16 e7) (diff e23 e17 e6) (diff e23 e18 e5) (diff e23 e19 e4) (diff e23 e20 e3) (diff e23 e21 e2) (diff e23 e22 e1) (diff e23 e23 e0) (diff e24 e0 e24) (diff e24 e1 e23) (diff e24 e2 e22) (diff e24 e3 e21) (diff e24 e4 e20) (diff e24 e5 e19) (diff e24 e6 e18) (diff e24 e7 e17) (diff e24 e8 e16) (diff e24 e9 e15) (diff e24 e10 e14) (diff e24 e11 e13) (diff e24 e12 e12) (diff e24 e13 e11) (diff e24 e14 e10) (diff e24 e15 e9) (diff e24 e16 e8) (diff e24 e17 e7) (diff e24 e18 e6) (diff e24 e19 e5) (diff e24 e20 e4) (diff e24 e21 e3) (diff e24 e22 e2) (diff e24 e23 e1) (diff e24 e24 e0) (diff e25 e0 e25) (diff e25 e1 e24) (diff e25 e2 e23) (diff e25 e3 e22) (diff e25 e4 e21) (diff e25 e5 e20) (diff e25 e6 e19) (diff e25 e7 e18) (diff e25 e8 e17) (diff e25 e9 e16) (diff e25 e10 e15) (diff e25 e11 e14) (diff e25 e12 e13) (diff e25 e13 e12) (diff e25 e14 e11) (diff e25 e15 e10) (diff e25 e16 e9) (diff e25 e17 e8) (diff e25 e18 e7) (diff e25 e19 e6) (diff e25 e20 e5) (diff e25 e21 e4) (diff e25 e22 e3) (diff e25 e23 e2) (diff e25 e24 e1) (diff e25 e25 e0) (diff e26 e0 e26) (diff e26 e1 e25) (diff e26 e2 e24) (diff e26 e3 e23) (diff e26 e4 e22) (diff e26 e5 e21) (diff e26 e6 e20) (diff e26 e7 e19) (diff e26 e8 e18) (diff e26 e9 e17) (diff e26 e10 e16) (diff e26 e11 e15) (diff e26 e12 e14) (diff e26 e13 e13) (diff e26 e14 e12) (diff e26 e15 e11) (diff e26 e16 e10) (diff e26 e17 e9) (diff e26 e18 e8) (diff e26 e19 e7) (diff e26 e20 e6) (diff e26 e21 e5) (diff e26 e22 e4) (diff e26 e23 e3) (diff e26 e24 e2) (diff e26 e25 e1) (diff e26 e26 e0) (diff e27 e0 e27) (diff e27 e1 e26) (diff e27 e2 e25) (diff e27 e3 e24) (diff e27 e4 e23) (diff e27 e5 e22) (diff e27 e6 e21) (diff e27 e7 e20) (diff e27 e8 e19) (diff e27 e9 e18) (diff e27 e10 e17) (diff e27 e11 e16) (diff e27 e12 e15) (diff e27 e13 e14) (diff e27 e14 e13) (diff e27 e15 e12) (diff e27 e16 e11) (diff e27 e17 e10) (diff e27 e18 e9) (diff e27 e19 e8) (diff e27 e20 e7) (diff e27 e21 e6) (diff e27 e22 e5) (diff e27 e23 e4) (diff e27 e24 e3) (diff e27 e25 e2) (diff e27 e26 e1) (diff e27 e27 e0) (diff e28 e0 e28) (diff e28 e1 e27) (diff e28 e2 e26) (diff e28 e3 e25) (diff e28 e4 e24) (diff e28 e5 e23) (diff e28 e6 e22) (diff e28 e7 e21) (diff e28 e8 e20) (diff e28 e9 e19) (diff e28 e10 e18) (diff e28 e11 e17) (diff e28 e12 e16) (diff e28 e13 e15) (diff e28 e14 e14) (diff e28 e15 e13) (diff e28 e16 e12) (diff e28 e17 e11) (diff e28 e18 e10) (diff e28 e19 e9) (diff e28 e20 e8) (diff e28 e21 e7) (diff e28 e22 e6) (diff e28 e23 e5) (diff e28 e24 e4) (diff e28 e25 e3) (diff e28 e26 e2) (diff e28 e27 e1) (diff e28 e28 e0) (diff e29 e0 e29) (diff e29 e1 e28) (diff e29 e2 e27) (diff e29 e3 e26) (diff e29 e4 e25) (diff e29 e5 e24) (diff e29 e6 e23) (diff e29 e7 e22) (diff e29 e8 e21) (diff e29 e9 e20) (diff e29 e10 e19) (diff e29 e11 e18) (diff e29 e12 e17) (diff e29 e13 e16) (diff e29 e14 e15) (diff e29 e15 e14) (diff e29 e16 e13) (diff e29 e17 e12) (diff e29 e18 e11) (diff e29 e19 e10) (diff e29 e20 e9) (diff e29 e21 e8) (diff e29 e22 e7) (diff e29 e23 e6) (diff e29 e24 e5) (diff e29 e25 e4) (diff e29 e26 e3) (diff e29 e27 e2) (diff e29 e28 e1) (diff e29 e29 e0) (diff e30 e0 e30) (diff e30 e1 e29) (diff e30 e2 e28) (diff e30 e3 e27) (diff e30 e4 e26) (diff e30 e5 e25) (diff e30 e6 e24) (diff e30 e7 e23) (diff e30 e8 e22) (diff e30 e9 e21) (diff e30 e10 e20) (diff e30 e11 e19) (diff e30 e12 e18) (diff e30 e13 e17) (diff e30 e14 e16) (diff e30 e15 e15) (diff e30 e16 e14) (diff e30 e17 e13) (diff e30 e18 e12) (diff e30 e19 e11) (diff e30 e20 e10) (diff e30 e21 e9) (diff e30 e22 e8) (diff e30 e23 e7) (diff e30 e24 e6) (diff e30 e25 e5) (diff e30 e26 e4) (diff e30 e27 e3) (diff e30 e28 e2) (diff e30 e29 e1) (diff e30 e30 e0) (diff e31 e0 e31) (diff e31 e1 e30) (diff e31 e2 e29) (diff e31 e3 e28) (diff e31 e4 e27) (diff e31 e5 e26) (diff e31 e6 e25) (diff e31 e7 e24) (diff e31 e8 e23) (diff e31 e9 e22) (diff e31 e10 e21) (diff e31 e11 e20) (diff e31 e12 e19) (diff e31 e13 e18) (diff e31 e14 e17) (diff e31 e15 e16) (diff e31 e16 e15) (diff e31 e17 e14) (diff e31 e18 e13) (diff e31 e19 e12) (diff e31 e20 e11) (diff e31 e21 e10) (diff e31 e22 e9) (diff e31 e23 e8) (diff e31 e24 e7) (diff e31 e25 e6) (diff e31 e26 e5) (diff e31 e27 e4) (diff e31 e28 e3) (diff e31 e29 e2) (diff e31 e30 e1) (diff e31 e31 e0) (diff e32 e0 e32) (diff e32 e1 e31) (diff e32 e2 e30) (diff e32 e3 e29) (diff e32 e4 e28) (diff e32 e5 e27) (diff e32 e6 e26) (diff e32 e7 e25) (diff e32 e8 e24) (diff e32 e9 e23) (diff e32 e10 e22) (diff e32 e11 e21) (diff e32 e12 e20) (diff e32 e13 e19) (diff e32 e14 e18) (diff e32 e15 e17) (diff e32 e16 e16) (diff e32 e17 e15) (diff e32 e18 e14) (diff e32 e19 e13) (diff e32 e20 e12) (diff e32 e21 e11) (diff e32 e22 e10) (diff e32 e23 e9) (diff e32 e24 e8) (diff e32 e25 e7) (diff e32 e26 e6) (diff e32 e27 e5) (diff e32 e28 e4) (diff e32 e29 e3) (diff e32 e30 e2) (diff e32 e31 e1) (diff e32 e32 e0) (available_energy rover0 e32)) (:goal (and (communicated_soil_data waypoint0) (communicated_rock_data waypoint0) (communicated_image_data objective1 colour) (communicated_image_data objective0 colour))) (:metric minimize (total-cost)))