(define (problem ex_bw_5_15874)
  (:domain exploding-blocksworld)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 - horizon-value b1 b2 b3 b4 b5 - block)
  (:init (horizon horzn8) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (emptyhand) (on-table b1) (on b2 b3) (on-table b3) (on b4 b5) (on-table b5) (clear b1) (clear b2) (clear b4) (no-detonated b1) (no-destroyed b1) (no-detonated b2) (no-destroyed b2) (no-detonated b3) (no-destroyed b3) (no-detonated b4) (no-destroyed b4) (no-detonated b5) (no-destroyed b5) (no-destroyed-table))
  (:goal (and (on b3 b4) (on-table b5)))
  (:metric minimize (total-cost))
)
