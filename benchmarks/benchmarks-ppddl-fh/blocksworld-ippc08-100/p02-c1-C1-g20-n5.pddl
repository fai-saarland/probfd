(define (problem bw_5_p02)
  (:domain blocks-domain)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 - horizon-value b1 b2 b3 b4 b5 - block)
  (:init (horizon horzn10) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (emptyhand) (on-table b1) (on-table b2) (on b3 b5) (on b4 b1) (on-table b5) (clear b2) (clear b3) (clear b4))
  (:goal (and (emptyhand) (on b1 b3) (on b2 b4) (on-table b3) (on b4 b1) (on b5 b2) (clear b5)))
  (:metric minimize (total-cost))
)
