#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

static struct timeval time_start;

static void print_timedelta(time_t *delta)
{
    struct tm *t = localtime(delta);

    /* Normalize because delta is not a date */
    t->tm_year -= 70;
    t->tm_mday -= 1;
    t->tm_hour -= 1;

    printf("\n");
    if(t->tm_year >0) printf("%dy ", t->tm_year);
    if(t->tm_mon  >0) printf("%dm ", t->tm_mon);
    if(t->tm_mday >0) printf("%dd ", t->tm_mday);
    if(t->tm_hour >0) printf("%dh ", t->tm_hour);
    if(t->tm_min  >0) printf("%dm ", t->tm_min);
    if(t->tm_sec  >0) printf("%ds" , t->tm_sec);
    printf("\n");
}

static void handle_sigchld(int signum)
{
    if (signum == SIGCHLD) {
        struct timeval time_end, delta;

        int z = gettimeofday(&time_end, NULL);
        if (z != 0) {
            perror("gettimeofday");
        }

        timersub (&time_end, &time_start, &delta);
        print_timedelta(&delta.tv_sec);
        exit(0);
    }
}

static void usage(char * progname)
{
    printf("Usage : %s prog args\n", progname);
    exit(EX_USAGE);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        usage(argv[0]);
    }

    char * progname = argv[1];

    int z = gettimeofday(&time_start, NULL);
    if (z != 0) {
        perror("gettimeofday");
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
    } else if (pid > 0) {
        /* Parent */
        signal(SIGCHLD, handle_sigchld);
        int i;
        for (i=0;;i++) {
            putchar('.');
            fflush(stdout);
            sleep(1);
        }
    } else {
        /* Child */
        int z = execvp(progname, &argv[1]);
        if (z != 0) {
            perror("execvp");
        }
    }

    return 0;
}
