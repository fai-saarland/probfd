(define (problem tire_29_0_3597)
  (:domain tire)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 - horizon-value n0 n1 n2 n3 n4 n5 n6 n7 n8 n9 n10 n11 n12 n13 n14 n15 n16 n17 n18 n19 n20 n21 n22 n23 n24 n25 n26 n27 n28 - location)
  (:init (horizon horzn4) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (vehicle-at n10)
         (road n0 n12) (road n12 n0)
         (road n1 n25) (road n25 n1)
         (road n1 n27) (road n27 n1)
         (road n2 n6) (road n6 n2)
         (road n2 n18) (road n18 n2)
         (road n3 n24) (road n24 n3)
         (road n4 n19) (road n19 n4)
         (road n5 n17) (road n17 n5)
         (road n5 n21) (road n21 n5)
         (road n5 n26) (road n26 n5)
         (road n6 n14) (road n14 n6)
         (road n6 n19) (road n19 n6)
         (road n6 n20) (road n20 n6)
         (road n6 n25) (road n25 n6)
         (road n7 n9) (road n9 n7)
         (road n8 n9) (road n9 n8)
         (road n8 n15) (road n15 n8)
         (road n9 n13) (road n13 n9)
         (road n9 n17) (road n17 n9)
         (road n10 n15) (road n15 n10)
         (road n11 n16) (road n16 n11)
         (road n11 n26) (road n26 n11)
         (road n11 n28) (road n28 n11)
         (road n12 n22) (road n22 n12)
         (road n13 n14) (road n14 n13)
         (road n14 n19) (road n19 n14)
         (road n14 n23) (road n23 n14)
         (road n14 n25) (road n25 n14)
         (road n15 n16) (road n16 n15)
         (road n15 n27) (road n27 n15)
         (road n16 n21) (road n21 n16)
         (road n16 n24) (road n24 n16)
         (road n16 n25) (road n25 n16)
         (road n17 n25) (road n25 n17)
         (road n18 n22) (road n22 n18)
         (road n18 n24) (road n24 n18)
         (road n18 n25) (road n25 n18)
         (road n19 n21) (road n21 n19)
         (road n19 n24) (road n24 n19)
         (road n19 n27) (road n27 n19)
         (road n20 n22) (road n22 n20)
         (road n20 n23) (road n23 n20)
         (road n20 n25) (road n25 n20)
         (road n21 n27) (road n27 n21)
         (road n22 n28) (road n28 n22)
         (road n23 n24) (road n24 n23)
         (road n23 n28) (road n28 n23)
         (road n24 n27) (road n27 n24)
         (road n25 n27) (road n27 n25)
         (road n25 n28) (road n28 n25)
         (road n26 n27) (road n27 n26)
         (road n26 n28) (road n28 n26)
         (road n27 n28) (road n28 n27)
         (spare-in n1)
         (spare-in n2)
         (spare-in n5)
         (spare-in n6)
         (spare-in n7)
         (spare-in n9)
         (spare-in n10)
         (spare-in n13)
         (spare-in n14)
         (spare-in n16)
         (spare-in n17)
         (spare-in n19)
         (spare-in n20)
         (spare-in n21)
         (spare-in n24)
         (spare-in n28)
         (not-flattire)
  )
  (:goal (vehicle-at n24))
  (:metric minimize (total-cost))
)
