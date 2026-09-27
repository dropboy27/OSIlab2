#include <iostream>
#include <string>
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    std::string file_name;
    std::cout << "Enter a file name" << std::endl;
    std::cin >> file_name;
    int file_fd = open(file_name.c_str(), O_RDONLY);
    if (file_fd == -1) {
        perror("open");
        return 1;
    }
    int pipe1[2];
    int result = pipe(pipe1);
    if (result == -1) {
        perror("pipe");
        return 1;
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        std::cout << "Child PID: " << getpid()<< std::endl;
        if (dup2(file_fd, 0) == -1) {
            perror("dup2 stdin");
            return 1;
        }
        if (dup2(pipe1[1], 1) == -1) {
            perror("dup2 stdout");
            return 1;
        }
        close(file_fd);
        close(pipe1[0]);
        close(pipe1[1]);
        execl("./child", "child", (char*)NULL);
        perror("execl");
        return 1;
    } else {
        std::cout << "Parent PID: " << getpid()<< std::endl;
        close(pipe1[1]);
        close(file_fd);
        char buf[256];
        ssize_t n;
        while ((n = read(pipe1[0], buf, 256)) > 0){
            write(1, buf, n);

        }
        if (n == -1) {
            perror("read");
        }
        close(pipe1[0]);
        int status;
        waitpid(pid, &status, 0);
    }
}