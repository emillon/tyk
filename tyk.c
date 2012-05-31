#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>

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

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
    } else if (pid > 0) {
        /* Parent */
        waitpid(pid, NULL, 0);
    } else {
        /* Child */
        int z = execvp(progname, &argv[1]);
        if (z != 0) {
            perror("execvp");
        }
    }

    return 0;
}