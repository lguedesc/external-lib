#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int number_of_parameters() {
    return 5;
}

int system_dimension() {
    return 2;
}

void duffing(int dim, double *x, double t, double *par, double *f) {
    // OMEGA = par[0]
    // gamma = par[1]
    // zeta = par[2]
    // alpha = par[3]
    // beta = par[4]
    
    if (dim == 2) {
        f[0] = x[1];
        f[1] = par[1]*sin(par[0] * t) - 2*par[2]*x[1] - par[3]*x[0] - par[4]*x[0]*x[0]*x[0];    
    } 
    else if (dim == 6) {
        f[0] = x[1];
        f[1] = par[1]*sin(par[0] * t) - 2*par[2]*x[1] - par[3]*x[0] - par[4]*x[0]*x[0]*x[0];
        for (int i = 0; i < 2; i++) {
            f[2 + i] = x[4 + i];
            f[4 + i] = -par[3]*x[2 + i] - 3*par[4]*x[0]*x[0]*x[2 + i] - 2*par[2]*x[4 + i];
        }
    }
    else {
        error();
    }    
}

void error(void) {
    printf("ERROR:\n");
    printf("Wrong dimension allocated for the system of equations.\n");
    printf("Please check the input file.\n");
    printf("Exiting Program...\n");
    exit(1);
}