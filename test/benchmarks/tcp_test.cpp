#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <netdb.h>
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


int test_tcp_latency(unsigned int iterations, float iterDelay)
{
    int size = sizeof(unsigned int);
    double min = FLT_MAX, max = 0.0, total = 0.0; // Time in seconds for all

    ssize_t len;
    size_t sofar;

    int yes = 1;
    int ret;
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints;
    struct addrinfo *res;
    int sockfd, new_fd;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
    if ((ret = getaddrinfo("127.0.0.1", "3491", &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return 1;
    }


    if (!fork()) {  /* child */

        if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
            perror("socket");
            exit(1);
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        } 

        if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
            perror("bind");
            exit(1);
        }

        if (listen(sockfd, 1) == -1) {
            perror("listen");
            exit(1);
        } 

        addr_size = sizeof their_addr;

        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size)) == -1) {
            perror("accept");
            exit(1);
        } 

        int val1, val2;
        int res;
        for (unsigned int iter = 0; iter < iterations; iter++) {

            for (sofar = 0; sofar < size; ) {
                len = read(new_fd, &val1 + sofar, size - sofar);
                if (len == -1) {
                    perror("read");
                    return 1;
                }
                sofar += len;
            }
            for (sofar = 0; sofar < size; ) {
                len = read(new_fd, &val2 + sofar, size - sofar);
                if (len == -1) {
                    perror("read");
                    return 1;
                }
                sofar += len;
            }

            res = val1 + val2;

            if (write(new_fd, &res, size) != size) {
                perror("write");
                return 1;
            }
        }
            exit(0);
    } else { /* parent */

        sleep(1);

        if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
            perror("socket");
            exit(1);
        }

        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
            perror("connect");
            exit(1);
        }

        for (unsigned int iter = 0; iter < iterations; iter++) {
            int val1 = iter;
            int val2 = iter + 3;
            int expected = val1 + val2;  // Expect to get the sum back
            int res;
            struct timespec start;
            clock_gettime(CLOCK_MONOTONIC, &start);

            if (write(sockfd, &val1, size) != size) {
                perror("write");
                return 1;
            }
            if (write(sockfd, &val2, size) != size) {
                perror("write");
                return 1;
            }

            for (sofar = 0; sofar < size; ) {
                len = read(sockfd, &res + sofar, size - sofar);
                if (len == -1) {
                    perror("read");
                    return 1;
                }
                sofar += len;
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
    cout << "tcp socket Time per iteration min: " << min
        << " avg: " << (total / iterations)
        << " max: " << max
        << " total: " << total
        << endl;

    return 0;
}
