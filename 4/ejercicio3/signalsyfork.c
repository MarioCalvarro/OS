#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>

pid_t kill_pid;

void kill_child(int sig)
{
    if (sig == SIGALRM) {
        kill(kill_pid, SIGTERM);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2) { 
        perror("One argument required (name of program and its arguments).");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork(); 
    if (pid == 0) { 
        execvp(argv[1], argv + 1);
        err(EXIT_FAILURE, "ERROR!");
    }

    kill_pid = pid;

    struct sigaction act = { 0 };
    act.sa_handler = &kill_child;
    sigaction(SIGALRM, &act, NULL);
    alarm(5);

    int status;
    waitpid(pid, &status, WUNTRACED | WCONTINUED);
    if (WIFEXITED(status)) { 
        printf("Child terminanted normally!\n"); 
    }
    else if (WIFSIGNALED(status)) {
        printf("Child terminanted by signal!\n");
    }

	return 0;
}
