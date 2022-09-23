(define (problem random-problem99) 
 (:domain prob_domain) 
 (:init 
(not-clear)
(OC NO) (VC QS) (TO NO) (VC IE) (TO MW) (OC RF) (QX MW TZ) (QX TZ RF) (QX TZ RU) (VC TZ) (OC KG) (OC MW) (GL IE) (GL NO) (OC OV) (OC TZ) (GL RU) (GL KG) (VC NO) (QX NO QS)  
)
 (:goal (and 
(GL TZ ) 
(VC KG ) 
(VC RU ) 
(VC VM ) 
))
(:metric minimize (total-cost))
)
