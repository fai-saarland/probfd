(define (problem rect-15-15-4-4-9)
  (:domain rectangle-world)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 horzn21 horzn22 - horizon-value n0 n1 n2 n3 n4 n5 n6 n7 n8 n9 n10 n11 n12 n13 n14 - int)
  (:init (horizon horzn22) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (horizon-decrement horzn15 horzn14) (horizon-decrement horzn16 horzn15) (horizon-decrement horzn17 horzn16) (horizon-decrement horzn18 horzn17) (horizon-decrement horzn19 horzn18) (horizon-decrement horzn20 horzn19) (horizon-decrement horzn21 horzn20) (horizon-decrement horzn22 horzn21) (= (total-cost) 0) (xpos n0)
	 (ypos n0)
         (next n0 n1)
         (next n1 n2)
         (next n2 n3)
         (next n3 n4)
         (next n4 n5)
         (next n5 n6)
         (next n6 n7)
         (next n7 n8)
         (next n8 n9)
         (next n9 n10)
         (next n10 n11)
         (next n11 n12)
         (next n12 n13)
         (next n13 n14)
	 (safeX n7)
	 (safeX n10)
	 (safeX n14)
	 (safeY n5)
	 (safeY n8)
	 (safeY n13)
	 (unsafe n0 n2)
	 (unsafe n0 n3)
	 (unsafe n0 n4)
	 (unsafe n0 n5)
	 (unsafe n0 n12)
	 (unsafe n1 n0)
	 (unsafe n1 n3)
	 (unsafe n1 n4)
	 (unsafe n1 n6)
	 (unsafe n1 n13)
	 (unsafe n2 n1)
	 (unsafe n2 n2)
	 (unsafe n2 n7)
	 (unsafe n2 n9)
	 (unsafe n3 n5)
	 (unsafe n3 n10)
	 (unsafe n3 n12)
	 (unsafe n4 n4)
	 (unsafe n4 n7)
	 (unsafe n4 n9)
	 (unsafe n4 n10)
	 (unsafe n4 n11)
	 (unsafe n5 n8)
	 (unsafe n5 n10)
	 (unsafe n6 n1)
	 (unsafe n6 n6)
	 (unsafe n6 n14)
	 (unsafe n7 n3)
	 (unsafe n7 n5)
	 (unsafe n7 n9)
	 (unsafe n7 n14)
	 (unsafe n9 n1)
	 (unsafe n9 n6)
	 (unsafe n9 n8)
	 (unsafe n9 n10)
	 (unsafe n9 n13)
	 (unsafe n9 n14)
	 (unsafe n10 n2)
	 (unsafe n10 n6)
	 (unsafe n10 n14)
	 (unsafe n11 n0)
	 (unsafe n11 n2)
	 (unsafe n11 n5)
	 (unsafe n11 n11)
	 (unsafe n12 n2)
	 (unsafe n12 n6)
	 (unsafe n12 n8)
	 (unsafe n12 n10)
	 (unsafe n12 n13)
	 (unsafe n13 n1)
	 (unsafe n13 n4)
	 (unsafe n13 n9)
	 (unsafe n13 n12)
	 (unsafe n14 n0)
	 (unsafe n14 n7)
  )
  (:goal (and (not (dead)) (xpos n14) (ypos n14)))
  (:metric minimize (total-cost))
)
