#include "unit_utils.h"

void assert_(bool condition, const char* message) {
    if (!condition) {
        printf("%s\n", message);
        exit(1);
    }
}