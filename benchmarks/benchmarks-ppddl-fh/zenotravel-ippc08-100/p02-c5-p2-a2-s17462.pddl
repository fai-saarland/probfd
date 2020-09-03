(define (problem zeno_5_2_2_17462)
  (:domain zenotravel)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 - horizon-value c0 c1 c2 c3 c4 - city p0 p1 - person a0 a1 - aircraft f0 f1 f2 f3 f4 - flevel)
  (:init (horizon horzn8) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (next f0 f1) (next f1 f2) (next f2 f3) (next f3 f4)
         (at-person p0 c2) (not-boarding p0) (not-debarking p0)
         (at-person p1 c3) (not-boarding p1) (not-debarking p1)
         (at-aircraft a0 c0) (fuel-level a0 f0) (not-refueling a0)
         (at-aircraft a1 c0) (fuel-level a1 f4) (not-refueling a1)
  )
  (:goal (and (at-person p0 c1) (at-person p1 c3)))(:metric minimize (total-cost))
)
