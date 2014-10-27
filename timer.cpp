/*This file contains the declaration of the function timerval() that is used in the benchmarks to
 *register time
 */
#include <time.h>

double timerval () {
struct timespec st;
clock_gettime (CLOCK_REALTIME, &st);
return st.tv_sec + st.tv_nsec * 1e-9;
}



