/* Copyright (c) 2012 Etienne Millon <me@emillon.org>
 * ----------------------------------------------------------------------------
 *                        "THE BEER-WARE LICENSE"
 *
 * <me@emillon.org> wrote this file. As long as you retain this notice you can
 * do whatever you want with this stuff. If we meet some day, and you think this
 * stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static struct timeval time_start;
static pid_t child_pid;

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

static void timeout_process (void)
{
    printf("Timeout.\n");
    int z = kill(child_pid, SIGTERM);
    if (z != 0) {
        perror("kill");
    }
}

static void handle_signals(int signum)
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
    } else if (signum == SIGALRM) {
        timeout_process();
    }
}

static void usage(char * progname)
{
    printf("Usage : %s [-t time] [-s size] prog args\n", progname);
    exit(EX_USAGE);
}

static void hide_cursor()
{
    printf("\e[?25l");
}

static void show_cursor()
{
    printf("\e[?25h");
}

static void draw_pb(int pb_size, int totaltime_sec)
{
    int i;
    int time_frame_usec = 125000 * totaltime_sec / pb_size;
    const char* boxes[] =
        { "▏"
        , "▎"
        , "▍"
        , "▌"
        , "▋"
        , "▊"
        , "▉"
        , "█"
        };

    printf("[\e[%dC]\e[%dD", pb_size, pb_size+1);

    hide_cursor();
    atexit(show_cursor);
    for(i=0;i<8*pb_size;i++) {
        int m = i % 8;
        printf("%s", boxes[m]);
        if (m != 7) {
            printf("\e[D");
        }
        fflush(stdout);
        usleep(time_frame_usec);
    }
    printf("\n");
}

static int parse_time(char *s)
{
    size_t n = strlen(s);

    if (n == 0) {
        return -1;
    }

    char sfx = s[n-1];

    if (isalpha(sfx)) {
        int mul = 1;
        switch (sfx) {
            case 'h':
                mul = 60*60;
                break;
            case 'm':
                mul = 60;
                break;
            default:
                printf("Unknown time suffix %c\n", sfx);
                exit(EX_USAGE);
                break;
        }

        char * ds = strdup(s);
        ds[n-1] = '\0';
        int base = atoi(ds);
        free(ds);
        return (base * mul);
    }

    return atoi(s);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        usage(argv[0]);
    }

    int timeout_sec = 8;
    int pb_size = 10;

    int opt;
    while ((opt = getopt(argc, argv, "t:s:")) != -1) {
        switch (opt) {
            case 't':
                timeout_sec = parse_time(optarg);
                break;
            case 's':
                pb_size = atoi(optarg);
                break;
            default:
                printf("No such option : %d\n", opt);
                usage(argv[0]);
                break;
        }
    }

    char * progname = argv[optind];

    int z = gettimeofday(&time_start, NULL);
    if (z != 0) {
        perror("gettimeofday");
    }

    child_pid = fork();

    if (child_pid < 0) {
        perror("fork");
    } else if (child_pid > 0) {
        /* Parent */
        signal(SIGCHLD, handle_signals);
        signal(SIGALRM, handle_signals);
        alarm(timeout_sec);
        draw_pb(pb_size, timeout_sec);
        waitpid(child_pid, NULL, 0);
    } else {
        /* Child */
        z = execvp(progname, &argv[optind]);
        if (z != 0) {
            perror("execvp");
        }
    }

    return 0;
}
