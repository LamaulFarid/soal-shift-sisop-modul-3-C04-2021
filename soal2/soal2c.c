#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int pid;
int pipe_[2][2];

void command1() {
    dup2(pipe_[0][1], 1); // set output ke pipe_[0]

    // close fds
    close(pipe_[0][0]);
    close(pipe_[0][1]);

    execlp("ps", "ps", "-aux", NULL);
    exit(EXIT_SUCCESS);
}

void command2() {
    dup2(pipe_[0][0], 0);   // input berasal dari pipe_[0]
    dup2(pipe_[1][1], 1);  // set output to pipe_[1]

    // close fds
    close(pipe_[0][0]);
    close(pipe_[0][1]);
    close(pipe_[1][0]);
    close(pipe_[1][1]);

    execlp("sort", "sort", "-nrk", "3,3", NULL);
    exit(EXIT_SUCCESS);
}

void command3() {
    dup2(pipe_[1][0], 0); // input from pipe_[1]

    // close fds
    close(pipe_[1][0]);
    close(pipe_[1][1]);
    
    execlp("head", "head", "-5", NULL);
    exit(EXIT_SUCCESS);
}

int main() {
    // buat pipe_[0]
    if(pipe(pipe_[0]) == -1){
        perror("gagal membuat pipe");
        exit(EXIT_FAILURE);
    }

    // fork (ps -aux)
    if(fork() == 0) command1();

    // buat pipe_[1]
    if(pipe(pipe_[1]) == -1){
        perror("gagal membuat pipe");
        exit(EXIT_FAILURE);
    }

    // fork (sort)
    if(fork() == 0) command2();

    // tutup fds tidak terpakai
    close(pipe_[0][0]);
    close(pipe_[0][1]);

    // fork (grep sbin)
    if(fork() == 0) command3();
}
