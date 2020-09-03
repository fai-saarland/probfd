(define (problem tire_45_0_26433)
  (:domain tire)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 - horizon-value n0 n1 n2 n3 n4 n5 n6 n7 n8 n9 n10 n11 n12 n13 n14 n15 n16 n17 n18 n19 n20 n21 n22 n23 n24 n25 n26 n27 n28 n29 n30 n31 n32 n33 n34 n35 n36 n37 n38 n39 n40 n41 n42 n43 n44 - location)
  (:init (horizon horzn6) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (vehicle-at n23)
         (road n0 n32) (road n32 n0)
         (road n0 n36) (road n36 n0)
         (road n1 n40) (road n40 n1)
         (road n2 n39) (road n39 n2)
         (road n3 n25) (road n25 n3)
         (road n4 n37) (road n37 n4)
         (road n5 n7) (road n7 n5)
         (road n5 n14) (road n14 n5)
         (road n5 n43) (road n43 n5)
         (road n6 n24) (road n24 n6)
         (road n6 n31) (road n31 n6)
         (road n7 n19) (road n19 n7)
         (road n8 n12) (road n12 n8)
         (road n9 n16) (road n16 n9)
         (road n9 n23) (road n23 n9)
         (road n10 n36) (road n36 n10)
         (road n10 n41) (road n41 n10)
         (road n11 n21) (road n21 n11)
         (road n11 n33) (road n33 n11)
         (road n11 n35) (road n35 n11)
         (road n11 n42) (road n42 n11)
         (road n12 n22) (road n22 n12)
         (road n12 n41) (road n41 n12)
         (road n13 n40) (road n40 n13)
         (road n14 n24) (road n24 n14)
         (road n15 n33) (road n33 n15)
         (road n15 n40) (road n40 n15)
         (road n17 n20) (road n20 n17)
         (road n17 n30) (road n30 n17)
         (road n18 n21) (road n21 n18)
         (road n18 n42) (road n42 n18)
         (road n18 n43) (road n43 n18)
         (road n20 n29) (road n29 n20)
         (road n20 n40) (road n40 n20)
         (road n20 n42) (road n42 n20)
         (road n21 n32) (road n32 n21)
         (road n21 n38) (road n38 n21)
         (road n22 n42) (road n42 n22)
         (road n23 n35) (road n35 n23)
         (road n24 n28) (road n28 n24)
         (road n24 n36) (road n36 n24)
         (road n24 n41) (road n41 n24)
         (road n25 n27) (road n27 n25)
         (road n26 n28) (road n28 n26)
         (road n27 n36) (road n36 n27)
         (road n27 n37) (road n37 n27)
         (road n28 n32) (road n32 n28)
         (road n28 n41) (road n41 n28)
         (road n31 n40) (road n40 n31)
         (road n32 n34) (road n34 n32)
         (road n33 n35) (road n35 n33)
         (road n34 n37) (road n37 n34)
         (road n34 n42) (road n42 n34)
         (road n35 n42) (road n42 n35)
         (road n36 n41) (road n41 n36)
         (road n38 n44) (road n44 n38)
         (road n39 n40) (road n40 n39)
         (road n39 n42) (road n42 n39)
         (road n39 n43) (road n43 n39)
         (road n39 n44) (road n44 n39)
         (road n40 n41) (road n41 n40)
         (road n41 n43) (road n43 n41)
         (road n42 n43) (road n43 n42)
         (road n43 n44) (road n44 n43)
         (spare-in n2)
         (spare-in n5)
         (spare-in n8)
         (spare-in n9)
         (spare-in n11)
         (spare-in n14)
         (spare-in n19)
         (spare-in n20)
         (spare-in n26)
         (spare-in n27)
         (spare-in n28)
         (spare-in n30)
         (spare-in n31)
         (spare-in n32)
         (spare-in n34)
         (spare-in n35)
         (spare-in n36)
         (spare-in n43)
         (spare-in n44)
         (not-flattire)
  )
  (:goal (vehicle-at n39))
  (:metric minimize (total-cost))
)
