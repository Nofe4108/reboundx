#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rebound.h"
#include "reboundx.h"

void rebx_mass_loss_test(struct reb_simulation* const sim, struct rebx_operator* const operator, const double dt){
    struct reb_particle* const p = &sim->particles[0];
    if (p->m > .01){
    p->m += + (-.00001);
    }
}
