(define (problem random-problem51) 
 (:domain prob_domain) 
 
(:objects horzn0 horzn1 horzn2 - horizon-value)
(:init (horizon horzn2) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) 
(not-clear)
(WQ JK) (LS DC BZ) (AL GL GL) (LS CL YY) (HR FN) (WQ GC) (LS YV GC) (AL FN YY) (DR CL) (WQ BZ) (AL YY JK) (LS DC DC) (DR JK) (WQ GL) (WQ FN) (LS FN YV) (HR GC) (WQ XR) (AL XR YV) (AL GL YY)  
)
 (:goal (and 
(LS YY  GC ) 
(DR GC ) 
))
(:metric minimize (total-cost))
)
