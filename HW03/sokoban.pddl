(define (domain sokoban)
(:requirements :strips)

(:predicates
    (wall ?x ?y)
    (box ?x ?y)
    (at ?x ?y)
    (inc ?p ?pp)
    (dec ?pp ?p)
)


(:action step-left
        :parameters (?x ?y ?xn)
        :precondition 
            (and (at ?x ?y)
                 (not (wall ?xn ?y))
                 (not (box ?xn ?y))
                 (dec ?x ?xn)
            )
        :effect
            (and (not (at ?x ?y))
                 (at ?xn ?y)
            )
    )

(:action step-right
        :parameters (?x ?y ?xn)
        :precondition 
            (and (at ?x ?y)
                 (not (wall ?xn ?y))
                 (not (box ?xn ?y))
                 (inc ?x ?xn)
            )
        :effect 
            (and (not (at ?x ?y))
                 (at ?xn ?y)
            )
    )

(:action step-down
        :parameters (?x ?y ?yn)
        :precondition 
            (and (at ?x ?y)
                 (not (wall ?x ?yn))
                 (not (box ?x ?yn))
                 (inc ?y ?yn)
                )
        :effect 
            (and (not (at ?x ?y)) 
                 (at ?x ?yn)
                 )
    )

(:action step-up
        :parameters (?x ?y ?yn)
        :precondition
            (and (at ?x ?y)
                 (not (wall ?x ?yn))
                 (not (box ?x ?yn))
                 (dec ?y ?yn)
            )
        :effect
            (and (not (at ?x ?y))
                 (at ?x ?yn)
            )
    )

(:action move-box-left
        :parameters (?x ?y ?xn ?xnn)
        :precondition
            (and (at ?x ?y)
                 (not (wall ?xn ?y)) 
                 (box ?xn ?y)
                 (dec ?x ?xn)
                 (not (wall ?xnn ?y))
                 (not (box ?xnn ?y))
                 (dec ?xn ?xnn)
            )
        :effect (and (not (at ?x ?y))
                     (at ?xn ?y)
                     (not (box ?xn ?y))
                     (box ?xnn ?y)
                )
    )

(:action move-box-right
        :parameters (?x ?y ?xn ?xnn)
        :precondition 
            (and (at ?x ?y)
                 (not (wall ?xn ?y))
                 (box ?xn ?y)
                 (inc ?x ?xn)
                 (not (wall ?xnn ?y))
                 (not (box ?xnn ?y))
                 (inc ?xn ?xnn)
            )
        :effect (and (not (at ?x ?y))
                     (at ?xn ?y)
                     (not (box ?xn ?y))
                     (box ?xnn ?y)
                )
    )

(:action move-box-down
        :parameters (?x ?y ?yn ?ynn)
        :precondition 
            (and (at ?x ?y)
                 (not (wall ?x ?yn))
                 (box ?x ?yn)
                 (inc ?y ?yn)
                 (not (wall ?x ?ynn))
                 (not (box ?x ?ynn))
                 (inc ?yn ?ynn)
            )
        :effect (and (not (at ?x ?y))
                     (at ?x ?yn)
                     (not (box ?x ?yn))
                     (box ?x ?ynn)
                )
    )
    
(:action move-box-up
        :parameters (?x ?y ?yn ?ynn)
        :precondition 
            (and (at ?x ?y)
                 (not (wall ?x ?yn))
                 (box ?x ?yn)
                 (dec ?y ?yn)
                 (not (wall ?x ?ynn))
                 (not (box ?x ?ynn))
                 (dec ?yn ?ynn)
            )
        :effect (and (not (at ?x ?y))
                     (at ?x ?yn)
                    (not (box ?x ?yn))
                    (box ?x ?ynn)
                )
    )
    
)