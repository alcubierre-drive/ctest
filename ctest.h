#pragma once

#include <stdio.h>
#include <string.h>
#ifndef CT_NO_FORK
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#endif

#ifdef __cplusplus
#define CT_EXTERN extern "C"
#define CT_VISIBLE extern "C"
#else
#define CT_EXTERN extern
#define CT_VISIBLE 
#endif

#ifndef CT_PRINTF
#define CT_PRINTF printf
#endif

typedef struct ct_test_t ct_test_t;
struct ct_test_t {
    void (*run)( void );
    const char* name;
};

#ifndef CT_SUITE_MAX_FUNCS
#define CT_SUITE_MAX_FUNCS 128
#endif//CT_SUITE_MAX_FUNCS
#ifndef CT_SUITE_NAME_NCHAR
#define CT_SUITE_NAME_NCHAR 256
#endif//CT_SUITE_NAME_NCHAR

typedef struct ct_suite_t ct_suite_t;
struct ct_suite_t {
    ct_test_t tests[CT_SUITE_MAX_FUNCS];
    unsigned ntests;
    unsigned nasserts;
    unsigned nproblems;
    char name[CT_SUITE_NAME_NCHAR];
};

// this file's suite
#ifdef __cplusplus
static ct_suite_t ct_suite {};
#else
static ct_suite_t ct_suite = {0};
#endif

// all suites
#ifndef CT_MAX_SUITES
#define CT_MAX_SUITES 128
#endif//CT_MAX_SUITES
#ifndef CT_MAIN
CT_EXTERN ct_suite_t* ct_suites[CT_MAX_SUITES];
CT_EXTERN unsigned ct_nsuites;
#else
CT_VISIBLE ct_suite_t* ct_suites[CT_MAX_SUITES] = {0};
CT_VISIBLE unsigned ct_nsuites = 0;
#endif

// stupid initializer code
#ifdef __cplusplus
    #define INITIALIZER(f) \
        static void f(void); \
        struct f##_t_ { f##_t_(void) { f(); } }; static f##_t_ f##_; \
        static void f(void)
#elif defined(_MSC_VER)
    #pragma section(".CRT$XCU",read)
    #define INITIALIZER2_(f,p) \
        static void f(void); \
        __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
        __pragma(comment(linker,"/include:" p #f "_")) \
        static void f(void)
    #ifdef _WIN64
        #define INITIALIZER(f) INITIALIZER2_(f,"")
    #else
        #define INITIALIZER(f) INITIALIZER2_(f,"_")
    #endif
#else
    #define INITIALIZER(f) \
        static void f(void) __attribute__((constructor)); \
        static void f(void)
#endif

#ifndef CT_SUITE_NAME
#define CT_SUITE_NAME __FILE__
#endif

// append this file's suite to the list of all suites
INITIALIZER( ct_suite_append ) {
    if (ct_nsuites < CT_MAX_SUITES) {
        ct_suites[ct_nsuites++] = &ct_suite;
        strncpy( ct_suite.name, CT_SUITE_NAME, CT_SUITE_NAME_NCHAR-1 );
    } else {
        CT_PRINTF( "too many test suites (%u), redefine CT_MAX_SUITES\n", ct_nsuites+1 );
    }
}

static inline void ct_test_append( const char* name, void (*run)( void ) ) {
    ct_test_t t = { run, name };
    if (ct_suite.ntests < CT_SUITE_MAX_FUNCS)
        ct_suite.tests[ct_suite.ntests++] = t;
    else
        CT_PRINTF( "too many tests (%u) in suite %s, redefine CT_SUITE_MAX_FUNCS\n",
                 ct_suite.ntests+1, CT_SUITE_NAME );
}
// this is the macro to define test functions
#define CT_TEST( name ) \
    static void ct__test__##name( void ); \
    INITIALIZER( ct__test__register_##name ) { \
        ct_test_append( #name, ct__test__##name ); \
    } \
    static void ct__test__##name( void )

// we "need" colors
static const char ct_color_red[] = "\x1b[31;1m",
                  ct_color_green[] = "\x1b[32;1m",
                  ct_color_yellow[] = "\x1b[33;1m",
                  ct_color_reset[] = "\033[0m";

// and this is the macro to make assertions
#define CT_CHECK( expr ) { \
    ct_suite.nasserts++; \
    if (!(expr)) { \
        ct_suite.nproblems++; \
        CT_PRINTF( "%s!!! %s  (%s:%i) failed at CT_CHECK(" #expr ") !!!%s\n", ct_color_red, \
                __func__, __FILE__, __LINE__, ct_color_reset ); \
    } \
}

static void ct_suite_run( ct_suite_t* s, unsigned* nassert,
        unsigned* npassed, unsigned* ncases, unsigned* ncases_run,
        char** filters ) {
    if (s->ntests == 0) return;

    CT_PRINTF( "%s=== ct suite '%s' ===%s\n", ct_color_yellow, s->name, ct_color_reset );

    unsigned nc = 0, nc_run = 0;

    for (unsigned i=0; i<s->ntests; ++i) {
        int matches_case_filters = 0;
        int matches_suite_filters = 0;
        int nfilters = 0;
        char** f = filters;
        while (*f) {
            nfilters++;
            if (!strcmp(s->tests[i].name, *f)) matches_case_filters++;
            if (!strcmp(s->name, *f)) matches_suite_filters++;
            f++;
        }
        if (matches_suite_filters || matches_case_filters || nfilters==0) {
            CT_PRINTF( "... run test '%s'\n", s->tests[i].name );
            nc_run++;
            s->tests[i].run();
        }
        nc++;
    }
    *nassert = s->nasserts;
    *npassed = s->nasserts-s->nproblems;
    s->nasserts = s->nproblems = 0; // reset
    *ncases = nc;
    *ncases_run = nc_run;
}

static inline int ct_run( char** filters ) {
    unsigned npassed_total = 0, nassert_total = 0,
             ncases_total = 0, ncases_run_total = 0,
             ncrashed = 0;
    unsigned successes = 0;
    unsigned ct_nsuites_nz = 0;
    for (unsigned s=0; s<ct_nsuites; ++s) {
        if (ct_suites[s]->ntests == 0) continue;
        ct_nsuites_nz++;
        unsigned nums[4] = {0}; // nassert, npassed, ncases, ncases_run
        int retval = 0;

        #ifndef CT_NO_FORK
        int pipefd[2] = {0};

        if (pipe(pipefd) == -1) {
            CT_PRINTF("cannot setup pipe\n");
            exit(EXIT_FAILURE);
        }

        pid_t p = fork();
        if (p == 0) { // child
            ct_suite_run( ct_suites[s], &nums[0], &nums[1], &nums[2], &nums[3], filters );
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
        #else // CT_NO_FORK
        ct_suite_run( ct_suites[s], &nums[0], &nums[1], &nums[2], &nums[3], filters );
        #endif // CT_NO_FORK

        unsigned nassert = nums[0], npassed = nums[1],
                 ncases = nums[2], ncases_run = nums[3];

        nassert_total += nassert;
        npassed_total += npassed;
        ncases_total += ncases;
        ncases_run_total += ncases_run;
        unsigned suite_success = (npassed == nassert) && !retval;
        if (retval) {
            ncrashed++;
            CT_PRINTF( "%s=== ct suite '%s' CRASHED (%i) ===%s\n", ct_color_red,
                ct_suites[s]->name, retval, ct_color_reset );
        } else {
            if (ncases_run > 0) {
                CT_PRINTF( "%s=== ct suite '%s' passed %u/%u assertions in %u/%u cases ===%s\n",
                    suite_success ? ct_color_green : ct_color_red, ct_suites[s]->name, npassed,
                    nassert, ncases_run, ncases, ct_color_reset );
            } else {
                CT_PRINTF( "%s=== ct suite '%s' filtered out ===%s\n", ct_color_yellow,
                    ct_suites[s]->name, ct_color_reset );
            }
        }
        successes += suite_success;
    }

    CT_PRINTF( "%s=== ct report ===%s\n", ct_color_yellow, ct_color_reset );
    CT_PRINTF( "%s=== crashed %u/%u suites with %u/%u failed assertions in %u/%u cases ===%s\n",
        successes == ct_nsuites_nz ? ct_color_green : ct_color_red,
        ncrashed, ct_nsuites_nz, nassert_total - npassed_total, nassert_total,
        ncases_run_total, ncases_total, ct_color_reset );

    return !(successes == ct_nsuites_nz);
}
