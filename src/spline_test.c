//
//  spline_test.c
//  
//
//  Created by Noah Ferich on 1/30/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rebound.h"
#include "reboundx.h"

void rebx_spline_test(struct reb_simulation* const sim, struct rebx_operator* const operator, const double dt){
rebx_get_param(sim->extras, p->ap, "spline_file");
