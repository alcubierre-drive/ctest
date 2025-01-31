#!/bin/bash

suites_dir="test/"
outfile="ctest_run.h"
cpp_mode=false

if [ -z "$1" ]; then
    echo "print help with $(basename "$0") -h"
fi
while getopts "i:o:Ch" opt; do
    case "$opt" in
        h)
            echo -e \
                "usage: $(basename "$0")\n\
                    [-i test dir (default: 'test/')]\n\
                    [-o output file (default: 'ctest_run.h')]\n\
                    [-C:also search for .cpp files]"
            exit 0
            ;;
        i)
            suites_dir="$OPTARG"
            ;;
        f)
            outfile="$OPTARG"
            ;;
        C)
            cpp_mode=true;
            ;;
    esac
done
shift $((OPTIND-1))
[ "$1" = "--" ] && shift

suites=( $(find "$suites_dir" -iname "*.c") )
if $cpp_mode; then
    cppsuites=( $(find "$suites_dir" -iname "*.cpp") )
    for ((i=0; i<${#cppsuites[@]}; i++)); do
        suites+=( ${cppsuites[$i]} )
    done
fi

cat <<EOF > "$outfile"
#pragma once

/** AUTOGENERATED BY 'ctest_headers.sh' **
 *
 * HEADER FILE THAT IMPLEMENTS THE TESTING FRAMEWORK GIVEN TEST SUITES IN THE
 * DIRECTORY '$suites_dir'. THE MAIN TESTING FUNCTION IS DECLARED AS
 *
 * ctest_run( int* pargc, char*** pargv );
 *
 * NOTE THAT YOU CAN CHOOSE TO NOT ALLOW FORKING BY DEFINING 'CTEST_NO_FORK'
 * BEFORE INCLUDING THIS HEADER FILE ('$outfile').
 */

#include "ctest.h"

#ifdef __cplusplus
extern "C" {
#endif

EOF

funcs=()
for suite in ${suites[@]}; do
    ini_name="$(grep INI_SUITE "${suite}" | cut -d"(" -f2 | cut -d"," -f1 | tr -d ' ')"
    if ! [ -z $ini_name ]; then
        funcs+=( "register_$ini_name" )
        echo "testsuite_t register_$ini_name( void );" >> "$outfile"
    fi
done

cat <<EOF >> "$outfile"

static int ctest_run( int argc, char** argv ) {
    testsuite_t suites[${#funcs[@]}+1] = {0};
    unsigned nsuites = 0;
EOF

# register all testsuites
for ((i=0; i<${#funcs[@]}; i++)); do
    echo "    suites[nsuites++] = ${funcs[$i]}();" >> "$outfile"
done

cat <<EOF >> "$outfile"

    char** filters = argv;
    filters[argc] = NULL;
    filters++; // skip argv[0]
    return ctest_run_suites( suites, nsuites, filters );
}

#ifdef __cplusplus
}
#endif
EOF
