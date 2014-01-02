#include <cerrno>
#include <sys/socket.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <grp.h>
#include <iostream>
#include <libgen.h>
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
int test_unix_latency(unsigned int iterations, float iterDelay)
{
    int sv[2]; /* the pair of socket descriptors */
    int size = sizeof(unsigned int);
    double min = FLT_MAX, max = 0.0, total = 0.0; // Time in seconds for all

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        perror("socketpair");
        exit(1);
    }

    if (!fork()) {  /* child */
        int val1, val2;
        int res;
        for (unsigned int iter = 0; iter < iterations; iter++) {

            if (read(sv[1], &val1, size) != size) {
                perror("read");
                return 1;
            }
            if (read(sv[1], &val2, size) != size) {
                perror("read");
                return 1;
            }
            res = val1 + val2;

            if (write(sv[1], &res, size) != size) {
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

            if (write(sv[0], &val1, size) != size) {
                perror("write");
                return 1;
            }
            if (write(sv[0], &val2, size) != size) {
                perror("write");
                return 1;
            }

            if (read(sv[0], &res, size) != size) {
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
    cout << "unix socket Time per iteration min: " << min
        << " avg: " << (total / iterations)
        << " max: " << max
        << " total: " << total
        << endl;

    return 0;
}
