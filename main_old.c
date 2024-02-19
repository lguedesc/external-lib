#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "odesolvers.h"
#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

void error(void);
void (*dynsys)(int dim, double *x, double t, double *par, double *f);
void write_results(FILE *output_file, int dim, double t, double *x, int mode);
double time_of_execution(int mode, clock_t *start, clock_t *end);

#ifdef _WIN32
    void (*load_dynsys(const char *lib_name, const char *func_name, HINSTANCE **lib))(int, double *, double, double *, double *) {
        // Load the DLL
        *lib = LoadLibrary(lib_name);
        if (!*lib) {
            // Handle error
            fprintf(stderr, "Error: %lu\n", GetLastError());
            exit(EXIT_FAILURE);
        }
        // Get a pointer to the function in the DLL
        void (*dynsys)(int, double *, double, double *, double *) = (void (*)(int, double *, double, double *, double *))GetProcAddress(*lib, func_name);
        if (!dynsys) {
            // Handle error
            fprintf(stderr, "Error: %lu\n", GetLastError());
            exit(EXIT_FAILURE);
        }

        return dynsys;
    }

    void close_dylib(HINSTANCE *lib) {
        FreeLibrary(*lib);
    }

#else
    void (*load_dynsys_old(const char* lib_name, const char* func_name, void *lib))(int, double *, double, double *, double *) {
        // Open dynamic library
        lib = dlopen(lib_name, RTLD_LAZY);
        if (!lib) {
            // Handle error
            fprintf(stderr, "%s\n", dlerror());
            exit(EXIT_FAILURE);
        }
        // Clear any existing error
        dlerror();
        // Get a pointer to the function in the shared library
        *(void **) (&dynsys) = dlsym(lib, func_name);
        const char *dlsym_error = dlerror();
        if (dlsym_error) {
            // Handle error
            fprintf(stderr, "%s\n", dlsym_error);
            exit(EXIT_FAILURE);
        }

        return dynsys;
    }

     void (*load_dynsys(const char* lib_name, const char* func_name, void **lib))(int, double *, double, double *, double *) {
        // Open dynamic library
        *lib = dlopen(lib_name, RTLD_LAZY);
        if (!*lib) {
            // Handle error
            fprintf(stderr, "%s\n", dlerror());
            exit(EXIT_FAILURE);
        }
        // Clear any existing error
        dlerror();
        // Get a pointer to the function in the shared library
        void (*dynsys)(int, double *, double, double *, double *);
        *(void **) (&dynsys) = dlsym(*lib, func_name);
        const char *dlsym_error = dlerror();
        if (dlsym_error) {
            // Handle error
            fprintf(stderr, "%s\n", dlsym_error);
            exit(EXIT_FAILURE);
        }

        return dynsys;
    }

    void close_dylib(void *lib) {
        dlclose(lib);
    }

#endif

int main(void) {
    // Load shared library
    printf("Loading Shared Library.\n");
    void *dylib = NULL;
    dynsys = load_dynsys("duffing.dylib", "duffing", &dylib);
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
    FILE *out = fopen("output_rk4_dylib.csv", "w");
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
    close_dylib(dylib);
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
