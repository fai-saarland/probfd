(define (problem a-drive-problem597)
(:domain drive)
(:objects c0 c1 c2 c3 c4 c5  - coord)
(:init 
       (heading east)
       (at c0 c0)
       (alive)
       (light_color unknown)
       (clockwise north east)
       (clockwise east south)
       (clockwise south west)
       (clockwise west north)
       (nextx c0 c1 east)
       (nextx c1 c0 west)
       (nextx c1 c2 east)
       (nextx c2 c1 west)
       (nextx c2 c3 east)
       (nextx c3 c2 west)
       (nextx c3 c4 east)
       (nextx c4 c3 west)
       (nextx c0 c0 north)
       (nextx c0 c0 south)
       (nextx c1 c1 north)
       (nextx c1 c1 south)
       (nextx c2 c2 north)
       (nextx c2 c2 south)
       (nextx c3 c3 north)
       (nextx c3 c3 south)
       (nextx c4 c4 north)
       (nextx c4 c4 south)
       (nexty c0 c1 north)
       (nexty c1 c0 south)
       (nexty c1 c2 north)
       (nexty c2 c1 south)
       (nexty c2 c3 north)
       (nexty c3 c2 south)
       (nexty c3 c4 north)
       (nexty c4 c3 south)
       (nexty c4 c5 north)
       (nexty c5 c4 south)
       (nexty c0 c0 east)
       (nexty c0 c0 west)
       (nexty c1 c1 east)
       (nexty c1 c1 west)
       (nexty c2 c2 east)
       (nexty c2 c2 west)
       (nexty c3 c3 east)
       (nexty c3 c3 west)
       (nexty c4 c4 east)
       (nexty c4 c4 west)
       (nexty c5 c5 east)
       (nexty c5 c5 west)
       (light_delay c0 c0 quick)
       (light_delay c0 c1 quick)
       (light_delay c0 c2 normal)
       (light_delay c0 c3 slow)
       (light_delay c0 c4 normal)
       (light_delay c0 c5 slow)
       (light_delay c1 c0 quick)
       (light_delay c1 c1 quick)
       (light_delay c1 c2 normal)
       (light_delay c1 c3 slow)
       (light_delay c1 c4 slow)
       (light_delay c1 c5 quick)
       (light_delay c2 c0 quick)
       (light_delay c2 c1 quick)
       (light_delay c2 c2 quick)
       (light_delay c2 c3 slow)
       (light_delay c2 c4 quick)
       (light_delay c2 c5 normal)
       (light_delay c3 c0 quick)
       (light_delay c3 c1 quick)
       (light_delay c3 c2 slow)
       (light_delay c3 c3 slow)
       (light_delay c3 c4 normal)
       (light_delay c3 c5 quick)
       (light_delay c4 c0 slow)
       (light_delay c4 c1 quick)
       (light_delay c4 c2 slow)
       (light_delay c4 c3 quick)
       (light_delay c4 c4 quick)
       (light_delay c4 c5 normal)
       (light_preference c0 c0 north_south)
       (light_preference c0 c1 east_west)
       (light_preference c0 c2 none)
       (light_preference c0 c3 none)
       (light_preference c0 c4 north_south)
       (light_preference c0 c5 none)
       (light_preference c1 c0 north_south)
       (light_preference c1 c1 east_west)
       (light_preference c1 c2 north_south)
       (light_preference c1 c3 north_south)
       (light_preference c1 c4 none)
       (light_preference c1 c5 north_south)
       (light_preference c2 c0 none)
       (light_preference c2 c1 north_south)
       (light_preference c2 c2 north_south)
       (light_preference c2 c3 none)
       (light_preference c2 c4 none)
       (light_preference c2 c5 east_west)
       (light_preference c3 c0 north_south)
       (light_preference c3 c1 north_south)
       (light_preference c3 c2 none)
       (light_preference c3 c3 north_south)
       (light_preference c3 c4 north_south)
       (light_preference c3 c5 east_west)
       (light_preference c4 c0 east_west)
       (light_preference c4 c1 north_south)
       (light_preference c4 c2 none)
       (light_preference c4 c3 none)
       (light_preference c4 c4 none)
       (light_preference c4 c5 none)
       (road-length c0 c0 c0 c1 short)
       (road-length c0 c1 c0 c0 short)
       (road-length c0 c1 c0 c2 short)
       (road-length c0 c2 c0 c1 short)
       (road-length c0 c2 c0 c3 short)
       (road-length c0 c3 c0 c2 short)
       (road-length c0 c3 c0 c4 long)
       (road-length c0 c4 c0 c3 long)
       (road-length c0 c4 c0 c5 medium)
       (road-length c0 c5 c0 c4 medium)
       (road-length c1 c0 c1 c1 long)
       (road-length c1 c1 c1 c0 long)
       (road-length c1 c1 c1 c2 long)
       (road-length c1 c2 c1 c1 long)
       (road-length c1 c2 c1 c3 medium)
       (road-length c1 c3 c1 c2 medium)
       (road-length c1 c3 c1 c4 medium)
       (road-length c1 c4 c1 c3 medium)
       (road-length c1 c4 c1 c5 long)
       (road-length c1 c5 c1 c4 long)
       (road-length c2 c0 c2 c1 medium)
       (road-length c2 c1 c2 c0 medium)
       (road-length c2 c1 c2 c2 short)
       (road-length c2 c2 c2 c1 short)
       (road-length c2 c2 c2 c3 long)
       (road-length c2 c3 c2 c2 long)
       (road-length c2 c3 c2 c4 long)
       (road-length c2 c4 c2 c3 long)
       (road-length c2 c4 c2 c5 long)
       (road-length c2 c5 c2 c4 long)
       (road-length c3 c0 c3 c1 short)
       (road-length c3 c1 c3 c0 short)
       (road-length c3 c1 c3 c2 medium)
       (road-length c3 c2 c3 c1 medium)
       (road-length c3 c2 c3 c3 medium)
       (road-length c3 c3 c3 c2 medium)
       (road-length c3 c3 c3 c4 medium)
       (road-length c3 c4 c3 c3 medium)
       (road-length c3 c4 c3 c5 long)
       (road-length c3 c5 c3 c4 long)
       (road-length c4 c0 c4 c1 short)
       (road-length c4 c1 c4 c0 short)
       (road-length c4 c1 c4 c2 medium)
       (road-length c4 c2 c4 c1 medium)
       (road-length c4 c2 c4 c3 long)
       (road-length c4 c3 c4 c2 long)
       (road-length c4 c3 c4 c4 medium)
       (road-length c4 c4 c4 c3 medium)
       (road-length c4 c4 c4 c5 medium)
       (road-length c4 c5 c4 c4 medium)
       (road-length c0 c0 c1 c0 medium)
       (road-length c1 c0 c0 c0 medium)
       (road-length c1 c0 c2 c0 short)
       (road-length c2 c0 c1 c0 short)
       (road-length c2 c0 c3 c0 medium)
       (road-length c3 c0 c2 c0 medium)
       (road-length c3 c0 c4 c0 medium)
       (road-length c4 c0 c3 c0 medium)
       (road-length c0 c1 c1 c1 short)
       (road-length c1 c1 c0 c1 short)
       (road-length c1 c1 c2 c1 short)
       (road-length c2 c1 c1 c1 short)
       (road-length c2 c1 c3 c1 long)
       (road-length c3 c1 c2 c1 long)
       (road-length c3 c1 c4 c1 medium)
       (road-length c4 c1 c3 c1 medium)
       (road-length c0 c2 c1 c2 long)
       (road-length c1 c2 c0 c2 long)
       (road-length c1 c2 c2 c2 long)
       (road-length c2 c2 c1 c2 long)
       (road-length c2 c2 c3 c2 short)
       (road-length c3 c2 c2 c2 short)
       (road-length c3 c2 c4 c2 long)
       (road-length c4 c2 c3 c2 long)
       (road-length c0 c3 c1 c3 medium)
       (road-length c1 c3 c0 c3 medium)
       (road-length c1 c3 c2 c3 short)
       (road-length c2 c3 c1 c3 short)
       (road-length c2 c3 c3 c3 medium)
       (road-length c3 c3 c2 c3 medium)
       (road-length c3 c3 c4 c3 medium)
       (road-length c4 c3 c3 c3 medium)
       (road-length c0 c4 c1 c4 short)
       (road-length c1 c4 c0 c4 short)
       (road-length c1 c4 c2 c4 short)
       (road-length c2 c4 c1 c4 short)
       (road-length c2 c4 c3 c4 long)
       (road-length c3 c4 c2 c4 long)
       (road-length c3 c4 c4 c4 long)
       (road-length c4 c4 c3 c4 long)
       (road-length c0 c5 c1 c5 medium)
       (road-length c1 c5 c0 c5 medium)
       (road-length c1 c5 c2 c5 short)
       (road-length c2 c5 c1 c5 short)
       (road-length c2 c5 c3 c5 long)
       (road-length c3 c5 c2 c5 long)
       (road-length c3 c5 c4 c5 medium)
       (road-length c4 c5 c3 c5 medium)
)
(:goal (and (alive) (at c4 c5)))
(:metric minimize (total-cost))
)
