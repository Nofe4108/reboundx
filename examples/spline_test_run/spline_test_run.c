//
//  spline_test_run.c
//  
//
//  Created by Noah Ferich on 1/30/20.
//

#include "spline_test_run.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rebound.h"
#include "reboundx.h"

char data_file[1000] = "test_data.txt";
char *data_pointer;
data_pointer = &data_file

int main(int argc, char* argv[]){
struct reb_simulation* sim = reb_create_simulation();
    
    struct rebx_extras* rebx = rebx_attach(sim); // initialize reboundx
    struct rebx_operator* modify_mass = rebx_load_operator(rebx, "spline_test");
    rebx_add_operator(rebx, spline_test);
    
    
    
}
