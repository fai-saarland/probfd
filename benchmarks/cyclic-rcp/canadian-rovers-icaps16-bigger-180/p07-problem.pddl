(define (problem roverprob--s717101--m25--r2--w6--o3--c1--g4--p0--P1--Z200--X200--C10) (:domain roverdom--s717101--m25--r2--w6--o3--c1--g4--p0--P1--Z200--X200--C10) (:objects general - Lander colour high_res low_res - Mode rover0 rover1 - Rover rover0store rover1store - Store camera0 camera1 - Camera objective0 objective1 objective2 - Objective) (:init (road_isunknown road0) (road_isunknown road1) (road_isunknown road2) (road_isunknown road3) (road_isunknown road4) (road_isunknown road5) (road_isunknown road6) (road_isunknown road7) (road_isunknown road8) (road_isunknown road9) (road_isunknown road10) (road_isunknown road11) (visible waypoint0 waypoint2) (visible waypoint2 waypoint0) (visible waypoint0 waypoint3) (visible waypoint3 waypoint0) (visible waypoint0 waypoint4) (visible waypoint4 waypoint0) (visible waypoint1 waypoint0) (visible waypoint0 waypoint1) (visible waypoint2 waypoint3) (visible waypoint3 waypoint2) (visible waypoint2 waypoint4) (visible waypoint4 waypoint2) (visible waypoint3 waypoint5) (visible waypoint5 waypoint3) (visible waypoint4 waypoint1) (visible waypoint1 waypoint4) (visible waypoint4 waypoint3) (visible waypoint3 waypoint4) (visible waypoint5 waypoint0) (visible waypoint0 waypoint5) (visible waypoint5 waypoint1) (visible waypoint1 waypoint5) (visible waypoint5 waypoint4) (visible waypoint4 waypoint5) (at_soil_sample waypoint2) (at_rock_sample waypoint2) (at_soil_sample waypoint4) (at_rock_sample waypoint4) (at_lander general waypoint5) (channel_free general) (at rover0 waypoint3) (available rover0) (store_of rover0store rover0) (empty rover0store) (equipped_for_soil_analysis rover0) (equipped_for_rock_analysis rover0) (equipped_for_imaging rover0) (can_traverse rover0 waypoint0 waypoint2) (can_traverse rover0 waypoint2 waypoint0) (can_traverse rover0 waypoint0 waypoint3) (can_traverse rover0 waypoint3 waypoint0) (can_traverse rover0 waypoint0 waypoint4) (can_traverse rover0 waypoint4 waypoint0) (can_traverse rover0 waypoint1 waypoint0) (can_traverse rover0 waypoint0 waypoint1) (can_traverse rover0 waypoint2 waypoint3) (can_traverse rover0 waypoint3 waypoint2) (can_traverse rover0 waypoint2 waypoint4) (can_traverse rover0 waypoint4 waypoint2) (can_traverse rover0 waypoint3 waypoint5) (can_traverse rover0 waypoint5 waypoint3) (can_traverse rover0 waypoint4 waypoint1) (can_traverse rover0 waypoint1 waypoint4) (can_traverse rover0 waypoint4 waypoint3) (can_traverse rover0 waypoint3 waypoint4) (can_traverse rover0 waypoint5 waypoint0) (can_traverse rover0 waypoint0 waypoint5) (can_traverse rover0 waypoint5 waypoint1) (can_traverse rover0 waypoint1 waypoint5) (can_traverse rover0 waypoint5 waypoint4) (can_traverse rover0 waypoint4 waypoint5) (on_board camera0 rover0) (calibration_target camera0 objective0) (calibration_target camera0 objective1) (calibration_target camera0 objective2) (supports camera0 colour) (supports camera0 high_res) (on_board camera1 rover1) (calibration_target camera1 objective0) (calibration_target camera1 objective1) (calibration_target camera1 objective2) (supports camera1 colour) (supports camera1 high_res) (visible_from objective0 waypoint3) (visible_from objective0 waypoint0) (visible_from objective1 waypoint0) (visible_from objective1 waypoint1) (visible_from objective1 waypoint5) (visible_from objective1 waypoint2) (visible_from objective2 waypoint1) (visible_from objective2 waypoint0) (visible_from objective2 waypoint5) (diff e0 e0 e0) (diff e1 e0 e1) (diff e1 e1 e0) (diff e2 e0 e2) (diff e2 e1 e1) (diff e2 e2 e0) (diff e3 e0 e3) (diff e3 e1 e2) (diff e3 e2 e1) (diff e3 e3 e0) (diff e4 e0 e4) (diff e4 e1 e3) (diff e4 e2 e2) (diff e4 e3 e1) (diff e4 e4 e0) (diff e5 e0 e5) (diff e5 e1 e4) (diff e5 e2 e3) (diff e5 e3 e2) (diff e5 e4 e1) (diff e5 e5 e0) (diff e6 e0 e6) (diff e6 e1 e5) (diff e6 e2 e4) (diff e6 e3 e3) (diff e6 e4 e2) (diff e6 e5 e1) (diff e6 e6 e0) (diff e7 e0 e7) (diff e7 e1 e6) (diff e7 e2 e5) (diff e7 e3 e4) (diff e7 e4 e3) (diff e7 e5 e2) (diff e7 e6 e1) (diff e7 e7 e0) (diff e8 e0 e8) (diff e8 e1 e7) (diff e8 e2 e6) (diff e8 e3 e5) (diff e8 e4 e4) (diff e8 e5 e3) (diff e8 e6 e2) (diff e8 e7 e1) (diff e8 e8 e0) (diff e9 e0 e9) (diff e9 e1 e8) (diff e9 e2 e7) (diff e9 e3 e6) (diff e9 e4 e5) (diff e9 e5 e4) (diff e9 e6 e3) (diff e9 e7 e2) (diff e9 e8 e1) (diff e9 e9 e0) (diff e10 e0 e10) (diff e10 e1 e9) (diff e10 e2 e8) (diff e10 e3 e7) (diff e10 e4 e6) (diff e10 e5 e5) (diff e10 e6 e4) (diff e10 e7 e3) (diff e10 e8 e2) (diff e10 e9 e1) (diff e10 e10 e0) (diff e11 e0 e11) (diff e11 e1 e10) (diff e11 e2 e9) (diff e11 e3 e8) (diff e11 e4 e7) (diff e11 e5 e6) (diff e11 e6 e5) (diff e11 e7 e4) (diff e11 e8 e3) (diff e11 e9 e2) (diff e11 e10 e1) (diff e11 e11 e0) (diff e12 e0 e12) (diff e12 e1 e11) (diff e12 e2 e10) (diff e12 e3 e9) (diff e12 e4 e8) (diff e12 e5 e7) (diff e12 e6 e6) (diff e12 e7 e5) (diff e12 e8 e4) (diff e12 e9 e3) (diff e12 e10 e2) (diff e12 e11 e1) (diff e12 e12 e0) (diff e13 e0 e13) (diff e13 e1 e12) (diff e13 e2 e11) (diff e13 e3 e10) (diff e13 e4 e9) (diff e13 e5 e8) (diff e13 e6 e7) (diff e13 e7 e6) (diff e13 e8 e5) (diff e13 e9 e4) (diff e13 e10 e3) (diff e13 e11 e2) (diff e13 e12 e1) (diff e13 e13 e0) (diff e14 e0 e14) (diff e14 e1 e13) (diff e14 e2 e12) (diff e14 e3 e11) (diff e14 e4 e10) (diff e14 e5 e9) (diff e14 e6 e8) (diff e14 e7 e7) (diff e14 e8 e6) (diff e14 e9 e5) (diff e14 e10 e4) (diff e14 e11 e3) (diff e14 e12 e2) (diff e14 e13 e1) (diff e14 e14 e0) (diff e15 e0 e15) (diff e15 e1 e14) (diff e15 e2 e13) (diff e15 e3 e12) (diff e15 e4 e11) (diff e15 e5 e10) (diff e15 e6 e9) (diff e15 e7 e8) (diff e15 e8 e7) (diff e15 e9 e6) (diff e15 e10 e5) (diff e15 e11 e4) (diff e15 e12 e3) (diff e15 e13 e2) (diff e15 e14 e1) (diff e15 e15 e0) (diff e16 e0 e16) (diff e16 e1 e15) (diff e16 e2 e14) (diff e16 e3 e13) (diff e16 e4 e12) (diff e16 e5 e11) (diff e16 e6 e10) (diff e16 e7 e9) (diff e16 e8 e8) (diff e16 e9 e7) (diff e16 e10 e6) (diff e16 e11 e5) (diff e16 e12 e4) (diff e16 e13 e3) (diff e16 e14 e2) (diff e16 e15 e1) (diff e16 e16 e0) (diff e17 e0 e17) (diff e17 e1 e16) (diff e17 e2 e15) (diff e17 e3 e14) (diff e17 e4 e13) (diff e17 e5 e12) (diff e17 e6 e11) (diff e17 e7 e10) (diff e17 e8 e9) (diff e17 e9 e8) (diff e17 e10 e7) (diff e17 e11 e6) (diff e17 e12 e5) (diff e17 e13 e4) (diff e17 e14 e3) (diff e17 e15 e2) (diff e17 e16 e1) (diff e17 e17 e0) (diff e18 e0 e18) (diff e18 e1 e17) (diff e18 e2 e16) (diff e18 e3 e15) (diff e18 e4 e14) (diff e18 e5 e13) (diff e18 e6 e12) (diff e18 e7 e11) (diff e18 e8 e10) (diff e18 e9 e9) (diff e18 e10 e8) (diff e18 e11 e7) (diff e18 e12 e6) (diff e18 e13 e5) (diff e18 e14 e4) (diff e18 e15 e3) (diff e18 e16 e2) (diff e18 e17 e1) (diff e18 e18 e0) (diff e19 e0 e19) (diff e19 e1 e18) (diff e19 e2 e17) (diff e19 e3 e16) (diff e19 e4 e15) (diff e19 e5 e14) (diff e19 e6 e13) (diff e19 e7 e12) (diff e19 e8 e11) (diff e19 e9 e10) (diff e19 e10 e9) (diff e19 e11 e8) (diff e19 e12 e7) (diff e19 e13 e6) (diff e19 e14 e5) (diff e19 e15 e4) (diff e19 e16 e3) (diff e19 e17 e2) (diff e19 e18 e1) (diff e19 e19 e0) (diff e20 e0 e20) (diff e20 e1 e19) (diff e20 e2 e18) (diff e20 e3 e17) (diff e20 e4 e16) (diff e20 e5 e15) (diff e20 e6 e14) (diff e20 e7 e13) (diff e20 e8 e12) (diff e20 e9 e11) (diff e20 e10 e10) (diff e20 e11 e9) (diff e20 e12 e8) (diff e20 e13 e7) (diff e20 e14 e6) (diff e20 e15 e5) (diff e20 e16 e4) (diff e20 e17 e3) (diff e20 e18 e2) (diff e20 e19 e1) (diff e20 e20 e0) (diff e21 e0 e21) (diff e21 e1 e20) (diff e21 e2 e19) (diff e21 e3 e18) (diff e21 e4 e17) (diff e21 e5 e16) (diff e21 e6 e15) (diff e21 e7 e14) (diff e21 e8 e13) (diff e21 e9 e12) (diff e21 e10 e11) (diff e21 e11 e10) (diff e21 e12 e9) (diff e21 e13 e8) (diff e21 e14 e7) (diff e21 e15 e6) (diff e21 e16 e5) (diff e21 e17 e4) (diff e21 e18 e3) (diff e21 e19 e2) (diff e21 e20 e1) (diff e21 e21 e0) (diff e22 e0 e22) (diff e22 e1 e21) (diff e22 e2 e20) (diff e22 e3 e19) (diff e22 e4 e18) (diff e22 e5 e17) (diff e22 e6 e16) (diff e22 e7 e15) (diff e22 e8 e14) (diff e22 e9 e13) (diff e22 e10 e12) (diff e22 e11 e11) (diff e22 e12 e10) (diff e22 e13 e9) (diff e22 e14 e8) (diff e22 e15 e7) (diff e22 e16 e6) (diff e22 e17 e5) (diff e22 e18 e4) (diff e22 e19 e3) (diff e22 e20 e2) (diff e22 e21 e1) (diff e22 e22 e0) (diff e23 e0 e23) (diff e23 e1 e22) (diff e23 e2 e21) (diff e23 e3 e20) (diff e23 e4 e19) (diff e23 e5 e18) (diff e23 e6 e17) (diff e23 e7 e16) (diff e23 e8 e15) (diff e23 e9 e14) (diff e23 e10 e13) (diff e23 e11 e12) (diff e23 e12 e11) (diff e23 e13 e10) (diff e23 e14 e9) (diff e23 e15 e8) (diff e23 e16 e7) (diff e23 e17 e6) (diff e23 e18 e5) (diff e23 e19 e4) (diff e23 e20 e3) (diff e23 e21 e2) (diff e23 e22 e1) (diff e23 e23 e0) (diff e24 e0 e24) (diff e24 e1 e23) (diff e24 e2 e22) (diff e24 e3 e21) (diff e24 e4 e20) (diff e24 e5 e19) (diff e24 e6 e18) (diff e24 e7 e17) (diff e24 e8 e16) (diff e24 e9 e15) (diff e24 e10 e14) (diff e24 e11 e13) (diff e24 e12 e12) (diff e24 e13 e11) (diff e24 e14 e10) (diff e24 e15 e9) (diff e24 e16 e8) (diff e24 e17 e7) (diff e24 e18 e6) (diff e24 e19 e5) (diff e24 e20 e4) (diff e24 e21 e3) (diff e24 e22 e2) (diff e24 e23 e1) (diff e24 e24 e0) (diff e25 e0 e25) (diff e25 e1 e24) (diff e25 e2 e23) (diff e25 e3 e22) (diff e25 e4 e21) (diff e25 e5 e20) (diff e25 e6 e19) (diff e25 e7 e18) (diff e25 e8 e17) (diff e25 e9 e16) (diff e25 e10 e15) (diff e25 e11 e14) (diff e25 e12 e13) (diff e25 e13 e12) (diff e25 e14 e11) (diff e25 e15 e10) (diff e25 e16 e9) (diff e25 e17 e8) (diff e25 e18 e7) (diff e25 e19 e6) (diff e25 e20 e5) (diff e25 e21 e4) (diff e25 e22 e3) (diff e25 e23 e2) (diff e25 e24 e1) (diff e25 e25 e0) (diff e26 e0 e26) (diff e26 e1 e25) (diff e26 e2 e24) (diff e26 e3 e23) (diff e26 e4 e22) (diff e26 e5 e21) (diff e26 e6 e20) (diff e26 e7 e19) (diff e26 e8 e18) (diff e26 e9 e17) (diff e26 e10 e16) (diff e26 e11 e15) (diff e26 e12 e14) (diff e26 e13 e13) (diff e26 e14 e12) (diff e26 e15 e11) (diff e26 e16 e10) (diff e26 e17 e9) (diff e26 e18 e8) (diff e26 e19 e7) (diff e26 e20 e6) (diff e26 e21 e5) (diff e26 e22 e4) (diff e26 e23 e3) (diff e26 e24 e2) (diff e26 e25 e1) (diff e26 e26 e0) (diff e27 e0 e27) (diff e27 e1 e26) (diff e27 e2 e25) (diff e27 e3 e24) (diff e27 e4 e23) (diff e27 e5 e22) (diff e27 e6 e21) (diff e27 e7 e20) (diff e27 e8 e19) (diff e27 e9 e18) (diff e27 e10 e17) (diff e27 e11 e16) (diff e27 e12 e15) (diff e27 e13 e14) (diff e27 e14 e13) (diff e27 e15 e12) (diff e27 e16 e11) (diff e27 e17 e10) (diff e27 e18 e9) (diff e27 e19 e8) (diff e27 e20 e7) (diff e27 e21 e6) (diff e27 e22 e5) (diff e27 e23 e4) (diff e27 e24 e3) (diff e27 e25 e2) (diff e27 e26 e1) (diff e27 e27 e0) (diff e28 e0 e28) (diff e28 e1 e27) (diff e28 e2 e26) (diff e28 e3 e25) (diff e28 e4 e24) (diff e28 e5 e23) (diff e28 e6 e22) (diff e28 e7 e21) (diff e28 e8 e20) (diff e28 e9 e19) (diff e28 e10 e18) (diff e28 e11 e17) (diff e28 e12 e16) (diff e28 e13 e15) (diff e28 e14 e14) (diff e28 e15 e13) (diff e28 e16 e12) (diff e28 e17 e11) (diff e28 e18 e10) (diff e28 e19 e9) (diff e28 e20 e8) (diff e28 e21 e7) (diff e28 e22 e6) (diff e28 e23 e5) (diff e28 e24 e4) (diff e28 e25 e3) (diff e28 e26 e2) (diff e28 e27 e1) (diff e28 e28 e0) (diff e29 e0 e29) (diff e29 e1 e28) (diff e29 e2 e27) (diff e29 e3 e26) (diff e29 e4 e25) (diff e29 e5 e24) (diff e29 e6 e23) (diff e29 e7 e22) (diff e29 e8 e21) (diff e29 e9 e20) (diff e29 e10 e19) (diff e29 e11 e18) (diff e29 e12 e17) (diff e29 e13 e16) (diff e29 e14 e15) (diff e29 e15 e14) (diff e29 e16 e13) (diff e29 e17 e12) (diff e29 e18 e11) (diff e29 e19 e10) (diff e29 e20 e9) (diff e29 e21 e8) (diff e29 e22 e7) (diff e29 e23 e6) (diff e29 e24 e5) (diff e29 e25 e4) (diff e29 e26 e3) (diff e29 e27 e2) (diff e29 e28 e1) (diff e29 e29 e0) (diff e30 e0 e30) (diff e30 e1 e29) (diff e30 e2 e28) (diff e30 e3 e27) (diff e30 e4 e26) (diff e30 e5 e25) (diff e30 e6 e24) (diff e30 e7 e23) (diff e30 e8 e22) (diff e30 e9 e21) (diff e30 e10 e20) (diff e30 e11 e19) (diff e30 e12 e18) (diff e30 e13 e17) (diff e30 e14 e16) (diff e30 e15 e15) (diff e30 e16 e14) (diff e30 e17 e13) (diff e30 e18 e12) (diff e30 e19 e11) (diff e30 e20 e10) (diff e30 e21 e9) (diff e30 e22 e8) (diff e30 e23 e7) (diff e30 e24 e6) (diff e30 e25 e5) (diff e30 e26 e4) (diff e30 e27 e3) (diff e30 e28 e2) (diff e30 e29 e1) (diff e30 e30 e0) (diff e31 e0 e31) (diff e31 e1 e30) (diff e31 e2 e29) (diff e31 e3 e28) (diff e31 e4 e27) (diff e31 e5 e26) (diff e31 e6 e25) (diff e31 e7 e24) (diff e31 e8 e23) (diff e31 e9 e22) (diff e31 e10 e21) (diff e31 e11 e20) (diff e31 e12 e19) (diff e31 e13 e18) (diff e31 e14 e17) (diff e31 e15 e16) (diff e31 e16 e15) (diff e31 e17 e14) (diff e31 e18 e13) (diff e31 e19 e12) (diff e31 e20 e11) (diff e31 e21 e10) (diff e31 e22 e9) (diff e31 e23 e8) (diff e31 e24 e7) (diff e31 e25 e6) (diff e31 e26 e5) (diff e31 e27 e4) (diff e31 e28 e3) (diff e31 e29 e2) (diff e31 e30 e1) (diff e31 e31 e0) (diff e32 e0 e32) (diff e32 e1 e31) (diff e32 e2 e30) (diff e32 e3 e29) (diff e32 e4 e28) (diff e32 e5 e27) (diff e32 e6 e26) (diff e32 e7 e25) (diff e32 e8 e24) (diff e32 e9 e23) (diff e32 e10 e22) (diff e32 e11 e21) (diff e32 e12 e20) (diff e32 e13 e19) (diff e32 e14 e18) (diff e32 e15 e17) (diff e32 e16 e16) (diff e32 e17 e15) (diff e32 e18 e14) (diff e32 e19 e13) (diff e32 e20 e12) (diff e32 e21 e11) (diff e32 e22 e10) (diff e32 e23 e9) (diff e32 e24 e8) (diff e32 e25 e7) (diff e32 e26 e6) (diff e32 e27 e5) (diff e32 e28 e4) (diff e32 e29 e3) (diff e32 e30 e2) (diff e32 e31 e1) (diff e32 e32 e0) (diff e33 e0 e33) (diff e33 e1 e32) (diff e33 e2 e31) (diff e33 e3 e30) (diff e33 e4 e29) (diff e33 e5 e28) (diff e33 e6 e27) (diff e33 e7 e26) (diff e33 e8 e25) (diff e33 e9 e24) (diff e33 e10 e23) (diff e33 e11 e22) (diff e33 e12 e21) (diff e33 e13 e20) (diff e33 e14 e19) (diff e33 e15 e18) (diff e33 e16 e17) (diff e33 e17 e16) (diff e33 e18 e15) (diff e33 e19 e14) (diff e33 e20 e13) (diff e33 e21 e12) (diff e33 e22 e11) (diff e33 e23 e10) (diff e33 e24 e9) (diff e33 e25 e8) (diff e33 e26 e7) (diff e33 e27 e6) (diff e33 e28 e5) (diff e33 e29 e4) (diff e33 e30 e3) (diff e33 e31 e2) (diff e33 e32 e1) (diff e33 e33 e0) (diff e34 e0 e34) (diff e34 e1 e33) (diff e34 e2 e32) (diff e34 e3 e31) (diff e34 e4 e30) (diff e34 e5 e29) (diff e34 e6 e28) (diff e34 e7 e27) (diff e34 e8 e26) (diff e34 e9 e25) (diff e34 e10 e24) (diff e34 e11 e23) (diff e34 e12 e22) (diff e34 e13 e21) (diff e34 e14 e20) (diff e34 e15 e19) (diff e34 e16 e18) (diff e34 e17 e17) (diff e34 e18 e16) (diff e34 e19 e15) (diff e34 e20 e14) (diff e34 e21 e13) (diff e34 e22 e12) (diff e34 e23 e11) (diff e34 e24 e10) (diff e34 e25 e9) (diff e34 e26 e8) (diff e34 e27 e7) (diff e34 e28 e6) (diff e34 e29 e5) (diff e34 e30 e4) (diff e34 e31 e3) (diff e34 e32 e2) (diff e34 e33 e1) (diff e34 e34 e0) (diff e35 e0 e35) (diff e35 e1 e34) (diff e35 e2 e33) (diff e35 e3 e32) (diff e35 e4 e31) (diff e35 e5 e30) (diff e35 e6 e29) (diff e35 e7 e28) (diff e35 e8 e27) (diff e35 e9 e26) (diff e35 e10 e25) (diff e35 e11 e24) (diff e35 e12 e23) (diff e35 e13 e22) (diff e35 e14 e21) (diff e35 e15 e20) (diff e35 e16 e19) (diff e35 e17 e18) (diff e35 e18 e17) (diff e35 e19 e16) (diff e35 e20 e15) (diff e35 e21 e14) (diff e35 e22 e13) (diff e35 e23 e12) (diff e35 e24 e11) (diff e35 e25 e10) (diff e35 e26 e9) (diff e35 e27 e8) (diff e35 e28 e7) (diff e35 e29 e6) (diff e35 e30 e5) (diff e35 e31 e4) (diff e35 e32 e3) (diff e35 e33 e2) (diff e35 e34 e1) (diff e35 e35 e0) (diff e36 e0 e36) (diff e36 e1 e35) (diff e36 e2 e34) (diff e36 e3 e33) (diff e36 e4 e32) (diff e36 e5 e31) (diff e36 e6 e30) (diff e36 e7 e29) (diff e36 e8 e28) (diff e36 e9 e27) (diff e36 e10 e26) (diff e36 e11 e25) (diff e36 e12 e24) (diff e36 e13 e23) (diff e36 e14 e22) (diff e36 e15 e21) (diff e36 e16 e20) (diff e36 e17 e19) (diff e36 e18 e18) (diff e36 e19 e17) (diff e36 e20 e16) (diff e36 e21 e15) (diff e36 e22 e14) (diff e36 e23 e13) (diff e36 e24 e12) (diff e36 e25 e11) (diff e36 e26 e10) (diff e36 e27 e9) (diff e36 e28 e8) (diff e36 e29 e7) (diff e36 e30 e6) (diff e36 e31 e5) (diff e36 e32 e4) (diff e36 e33 e3) (diff e36 e34 e2) (diff e36 e35 e1) (diff e36 e36 e0) (diff e37 e0 e37) (diff e37 e1 e36) (diff e37 e2 e35) (diff e37 e3 e34) (diff e37 e4 e33) (diff e37 e5 e32) (diff e37 e6 e31) (diff e37 e7 e30) (diff e37 e8 e29) (diff e37 e9 e28) (diff e37 e10 e27) (diff e37 e11 e26) (diff e37 e12 e25) (diff e37 e13 e24) (diff e37 e14 e23) (diff e37 e15 e22) (diff e37 e16 e21) (diff e37 e17 e20) (diff e37 e18 e19) (diff e37 e19 e18) (diff e37 e20 e17) (diff e37 e21 e16) (diff e37 e22 e15) (diff e37 e23 e14) (diff e37 e24 e13) (diff e37 e25 e12) (diff e37 e26 e11) (diff e37 e27 e10) (diff e37 e28 e9) (diff e37 e29 e8) (diff e37 e30 e7) (diff e37 e31 e6) (diff e37 e32 e5) (diff e37 e33 e4) (diff e37 e34 e3) (diff e37 e35 e2) (diff e37 e36 e1) (diff e37 e37 e0) (diff e38 e0 e38) (diff e38 e1 e37) (diff e38 e2 e36) (diff e38 e3 e35) (diff e38 e4 e34) (diff e38 e5 e33) (diff e38 e6 e32) (diff e38 e7 e31) (diff e38 e8 e30) (diff e38 e9 e29) (diff e38 e10 e28) (diff e38 e11 e27) (diff e38 e12 e26) (diff e38 e13 e25) (diff e38 e14 e24) (diff e38 e15 e23) (diff e38 e16 e22) (diff e38 e17 e21) (diff e38 e18 e20) (diff e38 e19 e19) (diff e38 e20 e18) (diff e38 e21 e17) (diff e38 e22 e16) (diff e38 e23 e15) (diff e38 e24 e14) (diff e38 e25 e13) (diff e38 e26 e12) (diff e38 e27 e11) (diff e38 e28 e10) (diff e38 e29 e9) (diff e38 e30 e8) (diff e38 e31 e7) (diff e38 e32 e6) (diff e38 e33 e5) (diff e38 e34 e4) (diff e38 e35 e3) (diff e38 e36 e2) (diff e38 e37 e1) (diff e38 e38 e0) (diff e39 e0 e39) (diff e39 e1 e38) (diff e39 e2 e37) (diff e39 e3 e36) (diff e39 e4 e35) (diff e39 e5 e34) (diff e39 e6 e33) (diff e39 e7 e32) (diff e39 e8 e31) (diff e39 e9 e30) (diff e39 e10 e29) (diff e39 e11 e28) (diff e39 e12 e27) (diff e39 e13 e26) (diff e39 e14 e25) (diff e39 e15 e24) (diff e39 e16 e23) (diff e39 e17 e22) (diff e39 e18 e21) (diff e39 e19 e20) (diff e39 e20 e19) (diff e39 e21 e18) (diff e39 e22 e17) (diff e39 e23 e16) (diff e39 e24 e15) (diff e39 e25 e14) (diff e39 e26 e13) (diff e39 e27 e12) (diff e39 e28 e11) (diff e39 e29 e10) (diff e39 e30 e9) (diff e39 e31 e8) (diff e39 e32 e7) (diff e39 e33 e6) (diff e39 e34 e5) (diff e39 e35 e4) (diff e39 e36 e3) (diff e39 e37 e2) (diff e39 e38 e1) (diff e39 e39 e0) (diff e40 e0 e40) (diff e40 e1 e39) (diff e40 e2 e38) (diff e40 e3 e37) (diff e40 e4 e36) (diff e40 e5 e35) (diff e40 e6 e34) (diff e40 e7 e33) (diff e40 e8 e32) (diff e40 e9 e31) (diff e40 e10 e30) (diff e40 e11 e29) (diff e40 e12 e28) (diff e40 e13 e27) (diff e40 e14 e26) (diff e40 e15 e25) (diff e40 e16 e24) (diff e40 e17 e23) (diff e40 e18 e22) (diff e40 e19 e21) (diff e40 e20 e20) (diff e40 e21 e19) (diff e40 e22 e18) (diff e40 e23 e17) (diff e40 e24 e16) (diff e40 e25 e15) (diff e40 e26 e14) (diff e40 e27 e13) (diff e40 e28 e12) (diff e40 e29 e11) (diff e40 e30 e10) (diff e40 e31 e9) (diff e40 e32 e8) (diff e40 e33 e7) (diff e40 e34 e6) (diff e40 e35 e5) (diff e40 e36 e4) (diff e40 e37 e3) (diff e40 e38 e2) (diff e40 e39 e1) (diff e40 e40 e0) (diff e41 e0 e41) (diff e41 e1 e40) (diff e41 e2 e39) (diff e41 e3 e38) (diff e41 e4 e37) (diff e41 e5 e36) (diff e41 e6 e35) (diff e41 e7 e34) (diff e41 e8 e33) (diff e41 e9 e32) (diff e41 e10 e31) (diff e41 e11 e30) (diff e41 e12 e29) (diff e41 e13 e28) (diff e41 e14 e27) (diff e41 e15 e26) (diff e41 e16 e25) (diff e41 e17 e24) (diff e41 e18 e23) (diff e41 e19 e22) (diff e41 e20 e21) (diff e41 e21 e20) (diff e41 e22 e19) (diff e41 e23 e18) (diff e41 e24 e17) (diff e41 e25 e16) (diff e41 e26 e15) (diff e41 e27 e14) (diff e41 e28 e13) (diff e41 e29 e12) (diff e41 e30 e11) (diff e41 e31 e10) (diff e41 e32 e9) (diff e41 e33 e8) (diff e41 e34 e7) (diff e41 e35 e6) (diff e41 e36 e5) (diff e41 e37 e4) (diff e41 e38 e3) (diff e41 e39 e2) (diff e41 e40 e1) (diff e41 e41 e0) (diff e42 e0 e42) (diff e42 e1 e41) (diff e42 e2 e40) (diff e42 e3 e39) (diff e42 e4 e38) (diff e42 e5 e37) (diff e42 e6 e36) (diff e42 e7 e35) (diff e42 e8 e34) (diff e42 e9 e33) (diff e42 e10 e32) (diff e42 e11 e31) (diff e42 e12 e30) (diff e42 e13 e29) (diff e42 e14 e28) (diff e42 e15 e27) (diff e42 e16 e26) (diff e42 e17 e25) (diff e42 e18 e24) (diff e42 e19 e23) (diff e42 e20 e22) (diff e42 e21 e21) (diff e42 e22 e20) (diff e42 e23 e19) (diff e42 e24 e18) (diff e42 e25 e17) (diff e42 e26 e16) (diff e42 e27 e15) (diff e42 e28 e14) (diff e42 e29 e13) (diff e42 e30 e12) (diff e42 e31 e11) (diff e42 e32 e10) (diff e42 e33 e9) (diff e42 e34 e8) (diff e42 e35 e7) (diff e42 e36 e6) (diff e42 e37 e5) (diff e42 e38 e4) (diff e42 e39 e3) (diff e42 e40 e2) (diff e42 e41 e1) (diff e42 e42 e0) (diff e43 e0 e43) (diff e43 e1 e42) (diff e43 e2 e41) (diff e43 e3 e40) (diff e43 e4 e39) (diff e43 e5 e38) (diff e43 e6 e37) (diff e43 e7 e36) (diff e43 e8 e35) (diff e43 e9 e34) (diff e43 e10 e33) (diff e43 e11 e32) (diff e43 e12 e31) (diff e43 e13 e30) (diff e43 e14 e29) (diff e43 e15 e28) (diff e43 e16 e27) (diff e43 e17 e26) (diff e43 e18 e25) (diff e43 e19 e24) (diff e43 e20 e23) (diff e43 e21 e22) (diff e43 e22 e21) (diff e43 e23 e20) (diff e43 e24 e19) (diff e43 e25 e18) (diff e43 e26 e17) (diff e43 e27 e16) (diff e43 e28 e15) (diff e43 e29 e14) (diff e43 e30 e13) (diff e43 e31 e12) (diff e43 e32 e11) (diff e43 e33 e10) (diff e43 e34 e9) (diff e43 e35 e8) (diff e43 e36 e7) (diff e43 e37 e6) (diff e43 e38 e5) (diff e43 e39 e4) (diff e43 e40 e3) (diff e43 e41 e2) (diff e43 e42 e1) (diff e43 e43 e0) (diff e44 e0 e44) (diff e44 e1 e43) (diff e44 e2 e42) (diff e44 e3 e41) (diff e44 e4 e40) (diff e44 e5 e39) (diff e44 e6 e38) (diff e44 e7 e37) (diff e44 e8 e36) (diff e44 e9 e35) (diff e44 e10 e34) (diff e44 e11 e33) (diff e44 e12 e32) (diff e44 e13 e31) (diff e44 e14 e30) (diff e44 e15 e29) (diff e44 e16 e28) (diff e44 e17 e27) (diff e44 e18 e26) (diff e44 e19 e25) (diff e44 e20 e24) (diff e44 e21 e23) (diff e44 e22 e22) (diff e44 e23 e21) (diff e44 e24 e20) (diff e44 e25 e19) (diff e44 e26 e18) (diff e44 e27 e17) (diff e44 e28 e16) (diff e44 e29 e15) (diff e44 e30 e14) (diff e44 e31 e13) (diff e44 e32 e12) (diff e44 e33 e11) (diff e44 e34 e10) (diff e44 e35 e9) (diff e44 e36 e8) (diff e44 e37 e7) (diff e44 e38 e6) (diff e44 e39 e5) (diff e44 e40 e4) (diff e44 e41 e3) (diff e44 e42 e2) (diff e44 e43 e1) (diff e44 e44 e0) (diff e45 e0 e45) (diff e45 e1 e44) (diff e45 e2 e43) (diff e45 e3 e42) (diff e45 e4 e41) (diff e45 e5 e40) (diff e45 e6 e39) (diff e45 e7 e38) (diff e45 e8 e37) (diff e45 e9 e36) (diff e45 e10 e35) (diff e45 e11 e34) (diff e45 e12 e33) (diff e45 e13 e32) (diff e45 e14 e31) (diff e45 e15 e30) (diff e45 e16 e29) (diff e45 e17 e28) (diff e45 e18 e27) (diff e45 e19 e26) (diff e45 e20 e25) (diff e45 e21 e24) (diff e45 e22 e23) (diff e45 e23 e22) (diff e45 e24 e21) (diff e45 e25 e20) (diff e45 e26 e19) (diff e45 e27 e18) (diff e45 e28 e17) (diff e45 e29 e16) (diff e45 e30 e15) (diff e45 e31 e14) (diff e45 e32 e13) (diff e45 e33 e12) (diff e45 e34 e11) (diff e45 e35 e10) (diff e45 e36 e9) (diff e45 e37 e8) (diff e45 e38 e7) (diff e45 e39 e6) (diff e45 e40 e5) (diff e45 e41 e4) (diff e45 e42 e3) (diff e45 e43 e2) (diff e45 e44 e1) (diff e45 e45 e0) (diff e46 e0 e46) (diff e46 e1 e45) (diff e46 e2 e44) (diff e46 e3 e43) (diff e46 e4 e42) (diff e46 e5 e41) (diff e46 e6 e40) (diff e46 e7 e39) (diff e46 e8 e38) (diff e46 e9 e37) (diff e46 e10 e36) (diff e46 e11 e35) (diff e46 e12 e34) (diff e46 e13 e33) (diff e46 e14 e32) (diff e46 e15 e31) (diff e46 e16 e30) (diff e46 e17 e29) (diff e46 e18 e28) (diff e46 e19 e27) (diff e46 e20 e26) (diff e46 e21 e25) (diff e46 e22 e24) (diff e46 e23 e23) (diff e46 e24 e22) (diff e46 e25 e21) (diff e46 e26 e20) (diff e46 e27 e19) (diff e46 e28 e18) (diff e46 e29 e17) (diff e46 e30 e16) (diff e46 e31 e15) (diff e46 e32 e14) (diff e46 e33 e13) (diff e46 e34 e12) (diff e46 e35 e11) (diff e46 e36 e10) (diff e46 e37 e9) (diff e46 e38 e8) (diff e46 e39 e7) (diff e46 e40 e6) (diff e46 e41 e5) (diff e46 e42 e4) (diff e46 e43 e3) (diff e46 e44 e2) (diff e46 e45 e1) (diff e46 e46 e0) (diff e47 e0 e47) (diff e47 e1 e46) (diff e47 e2 e45) (diff e47 e3 e44) (diff e47 e4 e43) (diff e47 e5 e42) (diff e47 e6 e41) (diff e47 e7 e40) (diff e47 e8 e39) (diff e47 e9 e38) (diff e47 e10 e37) (diff e47 e11 e36) (diff e47 e12 e35) (diff e47 e13 e34) (diff e47 e14 e33) (diff e47 e15 e32) (diff e47 e16 e31) (diff e47 e17 e30) (diff e47 e18 e29) (diff e47 e19 e28) (diff e47 e20 e27) (diff e47 e21 e26) (diff e47 e22 e25) (diff e47 e23 e24) (diff e47 e24 e23) (diff e47 e25 e22) (diff e47 e26 e21) (diff e47 e27 e20) (diff e47 e28 e19) (diff e47 e29 e18) (diff e47 e30 e17) (diff e47 e31 e16) (diff e47 e32 e15) (diff e47 e33 e14) (diff e47 e34 e13) (diff e47 e35 e12) (diff e47 e36 e11) (diff e47 e37 e10) (diff e47 e38 e9) (diff e47 e39 e8) (diff e47 e40 e7) (diff e47 e41 e6) (diff e47 e42 e5) (diff e47 e43 e4) (diff e47 e44 e3) (diff e47 e45 e2) (diff e47 e46 e1) (diff e47 e47 e0) (diff e48 e0 e48) (diff e48 e1 e47) (diff e48 e2 e46) (diff e48 e3 e45) (diff e48 e4 e44) (diff e48 e5 e43) (diff e48 e6 e42) (diff e48 e7 e41) (diff e48 e8 e40) (diff e48 e9 e39) (diff e48 e10 e38) (diff e48 e11 e37) (diff e48 e12 e36) (diff e48 e13 e35) (diff e48 e14 e34) (diff e48 e15 e33) (diff e48 e16 e32) (diff e48 e17 e31) (diff e48 e18 e30) (diff e48 e19 e29) (diff e48 e20 e28) (diff e48 e21 e27) (diff e48 e22 e26) (diff e48 e23 e25) (diff e48 e24 e24) (diff e48 e25 e23) (diff e48 e26 e22) (diff e48 e27 e21) (diff e48 e28 e20) (diff e48 e29 e19) (diff e48 e30 e18) (diff e48 e31 e17) (diff e48 e32 e16) (diff e48 e33 e15) (diff e48 e34 e14) (diff e48 e35 e13) (diff e48 e36 e12) (diff e48 e37 e11) (diff e48 e38 e10) (diff e48 e39 e9) (diff e48 e40 e8) (diff e48 e41 e7) (diff e48 e42 e6) (diff e48 e43 e5) (diff e48 e44 e4) (diff e48 e45 e3) (diff e48 e46 e2) (diff e48 e47 e1) (diff e48 e48 e0) (diff e49 e0 e49) (diff e49 e1 e48) (diff e49 e2 e47) (diff e49 e3 e46) (diff e49 e4 e45) (diff e49 e5 e44) (diff e49 e6 e43) (diff e49 e7 e42) (diff e49 e8 e41) (diff e49 e9 e40) (diff e49 e10 e39) (diff e49 e11 e38) (diff e49 e12 e37) (diff e49 e13 e36) (diff e49 e14 e35) (diff e49 e15 e34) (diff e49 e16 e33) (diff e49 e17 e32) (diff e49 e18 e31) (diff e49 e19 e30) (diff e49 e20 e29) (diff e49 e21 e28) (diff e49 e22 e27) (diff e49 e23 e26) (diff e49 e24 e25) (diff e49 e25 e24) (diff e49 e26 e23) (diff e49 e27 e22) (diff e49 e28 e21) (diff e49 e29 e20) (diff e49 e30 e19) (diff e49 e31 e18) (diff e49 e32 e17) (diff e49 e33 e16) (diff e49 e34 e15) (diff e49 e35 e14) (diff e49 e36 e13) (diff e49 e37 e12) (diff e49 e38 e11) (diff e49 e39 e10) (diff e49 e40 e9) (diff e49 e41 e8) (diff e49 e42 e7) (diff e49 e43 e6) (diff e49 e44 e5) (diff e49 e45 e4) (diff e49 e46 e3) (diff e49 e47 e2) (diff e49 e48 e1) (diff e49 e49 e0) (diff e50 e0 e50) (diff e50 e1 e49) (diff e50 e2 e48) (diff e50 e3 e47) (diff e50 e4 e46) (diff e50 e5 e45) (diff e50 e6 e44) (diff e50 e7 e43) (diff e50 e8 e42) (diff e50 e9 e41) (diff e50 e10 e40) (diff e50 e11 e39) (diff e50 e12 e38) (diff e50 e13 e37) (diff e50 e14 e36) (diff e50 e15 e35) (diff e50 e16 e34) (diff e50 e17 e33) (diff e50 e18 e32) (diff e50 e19 e31) (diff e50 e20 e30) (diff e50 e21 e29) (diff e50 e22 e28) (diff e50 e23 e27) (diff e50 e24 e26) (diff e50 e25 e25) (diff e50 e26 e24) (diff e50 e27 e23) (diff e50 e28 e22) (diff e50 e29 e21) (diff e50 e30 e20) (diff e50 e31 e19) (diff e50 e32 e18) (diff e50 e33 e17) (diff e50 e34 e16) (diff e50 e35 e15) (diff e50 e36 e14) (diff e50 e37 e13) (diff e50 e38 e12) (diff e50 e39 e11) (diff e50 e40 e10) (diff e50 e41 e9) (diff e50 e42 e8) (diff e50 e43 e7) (diff e50 e44 e6) (diff e50 e45 e5) (diff e50 e46 e4) (diff e50 e47 e3) (diff e50 e48 e2) (diff e50 e49 e1) (diff e50 e50 e0) (diff e51 e0 e51) (diff e51 e1 e50) (diff e51 e2 e49) (diff e51 e3 e48) (diff e51 e4 e47) (diff e51 e5 e46) (diff e51 e6 e45) (diff e51 e7 e44) (diff e51 e8 e43) (diff e51 e9 e42) (diff e51 e10 e41) (diff e51 e11 e40) (diff e51 e12 e39) (diff e51 e13 e38) (diff e51 e14 e37) (diff e51 e15 e36) (diff e51 e16 e35) (diff e51 e17 e34) (diff e51 e18 e33) (diff e51 e19 e32) (diff e51 e20 e31) (diff e51 e21 e30) (diff e51 e22 e29) (diff e51 e23 e28) (diff e51 e24 e27) (diff e51 e25 e26) (diff e51 e26 e25) (diff e51 e27 e24) (diff e51 e28 e23) (diff e51 e29 e22) (diff e51 e30 e21) (diff e51 e31 e20) (diff e51 e32 e19) (diff e51 e33 e18) (diff e51 e34 e17) (diff e51 e35 e16) (diff e51 e36 e15) (diff e51 e37 e14) (diff e51 e38 e13) (diff e51 e39 e12) (diff e51 e40 e11) (diff e51 e41 e10) (diff e51 e42 e9) (diff e51 e43 e8) (diff e51 e44 e7) (diff e51 e45 e6) (diff e51 e46 e5) (diff e51 e47 e4) (diff e51 e48 e3) (diff e51 e49 e2) (diff e51 e50 e1) (diff e51 e51 e0) (diff e52 e0 e52) (diff e52 e1 e51) (diff e52 e2 e50) (diff e52 e3 e49) (diff e52 e4 e48) (diff e52 e5 e47) (diff e52 e6 e46) (diff e52 e7 e45) (diff e52 e8 e44) (diff e52 e9 e43) (diff e52 e10 e42) (diff e52 e11 e41) (diff e52 e12 e40) (diff e52 e13 e39) (diff e52 e14 e38) (diff e52 e15 e37) (diff e52 e16 e36) (diff e52 e17 e35) (diff e52 e18 e34) (diff e52 e19 e33) (diff e52 e20 e32) (diff e52 e21 e31) (diff e52 e22 e30) (diff e52 e23 e29) (diff e52 e24 e28) (diff e52 e25 e27) (diff e52 e26 e26) (diff e52 e27 e25) (diff e52 e28 e24) (diff e52 e29 e23) (diff e52 e30 e22) (diff e52 e31 e21) (diff e52 e32 e20) (diff e52 e33 e19) (diff e52 e34 e18) (diff e52 e35 e17) (diff e52 e36 e16) (diff e52 e37 e15) (diff e52 e38 e14) (diff e52 e39 e13) (diff e52 e40 e12) (diff e52 e41 e11) (diff e52 e42 e10) (diff e52 e43 e9) (diff e52 e44 e8) (diff e52 e45 e7) (diff e52 e46 e6) (diff e52 e47 e5) (diff e52 e48 e4) (diff e52 e49 e3) (diff e52 e50 e2) (diff e52 e51 e1) (diff e52 e52 e0) (diff e53 e0 e53) (diff e53 e1 e52) (diff e53 e2 e51) (diff e53 e3 e50) (diff e53 e4 e49) (diff e53 e5 e48) (diff e53 e6 e47) (diff e53 e7 e46) (diff e53 e8 e45) (diff e53 e9 e44) (diff e53 e10 e43) (diff e53 e11 e42) (diff e53 e12 e41) (diff e53 e13 e40) (diff e53 e14 e39) (diff e53 e15 e38) (diff e53 e16 e37) (diff e53 e17 e36) (diff e53 e18 e35) (diff e53 e19 e34) (diff e53 e20 e33) (diff e53 e21 e32) (diff e53 e22 e31) (diff e53 e23 e30) (diff e53 e24 e29) (diff e53 e25 e28) (diff e53 e26 e27) (diff e53 e27 e26) (diff e53 e28 e25) (diff e53 e29 e24) (diff e53 e30 e23) (diff e53 e31 e22) (diff e53 e32 e21) (diff e53 e33 e20) (diff e53 e34 e19) (diff e53 e35 e18) (diff e53 e36 e17) (diff e53 e37 e16) (diff e53 e38 e15) (diff e53 e39 e14) (diff e53 e40 e13) (diff e53 e41 e12) (diff e53 e42 e11) (diff e53 e43 e10) (diff e53 e44 e9) (diff e53 e45 e8) (diff e53 e46 e7) (diff e53 e47 e6) (diff e53 e48 e5) (diff e53 e49 e4) (diff e53 e50 e3) (diff e53 e51 e2) (diff e53 e52 e1) (diff e53 e53 e0) (diff e54 e0 e54) (diff e54 e1 e53) (diff e54 e2 e52) (diff e54 e3 e51) (diff e54 e4 e50) (diff e54 e5 e49) (diff e54 e6 e48) (diff e54 e7 e47) (diff e54 e8 e46) (diff e54 e9 e45) (diff e54 e10 e44) (diff e54 e11 e43) (diff e54 e12 e42) (diff e54 e13 e41) (diff e54 e14 e40) (diff e54 e15 e39) (diff e54 e16 e38) (diff e54 e17 e37) (diff e54 e18 e36) (diff e54 e19 e35) (diff e54 e20 e34) (diff e54 e21 e33) (diff e54 e22 e32) (diff e54 e23 e31) (diff e54 e24 e30) (diff e54 e25 e29) (diff e54 e26 e28) (diff e54 e27 e27) (diff e54 e28 e26) (diff e54 e29 e25) (diff e54 e30 e24) (diff e54 e31 e23) (diff e54 e32 e22) (diff e54 e33 e21) (diff e54 e34 e20) (diff e54 e35 e19) (diff e54 e36 e18) (diff e54 e37 e17) (diff e54 e38 e16) (diff e54 e39 e15) (diff e54 e40 e14) (diff e54 e41 e13) (diff e54 e42 e12) (diff e54 e43 e11) (diff e54 e44 e10) (diff e54 e45 e9) (diff e54 e46 e8) (diff e54 e47 e7) (diff e54 e48 e6) (diff e54 e49 e5) (diff e54 e50 e4) (diff e54 e51 e3) (diff e54 e52 e2) (diff e54 e53 e1) (diff e54 e54 e0) (diff e55 e0 e55) (diff e55 e1 e54) (diff e55 e2 e53) (diff e55 e3 e52) (diff e55 e4 e51) (diff e55 e5 e50) (diff e55 e6 e49) (diff e55 e7 e48) (diff e55 e8 e47) (diff e55 e9 e46) (diff e55 e10 e45) (diff e55 e11 e44) (diff e55 e12 e43) (diff e55 e13 e42) (diff e55 e14 e41) (diff e55 e15 e40) (diff e55 e16 e39) (diff e55 e17 e38) (diff e55 e18 e37) (diff e55 e19 e36) (diff e55 e20 e35) (diff e55 e21 e34) (diff e55 e22 e33) (diff e55 e23 e32) (diff e55 e24 e31) (diff e55 e25 e30) (diff e55 e26 e29) (diff e55 e27 e28) (diff e55 e28 e27) (diff e55 e29 e26) (diff e55 e30 e25) (diff e55 e31 e24) (diff e55 e32 e23) (diff e55 e33 e22) (diff e55 e34 e21) (diff e55 e35 e20) (diff e55 e36 e19) (diff e55 e37 e18) (diff e55 e38 e17) (diff e55 e39 e16) (diff e55 e40 e15) (diff e55 e41 e14) (diff e55 e42 e13) (diff e55 e43 e12) (diff e55 e44 e11) (diff e55 e45 e10) (diff e55 e46 e9) (diff e55 e47 e8) (diff e55 e48 e7) (diff e55 e49 e6) (diff e55 e50 e5) (diff e55 e51 e4) (diff e55 e52 e3) (diff e55 e53 e2) (diff e55 e54 e1) (diff e55 e55 e0) (diff e56 e0 e56) (diff e56 e1 e55) (diff e56 e2 e54) (diff e56 e3 e53) (diff e56 e4 e52) (diff e56 e5 e51) (diff e56 e6 e50) (diff e56 e7 e49) (diff e56 e8 e48) (diff e56 e9 e47) (diff e56 e10 e46) (diff e56 e11 e45) (diff e56 e12 e44) (diff e56 e13 e43) (diff e56 e14 e42) (diff e56 e15 e41) (diff e56 e16 e40) (diff e56 e17 e39) (diff e56 e18 e38) (diff e56 e19 e37) (diff e56 e20 e36) (diff e56 e21 e35) (diff e56 e22 e34) (diff e56 e23 e33) (diff e56 e24 e32) (diff e56 e25 e31) (diff e56 e26 e30) (diff e56 e27 e29) (diff e56 e28 e28) (diff e56 e29 e27) (diff e56 e30 e26) (diff e56 e31 e25) (diff e56 e32 e24) (diff e56 e33 e23) (diff e56 e34 e22) (diff e56 e35 e21) (diff e56 e36 e20) (diff e56 e37 e19) (diff e56 e38 e18) (diff e56 e39 e17) (diff e56 e40 e16) (diff e56 e41 e15) (diff e56 e42 e14) (diff e56 e43 e13) (diff e56 e44 e12) (diff e56 e45 e11) (diff e56 e46 e10) (diff e56 e47 e9) (diff e56 e48 e8) (diff e56 e49 e7) (diff e56 e50 e6) (diff e56 e51 e5) (diff e56 e52 e4) (diff e56 e53 e3) (diff e56 e54 e2) (diff e56 e55 e1) (diff e56 e56 e0) (diff e57 e0 e57) (diff e57 e1 e56) (diff e57 e2 e55) (diff e57 e3 e54) (diff e57 e4 e53) (diff e57 e5 e52) (diff e57 e6 e51) (diff e57 e7 e50) (diff e57 e8 e49) (diff e57 e9 e48) (diff e57 e10 e47) (diff e57 e11 e46) (diff e57 e12 e45) (diff e57 e13 e44) (diff e57 e14 e43) (diff e57 e15 e42) (diff e57 e16 e41) (diff e57 e17 e40) (diff e57 e18 e39) (diff e57 e19 e38) (diff e57 e20 e37) (diff e57 e21 e36) (diff e57 e22 e35) (diff e57 e23 e34) (diff e57 e24 e33) (diff e57 e25 e32) (diff e57 e26 e31) (diff e57 e27 e30) (diff e57 e28 e29) (diff e57 e29 e28) (diff e57 e30 e27) (diff e57 e31 e26) (diff e57 e32 e25) (diff e57 e33 e24) (diff e57 e34 e23) (diff e57 e35 e22) (diff e57 e36 e21) (diff e57 e37 e20) (diff e57 e38 e19) (diff e57 e39 e18) (diff e57 e40 e17) (diff e57 e41 e16) (diff e57 e42 e15) (diff e57 e43 e14) (diff e57 e44 e13) (diff e57 e45 e12) (diff e57 e46 e11) (diff e57 e47 e10) (diff e57 e48 e9) (diff e57 e49 e8) (diff e57 e50 e7) (diff e57 e51 e6) (diff e57 e52 e5) (diff e57 e53 e4) (diff e57 e54 e3) (diff e57 e55 e2) (diff e57 e56 e1) (diff e57 e57 e0) (available_energy rover0 e57)) (:goal (and (communicated_soil_data waypoint2) (communicated_soil_data waypoint4) (communicated_rock_data waypoint4) (communicated_rock_data waypoint2))) (:metric minimize (total-cost)))