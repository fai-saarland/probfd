(define (problem tire_27_0_29082)
  (:domain tire)
  (:objects horzn0 horzn1 horzn2 horzn3 - horizon-value n0 n1 n2 n3 n4 n5 n6 n7 n8 n9 n10 n11 n12 n13 n14 n15 n16 n17 n18 n19 n20 n21 n22 n23 n24 n25 n26 - location)
  (:init (horizon horzn3) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (vehicle-at n2)
         (road n0 n1) (road n1 n0)
         (road n0 n21) (road n21 n0)
         (road n1 n11) (road n11 n1)
         (road n1 n17) (road n17 n1)
         (road n2 n15) (road n15 n2)
         (road n2 n23) (road n23 n2)
         (road n2 n24) (road n24 n2)
         (road n3 n4) (road n4 n3)
         (road n3 n8) (road n8 n3)
         (road n3 n17) (road n17 n3)
         (road n3 n21) (road n21 n3)
         (road n4 n13) (road n13 n4)
         (road n5 n7) (road n7 n5)
         (road n6 n8) (road n8 n6)
         (road n7 n19) (road n19 n7)
         (road n9 n26) (road n26 n9)
         (road n10 n19) (road n19 n10)
         (road n11 n13) (road n13 n11)
         (road n11 n14) (road n14 n11)
         (road n11 n22) (road n22 n11)
         (road n12 n16) (road n16 n12)
         (road n13 n18) (road n18 n13)
         (road n13 n21) (road n21 n13)
         (road n14 n17) (road n17 n14)
         (road n14 n19) (road n19 n14)
         (road n14 n24) (road n24 n14)
         (road n14 n26) (road n26 n14)
         (road n15 n16) (road n16 n15)
         (road n15 n26) (road n26 n15)
         (road n16 n17) (road n17 n16)
         (road n16 n21) (road n21 n16)
         (road n16 n23) (road n23 n16)
         (road n16 n26) (road n26 n16)
         (road n17 n20) (road n20 n17)
         (road n17 n22) (road n22 n17)
         (road n17 n23) (road n23 n17)
         (road n18 n25) (road n25 n18)
         (road n19 n23) (road n23 n19)
         (road n19 n24) (road n24 n19)
         (road n19 n25) (road n25 n19)
         (road n21 n25) (road n25 n21)
         (road n22 n25) (road n25 n22)
         (road n22 n26) (road n26 n22)
         (road n23 n24) (road n24 n23)
         (road n23 n25) (road n25 n23)
         (road n24 n26) (road n26 n24)
         (road n25 n26) (road n26 n25)
         (spare-in n1)
         (spare-in n2)
         (spare-in n3)
         (spare-in n8)
         (spare-in n11)
         (spare-in n12)
         (spare-in n13)
         (spare-in n15)
         (spare-in n17)
         (spare-in n20)
         (spare-in n22)
         (spare-in n24)
         (spare-in n26)
         (not-flattire)
  )
  (:goal (vehicle-at n19))
  (:metric minimize (total-cost))
)
