(define (problem tire_23_0_15471)
  (:domain tire)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 - horizon-value n0 n1 n2 n3 n4 n5 n6 n7 n8 n9 n10 n11 n12 n13 n14 n15 n16 n17 n18 n19 n20 n21 n22 - location)
  (:init (horizon horzn6) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (vehicle-at n5)
         (road n0 n2) (road n2 n0)
         (road n0 n8) (road n8 n0)
         (road n0 n15) (road n15 n0)
         (road n0 n19) (road n19 n0)
         (road n1 n12) (road n12 n1)
         (road n1 n15) (road n15 n1)
         (road n2 n8) (road n8 n2)
         (road n2 n9) (road n9 n2)
         (road n2 n14) (road n14 n2)
         (road n2 n22) (road n22 n2)
         (road n3 n7) (road n7 n3)
         (road n3 n10) (road n10 n3)
         (road n3 n15) (road n15 n3)
         (road n3 n17) (road n17 n3)
         (road n4 n15) (road n15 n4)
         (road n4 n20) (road n20 n4)
         (road n5 n8) (road n8 n5)
         (road n6 n8) (road n8 n6)
         (road n6 n15) (road n15 n6)
         (road n6 n18) (road n18 n6)
         (road n6 n20) (road n20 n6)
         (road n7 n9) (road n9 n7)
         (road n7 n12) (road n12 n7)
         (road n7 n14) (road n14 n7)
         (road n7 n15) (road n15 n7)
         (road n7 n16) (road n16 n7)
         (road n7 n18) (road n18 n7)
         (road n7 n21) (road n21 n7)
         (road n8 n11) (road n11 n8)
         (road n8 n12) (road n12 n8)
         (road n8 n14) (road n14 n8)
         (road n8 n16) (road n16 n8)
         (road n8 n17) (road n17 n8)
         (road n9 n11) (road n11 n9)
         (road n10 n12) (road n12 n10)
         (road n10 n13) (road n13 n10)
         (road n10 n16) (road n16 n10)
         (road n10 n21) (road n21 n10)
         (road n11 n20) (road n20 n11)
         (road n11 n21) (road n21 n11)
         (road n12 n16) (road n16 n12)
         (road n13 n14) (road n14 n13)
         (road n13 n21) (road n21 n13)
         (road n14 n20) (road n20 n14)
         (road n14 n21) (road n21 n14)
         (road n15 n19) (road n19 n15)
         (road n15 n20) (road n20 n15)
         (road n15 n21) (road n21 n15)
         (road n16 n18) (road n18 n16)
         (road n16 n20) (road n20 n16)
         (road n16 n22) (road n22 n16)
         (road n17 n18) (road n18 n17)
         (road n17 n20) (road n20 n17)
         (road n17 n21) (road n21 n17)
         (road n18 n19) (road n19 n18)
         (road n18 n20) (road n20 n18)
         (road n18 n21) (road n21 n18)
         (road n18 n22) (road n22 n18)
         (road n19 n22) (road n22 n19)
         (road n20 n21) (road n21 n20)
         (road n20 n22) (road n22 n20)
         (road n21 n22) (road n22 n21)
         (spare-in n0)
         (spare-in n1)
         (spare-in n4)
         (spare-in n6)
         (spare-in n8)
         (spare-in n9)
         (spare-in n11)
         (spare-in n14)
         (spare-in n18)
         (spare-in n19)
         (spare-in n20)
         (spare-in n21)
         (spare-in n22)
         (not-flattire)
  )
  (:goal (vehicle-at n15))
  (:metric minimize (total-cost))
)
