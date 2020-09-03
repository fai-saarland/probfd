(define (problem tire_35_0_435)
  (:domain tire)
  (:objects horzn0 horzn1 - horizon-value n0 n1 n2 n3 n4 n5 n6 n7 n8 n9 n10 n11 n12 n13 n14 n15 n16 n17 n18 n19 n20 n21 n22 n23 n24 n25 n26 n27 n28 n29 n30 n31 n32 n33 n34 - location)
  (:init (horizon horzn1) (horizon-decrement horzn1 horzn0) (vehicle-at n28)
         (road n0 n2) (road n2 n0)
         (road n0 n17) (road n17 n0)
         (road n0 n22) (road n22 n0)
         (road n0 n25) (road n25 n0)
         (road n1 n2) (road n2 n1)
         (road n1 n7) (road n7 n1)
         (road n1 n10) (road n10 n1)
         (road n1 n11) (road n11 n1)
         (road n2 n14) (road n14 n2)
         (road n2 n18) (road n18 n2)
         (road n2 n22) (road n22 n2)
         (road n2 n27) (road n27 n2)
         (road n2 n31) (road n31 n2)
         (road n3 n13) (road n13 n3)
         (road n3 n16) (road n16 n3)
         (road n3 n21) (road n21 n3)
         (road n3 n24) (road n24 n3)
         (road n4 n34) (road n34 n4)
         (road n5 n14) (road n14 n5)
         (road n5 n25) (road n25 n5)
         (road n6 n8) (road n8 n6)
         (road n6 n27) (road n27 n6)
         (road n6 n29) (road n29 n6)
         (road n6 n34) (road n34 n6)
         (road n8 n12) (road n12 n8)
         (road n8 n14) (road n14 n8)
         (road n8 n23) (road n23 n8)
         (road n8 n28) (road n28 n8)
         (road n9 n21) (road n21 n9)
         (road n10 n12) (road n12 n10)
         (road n10 n15) (road n15 n10)
         (road n10 n19) (road n19 n10)
         (road n11 n19) (road n19 n11)
         (road n12 n23) (road n23 n12)
         (road n12 n32) (road n32 n12)
         (road n13 n22) (road n22 n13)
         (road n13 n29) (road n29 n13)
         (road n13 n31) (road n31 n13)
         (road n14 n26) (road n26 n14)
         (road n14 n31) (road n31 n14)
         (road n15 n24) (road n24 n15)
         (road n15 n29) (road n29 n15)
         (road n16 n20) (road n20 n16)
         (road n17 n33) (road n33 n17)
         (road n17 n34) (road n34 n17)
         (road n18 n20) (road n20 n18)
         (road n18 n21) (road n21 n18)
         (road n18 n25) (road n25 n18)
         (road n18 n26) (road n26 n18)
         (road n18 n29) (road n29 n18)
         (road n18 n32) (road n32 n18)
         (road n18 n34) (road n34 n18)
         (road n19 n22) (road n22 n19)
         (road n19 n24) (road n24 n19)
         (road n19 n27) (road n27 n19)
         (road n20 n23) (road n23 n20)
         (road n20 n27) (road n27 n20)
         (road n21 n26) (road n26 n21)
         (road n21 n31) (road n31 n21)
         (road n21 n33) (road n33 n21)
         (road n22 n28) (road n28 n22)
         (road n22 n29) (road n29 n22)
         (road n22 n30) (road n30 n22)
         (road n22 n31) (road n31 n22)
         (road n22 n33) (road n33 n22)
         (road n23 n27) (road n27 n23)
         (road n24 n25) (road n25 n24)
         (road n24 n27) (road n27 n24)
         (road n24 n29) (road n29 n24)
         (road n24 n30) (road n30 n24)
         (road n24 n34) (road n34 n24)
         (road n25 n29) (road n29 n25)
         (road n25 n30) (road n30 n25)
         (road n26 n29) (road n29 n26)
         (road n27 n28) (road n28 n27)
         (road n27 n29) (road n29 n27)
         (road n27 n31) (road n31 n27)
         (road n27 n34) (road n34 n27)
         (road n28 n29) (road n29 n28)
         (road n28 n34) (road n34 n28)
         (road n30 n31) (road n31 n30)
         (road n30 n32) (road n32 n30)
         (road n30 n33) (road n33 n30)
         (road n31 n32) (road n32 n31)
         (road n31 n33) (road n33 n31)
         (road n31 n34) (road n34 n31)
         (road n32 n33) (road n33 n32)
         (road n33 n34) (road n34 n33)
         (spare-in n0)
         (spare-in n5)
         (spare-in n8)
         (spare-in n9)
         (spare-in n11)
         (spare-in n12)
         (spare-in n14)
         (spare-in n16)
         (spare-in n19)
         (spare-in n20)
         (spare-in n22)
         (spare-in n23)
         (spare-in n24)
         (spare-in n27)
         (spare-in n29)
         (spare-in n30)
         (spare-in n31)
         (not-flattire)
  )
  (:goal (vehicle-at n22))
  (:metric minimize (total-cost))
)
