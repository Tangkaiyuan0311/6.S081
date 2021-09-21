#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

#define MAXLINE 1024

// read a line from fd, stores as c string in line, return number of bytes excluding '\0'
// size of line is maintained by caller
// return 0 on EOF or a single '\n'
int readline(char* line, int fd) {
	int i = 0; // bytes counter
	char c;
	while (read(fd, &c, 1) == 1) {
		if (c != '\n')
			line[i++] = c;
		else {
			line[i] = 0;
			break;
		}
	}
	return i;
}

// read one line from stdin, construct argv as {arg1, arg2..., argn, line}
// execute argv[1] one time for each line
int main(int argc, char** argv) {
	int nchild = 0; // number of times command is executed
	char* arg_buf[MAXARG]; // arguments to command
	int init_arg = argc - 2; // initial argument
	arg_buf[0] = argv[1]; // command name
	for (int i = 1; i < init_arg+1; i++)
		arg_buf[i] = argv[i+1];
	arg_buf[init_arg+2] = 0; // NULL terminated list of pointers
	char* command = argv[1];
	char line[MAXLINE]; // argument from stdin
	// read lines from stdin
	while (readline(line, 0) != 0) {
		//printf("read line: %s\n", line);
		nchild++;
		arg_buf[init_arg+1] = line;

		if (fork() == 0)
			exec(command, arg_buf);
	}

	//printf("reaping...\n");
	for (int i = 0; i < nchild; i++)
		wait(0);
	exit(0);
}
