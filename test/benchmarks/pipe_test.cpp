#include <cerrno>
#include <grp.h>
#include <iostream>
#include <libgen.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <testUtil.h>
#include <values.h>
#include <string.h>

using namespace std;
int test_pipe_latency(unsigned int iterations, float iterDelay)
{
    int ofds[2];
    int ifds[2];
    int size = sizeof(unsigned int);
    double min = FLT_MAX, max = 0.0, total = 0.0; // Time in seconds for all

    if (pipe(ofds) == -1) {
        perror("pipe");
        return 1;
    }

    if (pipe(ifds) == -1) {
        perror("pipe");
        return 1;
    }

    if (!fork()) {  /* child */
        int val1, val2;
        int res;
        for (unsigned int iter = 0; iter < iterations; iter++) {
            if (read(ifds[0], &val1, size) != size) {
                perror("read");
                return 1;
            }
            if (read(ifds[0], &val2, size) != size) {
                perror("read");
                return 1;
            }
            res = val1 + val2;
            if (write(ofds[1], &res, size) != size) {
                perror("write");
                return 1;
            }
        }
    exit(0);
    } else { /* parent */

        for (unsigned int iter = 0; iter < iterations; iter++) {

            int val1 = iter;
            int val2 = iter + 3;
            int expected = val1 + val2;  // Expect to get the sum back
            int res;
            struct timespec start;
            clock_gettime(CLOCK_MONOTONIC, &start);
            if (write(ifds[1], &val1, size) != size) {
                perror("write");
                return 1;
            }

            if (write(ifds[1], &val2, size) != size) {
                perror("write");
                return 1;
            }

            if (read(ofds[0], &res, size) != size) {
                perror("read");
                return 1;
            }
            struct timespec current;
            clock_gettime(CLOCK_MONOTONIC, &current);
            struct timespec deltaTimespec = tsDelta(&start, &current);
            double delta = ts2double(&deltaTimespec);
            min = (delta < min) ? delta : min;
            max = (delta > max) ? delta : max;
            total += delta;
            int result = res;
            //cout << res <<endl;
            if (result != (int) (iter + iter + 3)) {
                cerr << "Unexpected result for iteration " << iter << endl;
                cerr << "  result: " << result << endl;
                cerr << "expected: " << expected << endl;
            }

            if (iterDelay > 0.0) { testDelaySpin(iterDelay); }

        }


    }
    cout << "pipe Time per iteration min: " << min
        << " avg: " << (total / iterations)
        << " max: " << max
        << " total: " << total
        << endl;

    return 0;
}
