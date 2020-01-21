#include "rebound.h"
#include <stdio.h>
#include <stdlib.h>
#include "reboundx.h"


int main(int argc, char* argv[]) {
        struct reb_simulation* sim = reb_create_simulation();
        sim->dt = 0.1;
        sim->integrator = REB_INTEGRATOR_WHFAST;

        struct reb_particle p1 = {0};
        p1.m = 1.;
        reb_add(sim, p1);

        struct reb_particle p2 = {0};
        p2.x = 1;
        p2.vy = 1;
        p2.m = .01;
        reb_add(sim, p2);

        struct rebx_extras* rebx = rebx_attach(sim);  // first initialize rebx
        struct rebx_operator* mass_loss_test = rebx_load_operator(rebx, "mass_loss_test"); // add our new force
        rebx_add_operator(rebx, mass_loss_test);
        reb_move_to_com(sim);
        reb_integrate(sim,10000.);
        rebx_free(rebx);
        
        printf("%f\n",sim->particles[0].m);
        printf("%f\n", sim->t);
        printf("%f", p2.x);
        
}

