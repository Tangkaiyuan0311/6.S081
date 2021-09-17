/*
 * user program call c library wrapper int sleep(int);
 * wrapper set up the argument and trap to SYS_sleep
 * SYS_sleep call sleep service routine
 * This program is a user level program
 * */
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void main(int argc, char** argv) {
	
	if (argc != 2) {
		write(2, "Usage: sleep <ticks>\n", 21);
		exit(0);
	}

	// int atoi(const char*);
	int ticks = atoi(argv[1]);
	if (sleep(ticks) < 0) {
		write(2, "Error\n", 6);
		exit(1);
	}
	exit(0);
} 
