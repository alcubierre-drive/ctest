#define CT_MAIN
#include <ctest.h>

int main(int argc, char** argv) {
    argv[argc] = NULL;
    if (argc <= 1) return ct_run(argv+argc);

    if (!strcmp(argv[1],"-l")) return ct_list(argv+2);
    else                       return ct_run(argv+1);
}
