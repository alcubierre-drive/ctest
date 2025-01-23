#pragma once

#include <stdio.h>
#include <string.h>
#ifndef CTEST_NO_FORK
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#endif

typedef struct {
    const char* name;
    void (*run)( unsigned* nassert, unsigned* npassed, unsigned* ncases,
            unsigned* ncases_run, char** filters );
} testsuite_t;

static const char ctest_color_red[] = "\x1b[31;1m",
                  ctest_color_green[] = "\x1b[32;1m",
                  ctest_color_yellow[] = "\x1b[33;1m",
                  ctest_color_reset[] = "\033[0m";

#ifndef CTEST_PRINTF
#define CTEST_PRINTF printf
#endif

#define GENERATE_SUITE_TYPES(SUITE) \
typedef struct SUITE##_testfunc_t SUITE##_testfunc_t; \
 \
struct SUITE##_testfunc_t { \
    SUITE##_testfunc_t* prev; \
    const char* name; \
    void (*fun)( void ); \
};

#define NEXT_CASE( SUITE, NAME, PREV ) \
    static void SUITE##_##NAME( void ); \
    static SUITE##_testfunc_t SUITE##_testfunc_##NAME = { &SUITE##_testfunc_##PREV, #NAME, &SUITE##_##NAME }; \
    static void SUITE##_##NAME( void )

#define INI_SUITE( SUITE, NAME ) \
    GENERATE_SUITE_TYPES(SUITE) \
    static unsigned __nproblems = 0; \
    static unsigned __nasserts = 0; \
    static SUITE##_testfunc_t SUITE##_testfunc___endtests = { NULL, "__endtests", NULL }; \
    NEXT_CASE( SUITE, NAME, __endtests )

#define END_SUITE( SUITE, PREV ) \
    NEXT_CASE( SUITE, __starttests, PREV ) { \
        CTEST_PRINTF( "%s=== ctest suite '" #SUITE "' ===%s\n", \
                ctest_color_yellow, ctest_color_reset ); \
    } \
    void SUITE( unsigned* nassert, unsigned* npassed, unsigned* ncases, \
            unsigned* ncases_run, char** filters ) { \
        SUITE##_testfunc_t* tf = &SUITE##_testfunc___starttests; \
        unsigned nc = 0, nc_run = 0; \
        for (; tf->prev != NULL; tf=tf->prev) { \
            if (strcmp(tf->name, "__starttests")) { \
                int matches_case_filters = 0; \
                int matches_suite_filters = 0; \
                int nfilters = 0; \
                char** f = filters; \
                while (*f) { \
                    nfilters++; \
                    if (!strcmp(tf->name, *f)) matches_case_filters++; \
                    if (!strcmp(#SUITE, *f)) matches_suite_filters++; \
                    f++; \
                } \
                if (matches_suite_filters || matches_case_filters || nfilters==0) { \
                    CTEST_PRINTF( "... run test '%s'\n", tf->name ); \
                    nc_run++; \
                    tf->fun(); \
                } \
                nc++; \
            } else { \
                tf->fun(); \
            } \
        } \
        *nassert = __nasserts; \
        *npassed = __nasserts-__nproblems; \
        *ncases = nc; \
        *ncases_run = nc_run; \
    } \
    testsuite_t register_##SUITE( void ) { \
        testsuite_t r; \
        r.name = #SUITE; \
        r.run = &SUITE; \
        return r; \
    }

#define CTEST_CHK( expr ) { \
    __nasserts++; \
    if (!(expr)) { \
        __nproblems++; \
        CTEST_PRINTF( "%s!!! FAILED in '%s' (%s:%i): " #expr "%s\n", ctest_color_red, \
                __func__, __FILE__, __LINE__, ctest_color_reset ); \
    } \
}

static inline int ctest_run_suites( testsuite_t* suites, unsigned nsuites, char** filters ) {
    unsigned npassed_total = 0, nassert_total = 0,
             ncases_total = 0, ncases_run_total = 0,
             ncrashed = 0;
    unsigned successes = 0;
    for (unsigned s=0; s<nsuites; ++s) {
        unsigned nums[4] = {0}; // nassert, npassed, ncases, ncases_run
        int retval = 0;

        #ifndef CTEST_NO_FORK
        int pipefd[2] = {0};

        if (pipe(pipefd) == -1) {
            CTEST_PRINTF("cannot setup pipe\n");
            exit(EXIT_FAILURE);
        }

        pid_t p = fork();
        if (p == 0) { // child
            suites[s].run(&nums[0], &nums[1], &nums[2], &nums[3], filters);
            close(pipefd[0]);
            write(pipefd[1], nums, sizeof nums);
            close(pipefd[1]);
            exit(EXIT_SUCCESS);
        } else {
            close(pipefd[1]);
            read(pipefd[0], nums, sizeof nums);
            close(pipefd[0]);
            waitpid( p, &retval, 0 );
        }
        #else // CTEST_NO_FORK
        suites[s].run(&nums[0], &nums[1], &nums[2], &nums[3], filters);
        #endif // CTEST_NO_FORK

        unsigned nassert = nums[0], npassed = nums[1],
                 ncases = nums[2], ncases_run = nums[3];

        nassert_total += nassert;
        npassed_total += npassed;
        ncases_total += ncases;
        ncases_run_total += ncases_run;
        unsigned suite_success = (npassed == nassert) && !retval;
        if (retval) {
            ncrashed++;
            CTEST_PRINTF( "%s=== CRASHED:%i ctest suite '%s' ===%s\n", ctest_color_red,
                retval, suites[s].name, ctest_color_reset );
        } else {
            if (ncases_run > 0) {
                CTEST_PRINTF( "%s=== passed %u/%u assertions in %u/%u cases of suite '%s' ===%s\n",
                    suite_success ? ctest_color_green : ctest_color_red, npassed,
                    nassert, ncases_run, ncases, suites[s].name, ctest_color_reset );
            } else {
                CTEST_PRINTF( "%s=== filtered out suite '%s'===%s\n", ctest_color_yellow,
                    suites[s].name, ctest_color_reset );
            }
        }
        successes += suite_success;
    }

    CTEST_PRINTF( "%s=== ctest report ===%s\n", ctest_color_yellow, ctest_color_reset );
    CTEST_PRINTF( "%s=== crashed %u/%u suites with %u/%u failed assertions in %u/%u cases ===%s\n",
        successes == nsuites ? ctest_color_green : ctest_color_red,
        ncrashed, nsuites, nassert_total - npassed_total, nassert_total,
        ncases_run_total, ncases_total, ctest_color_reset );

    return !(successes == nsuites);
}
