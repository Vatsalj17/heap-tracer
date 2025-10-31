#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("usage: %s <executable>\n", argv[0]);
		return EXIT_FAILURE;
	}
    const char* shim = "./shim.so";
    const char* var = "LD_PRELOAD";
	setenv(var, shim, 1);
	pid_t child = fork();
    int status;
	if (child == 0) {
		execvp(argv[1], argv + 1);
	} else {
        waitpid(child, &status, WUNTRACED);
    }
}
