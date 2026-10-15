#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <string.h>

int main(void)
{
    char file_name[PATH_MAX];
    printf("Enter a file name\n");
    fflush(stdout);
    
    if (fgets(file_name, sizeof(file_name), stdin) == NULL)
    {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }
    file_name[strcspn(file_name, "\n")] = '\0';
    int file_fd = open(file_name, O_RDONLY);
    if (file_fd == -1)
    {
        perror("open");
        return 1;
    }
    int pipe1[2];
    int result = pipe(pipe1);
    if (result == -1)
    {
        perror("pipe");
        return 1;
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return 1;
    }
    else if (pid == 0)
    {
        printf("Child PID: %d\n", getpid());
        fflush(stdout);
        if (dup2(file_fd, 0) == -1)
        {
            perror("dup2 stdin");
            return 1;
        }
        if (dup2(pipe1[1], 1) == -1)
        {
            perror("dup2 stdout");
            return 1;
        }
        close(file_fd);
        close(pipe1[0]);
        close(pipe1[1]);
        execl("./child", "child", (char *)NULL);
        perror("execl");
        return 1;
    }
    else
    {
        printf("Parent PID: %d\n", getpid());
        fflush(stdout);
        close(pipe1[1]);
        close(file_fd);
        char buf[256];
        ssize_t n;
        while ((n = read(pipe1[0], buf, 256)) > 0)
        {
            write(1, buf, n);
        }
        if (n == -1)
        {
            perror("read");
        }
        close(pipe1[0]);
        int status;
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid");
            return 1;
        }
        if (WIFEXITED(status))
        {
            printf("Child exit code: %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Child killed by signal %d\n", WTERMSIG(status));
        }
    }
    return 0;
}