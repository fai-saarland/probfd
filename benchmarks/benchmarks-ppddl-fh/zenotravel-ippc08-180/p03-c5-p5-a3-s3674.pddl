(define (problem zeno_5_5_3_3674)
  (:domain zenotravel)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 horzn21 horzn22 horzn23 horzn24 horzn25 horzn26 horzn27 horzn28 horzn29 horzn30 horzn31 horzn32 horzn33 horzn34 horzn35 horzn36 horzn37 horzn38 horzn39 horzn40 horzn41 horzn42 horzn43 horzn44 horzn45 horzn46 horzn47 horzn48 horzn49 horzn50 - horizon-value c0 c1 c2 c3 c4 - city p0 p1 p2 p3 p4 - person a0 a1 a2 - aircraft f0 f1 f2 f3 f4 - flevel)
  (:init (horizon horzn50) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (horizon-decrement horzn15 horzn14) (horizon-decrement horzn16 horzn15) (horizon-decrement horzn17 horzn16) (horizon-decrement horzn18 horzn17) (horizon-decrement horzn19 horzn18) (horizon-decrement horzn20 horzn19) (horizon-decrement horzn21 horzn20) (horizon-decrement horzn22 horzn21) (horizon-decrement horzn23 horzn22) (horizon-decrement horzn24 horzn23) (horizon-decrement horzn25 horzn24) (horizon-decrement horzn26 horzn25) (horizon-decrement horzn27 horzn26) (horizon-decrement horzn28 horzn27) (horizon-decrement horzn29 horzn28) (horizon-decrement horzn30 horzn29) (horizon-decrement horzn31 horzn30) (horizon-decrement horzn32 horzn31) (horizon-decrement horzn33 horzn32) (horizon-decrement horzn34 horzn33) (horizon-decrement horzn35 horzn34) (horizon-decrement horzn36 horzn35) (horizon-decrement horzn37 horzn36) (horizon-decrement horzn38 horzn37) (horizon-decrement horzn39 horzn38) (horizon-decrement horzn40 horzn39) (horizon-decrement horzn41 horzn40) (horizon-decrement horzn42 horzn41) (horizon-decrement horzn43 horzn42) (horizon-decrement horzn44 horzn43) (horizon-decrement horzn45 horzn44) (horizon-decrement horzn46 horzn45) (horizon-decrement horzn47 horzn46) (horizon-decrement horzn48 horzn47) (horizon-decrement horzn49 horzn48) (horizon-decrement horzn50 horzn49) (next f0 f1) (next f1 f2) (next f2 f3) (next f3 f4)
         (at-person p0 c3) (not-boarding p0) (not-debarking p0)
         (at-person p1 c1) (not-boarding p1) (not-debarking p1)
         (at-person p2 c2) (not-boarding p2) (not-debarking p2)
         (at-person p3 c3) (not-boarding p3) (not-debarking p3)
         (at-person p4 c2) (not-boarding p4) (not-debarking p4)
         (at-aircraft a0 c2) (fuel-level a0 f2) (not-refueling a0)
         (at-aircraft a1 c4) (fuel-level a1 f0) (not-refueling a1)
         (at-aircraft a2 c1) (fuel-level a2 f0) (not-refueling a2)
  )
  (:goal (and (at-person p0 c4) (at-person p1 c1) (at-person p2 c0) (at-person p3 c1) (at-person p4 c4)))(:metric minimize (total-cost))
)
