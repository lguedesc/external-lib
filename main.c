#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "odesolvers.h"

#ifdef _WIN32
    #include <windows.h>
    #define LIB_EXT ".dll"
#elif defined(__APPLE__) || defined(__MACH__)
    #include <dlfcn.h>
    #define LIB_EXT ".dylib"
#else
    #include <dlfcn.h>
    #define LIB_EXT ".so"
#endif

void error(void);
void write_results(FILE *output_file, int dim, double t, double *x, int mode);
double time_of_execution(int mode, clock_t *start, clock_t *end);

typedef void (*dynsys_t)(int, double *, double, double *, double *);

dynsys_t load_dynsys(const char *lib_path, const char* sys_name) {
    dynsys_t dynsys = NULL;

#ifdef _WIN32
    HINSTANCE hLib = LoadLibrary(lib_path);
    if(hLib != NULL) {
        dynsys = (dynsys_t) GetProcAddress(hLib, sys_name);
        if (dynsys == NULL) {
            printf("Error (a) loading function: %lu\n", GetLastError());
            FreeLibrary(hLib);
        }
    }
    else {
        printf("Error (b) loading function: %lu\n", GetLastError());
    }
#else
    void *handle = dlopen(lib_path, RTLD_LAZY);
    if (handle != NULL) {
        // Clear any existing error
        dlerror();
        dynsys = (dynsys_t) dlsym(handle, sys_name);
        char *error = dlerror();
        if (error != NULL) {
            printf("Error loading function: %s\n", error);
            dlclose(handle);
            dynsys = NULL;
        }
    }
    else {
        printf("Error loading library: %s\n", dlerror());
    }
#endif

    return dynsys;
}

int main(void) {
    // Define paths
    const char *lib_name = "duffing";
    const char *sys_name = "duffing";
    char lib_path[256];
    sprintf(lib_path, "%s%s", lib_name, LIB_EXT);
    printf("lib_path = %s\n", lib_path);
    // Load shared library
    printf("Loading Shared Library.\n");
    dynsys_t dynsys = load_dynsys(lib_path, sys_name);
    if (dynsys == NULL) {
        printf("Failed to load dynsys function\n");
        return 1;
    }
    // Start counting time of execution
    clock_t t_start, t_end;
    double exectime = time_of_execution(1, &t_start, &t_end);
    // Print start message
    printf("Program Started\n");
    // Program Parameters
    double tf = 500;
    double h = 0.001;
    // System Parameters
    double params[5] = { 1.6, 0.5, 0.025, -0.5, 0.5 };
    int dim = 2;
    double *x = malloc(dim* sizeof(x));
    // Initial Conditions
    double t0 = 0;
    x[0] = -1; x[1] = 0;
    // Create output files to store results, create header and print initial conditions
    FILE *out = fopen("output_rk4_dylib_linux.csv", "w");
    write_results(out, dim, t0, x, 1);
    // Call numerical integrator
    for (double t = t0 + h; t < tf + h; t = t + h) {
        rk4_new(dim, x, t, h, params, dynsys);
        // write results of the integration
        write_results(out, dim, t, x, 2);
    }
    // Close output file and free allocated memory
    fclose(out);
    free(x);
    // Stop counting time of execution and print end message
    exectime = time_of_execution(2, &t_start, &t_end);
    printf("Program ended successfully with %g seconds.\n", exectime);
    return 0;
}

double time_of_execution(int mode, clock_t *start, clock_t *end) {    
    double elapsed = 0;
    // Start measuring time
    if (mode == 1) {
        (*start) = clock();
    }
    else if (mode == 2) {
        // Stop measuring time and calculate the elapsed time
        (*end) = clock();
        elapsed = (double)((*end) - (*start))/CLOCKS_PER_SEC;
    }
    else {
        printf("DEBUG ERROR: Mode %d used to measure the time of execution.\nUse mode = 1 to start the time counting or mode = 2 to end time counting.\n", mode);
        EXIT_FAILURE;
    }
    return elapsed;
}

void write_results(FILE *output_file, int dim, double t, double *x, int mode) {
    // Check the mode of the function
    if (mode == 0) {
        fprintf(output_file, "Time ");
        for (int i = 0; i < dim; i++) {
            fprintf(output_file, "x[%i] ", i);
        }
        fprintf(output_file, "\n");
    }
    else if (mode == 1) {
        // Header
        fprintf(output_file, "Time ");
        for (int i = 0; i < dim; i++) {
            fprintf(output_file, "x[%i] ", i);
        }
        fprintf(output_file, "\n");
        // Initial Conditions
        fprintf(output_file, "%.10f ", t);
        for (int i = 0; i < dim; i++) {
            fprintf(output_file, "%.10lf ", x[i]);
        }
        fprintf(output_file, "\n");
    } 
    else if (mode == 2) {
        fprintf(output_file, "%.10f ", t);
        for (int i = 0; i < dim; i++) {
            fprintf(output_file, "%.10lf ", x[i]);
        }
        fprintf(output_file, "\n");
    }
    else {
        printf("Failed to write results in output file using mode (%d)...\n", mode);
        return;
    }
}
