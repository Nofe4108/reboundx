
/**
 * Sun-Earth Output
 *
 * This example integrates a simple Sun-Earth system with the Reboundx mass_lost_test operator
 * Heartbeat function outputs Earth orbital data for
 * later import and and graphical analysis in Python.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "rebound.h"
#include "reboundx.h"

void heartbeat(struct reb_simulation* r);
double tmax;

int main(int argc, char* argv[]){
    struct reb_simulation* r = reb_create_simulation();
    // Setup constants
    r->dt = 1./20.;              // 1/20 Earth's period in yrs
    tmax = 4000;                  // 4 kyr
    r->G = 39.476926421373;        // in AU^3 / Msun / yr^2.
    r->ri_whfast.safe_mode = 0;  // Turn off safe mode. Need to call reb_integrator_synchronize() before outputs.
    r->ri_whfast.corrector = 11; // 11th order symplectic corrector
    r->integrator = REB_INTEGRATOR_WHFAST;
    r->heartbeat = heartbeat;
    r->exact_finish_time = 1;    // Finish exactly at tmax in reb_integrate(). Default is already 1.

    // Add Sun to sim
    struct reb_particle sun = {0}; // initialize w/ zeroes
    sun.m = 1;                     // in Msun
    reb_add(r, sun);
    // Add Earth to sim
    struct reb_orbit eo = {0};
    double e_mass = .000002988;
    eo.a = 1.0;                    // in AU
    struct reb_particle ep = reb_tools_orbit_to_particle(r->G, sun, e_mass, eo.a, eo.e, eo.inc, eo.Omega, eo.omega, eo.f);
    reb_add(r, ep);

    // Overwrite planet output file w/ col headers
    system("rm -f planet.txt"); // remove existing file
    FILE* file = fopen("planet.txt","a");
    fprintf(file, "Time(yrs)\t\tMass(Msun)\t\tSemi-major Axis(AU)\t\tEccentricity\t\tInclination(Radians)\t\tLongitude_of_Ascending_Node(Radians)\t\tArgument_of_Periapsis(Radians))\t\tTrue_Anomaly(Radians)\n");
    fclose(file);

    struct rebx_extras* rebx = rebx_attach(r);  // first initialize rebx
    struct rebx_operator* mass_loss_test = rebx_load_operator(rebx, "mass_loss_test"); // adds the operator
    rebx_add_operator(rebx, mass_loss_test);
    
    
    /*struct rebx_extras* rebx = rebx_attach(r); // initialize reboundx
    struct rebx_operator* modify_mass = rebx_load_operator(rebx, "modify_mass");
    rebx_add_operator(rebx, modify_mass);
    rebx_set_param_double(rebx, &r->particles[0].ap, "tau_mass", -1);
    double tau_mass = r->particles[1].m;*/

    /* We can approximate a linear mass loss/growth rate if the rate is small by taking tau_mass = M_initial / mass_loss_rate (or growth)
    double M_dot = 100000000; // mass growth rate for the planet (in simulation units--here Msun/yr)
    double tau_mass = r->particles[1].m / M_dot; // first planet gains mass at linear rate M_dot
    rebx_set_param_double(rebx, &r->particles[0].ap, "tau_mass", tau_mass);*/
    
    
    
    // Run simulation
    reb_move_to_com(r);
    reb_integrate(r, tmax);
    rebx_free(rebx);
}

void heartbeat(struct reb_simulation* r){
    if (reb_output_check(r, 1000.)){
        // retrieve Sun particle
       struct reb_particle sun = r->particles[0];
        //printf("%f\n",sun.m);
        printf("%f\n", r->particles[0].m);
        // retrieve Earth particle
        struct reb_particle ep = r->particles[1];
        struct reb_orbit eo  = reb_tools_particle_to_orbit(r->G, ep, sun);
        double t = r->t;
        double m = ep.m;
        double a = eo.a;
        double e = eo.e;
        double inc = eo.inc;
        double Omega = eo.Omega;
        double omega = eo.omega;
        double f = eo.f;
        FILE* file = fopen("planet.txt","a");

        reb_output_timing(r, tmax);
        reb_integrator_synchronize(r);
        fprintf(file,"%e\t\t%e\t\t%e\t\t%e\t\t%e\t\t%e\t\t%e\t\t%e\n",t,m,a,e,inc,Omega,omega,f);
        fclose(file);
    }
}

