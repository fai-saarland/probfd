(define (problem zeno_10_2_2_24056)
  (:domain zenotravel)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 horzn21 horzn22 horzn23 horzn24 horzn25 - horizon-value c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 - city p0 p1 - person a0 a1 - aircraft f0 f1 f2 f3 f4 - flevel)
  (:init (horizon horzn25) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (horizon-decrement horzn15 horzn14) (horizon-decrement horzn16 horzn15) (horizon-decrement horzn17 horzn16) (horizon-decrement horzn18 horzn17) (horizon-decrement horzn19 horzn18) (horizon-decrement horzn20 horzn19) (horizon-decrement horzn21 horzn20) (horizon-decrement horzn22 horzn21) (horizon-decrement horzn23 horzn22) (horizon-decrement horzn24 horzn23) (horizon-decrement horzn25 horzn24) (next f0 f1) (next f1 f2) (next f2 f3) (next f3 f4)
         (at-person p0 c2) (not-boarding p0) (not-debarking p0)
         (at-person p1 c0) (not-boarding p1) (not-debarking p1)
         (at-aircraft a0 c5) (fuel-level a0 f0) (not-refueling a0)
         (at-aircraft a1 c8) (fuel-level a1 f3) (not-refueling a1)
  )
  (:goal (and (at-person p0 c6) (at-person p1 c6)))
  (:metric minimize (total-cost))
)
