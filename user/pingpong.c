#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
	int p1[2]; // file descriptor into the kernel buffer, p->c
	int p2[2]; // c->p
	pipe(p1);
	pipe(p2);
	char buf[1];

	// p[0] for read, p[1] for write
	if (fork() == 0) {
		close(p1[1]);
		close(p2[0]);
		if (read(p1[0], buf, 1) < 1) {
			write(2, "write end closed\n", 16);
			exit(1);
		}
		printf("%d: received ping\n", getpid());
		if (write(p2[1], buf, sizeof(buf)) < 1) {
            write(2, "Write Error\n", 12);
            exit(1);
        }
		close(p1[0]);
		close(p2[1]);
		exit(0);
	}
	else {
		close(p1[0]);
        close(p2[1]);
		if (write(p1[1], "p", 1) < 1) {
			write(2, "Write Error\n", 12);
			exit(1);
		}
		if (read(p2[0], buf, 1) < 1) {
            write(2, "read end closed\n", 16);
            exit(1);
        }
		printf("%d: received pong\n", getpid());
		if (wait(0) < 0) {
			write(2, "Wait Error\n", 11);
            exit(1);
		}
		close(p1[0]);
		close(p2[1]);
		exit(0);
	}
}
