(define (problem bw_10_11511)
  (:domain blocks-domain)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 horzn21 horzn22 horzn23 horzn24 horzn25 - horizon-value b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 - block)
  (:init (horizon horzn25) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (horizon-decrement horzn15 horzn14) (horizon-decrement horzn16 horzn15) (horizon-decrement horzn17 horzn16) (horizon-decrement horzn18 horzn17) (horizon-decrement horzn19 horzn18) (horizon-decrement horzn20 horzn19) (horizon-decrement horzn21 horzn20) (horizon-decrement horzn22 horzn21) (horizon-decrement horzn23 horzn22) (horizon-decrement horzn24 horzn23) (horizon-decrement horzn25 horzn24) (emptyhand) (on-table b1) (on b2 b5) (on b3 b8) (on-table b4) (on b5 b3) (on b6 b2) (on b7 b1) (on b8 b10) (on-table b9) (on b10 b9) (clear b4) (clear b6) (clear b7))
  (:goal (and (emptyhand) (on-table b1) (on b2 b4) (on b3 b10) (on b4 b9) (on b5 b8) (on b6 b5) (on b7 b2) (on b8 b3) (on b9 b1) (on b10 b7) (clear b6)))
  (:metric minimize (total-cost))
)
