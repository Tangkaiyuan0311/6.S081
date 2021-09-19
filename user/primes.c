#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/*
  resursive structure
  assume: read pipe is set up (pl[1] = -1), write pipe is uninitialized
  process i: read from pl[0], handle the job of process i, i+1..., reap i+1, i+2... after job is done
 */
void pipeline(int* pl, int* pr) {
	int p;
	int n;
	read(pl[0], &p, 4); // assume has at least one int to transfer
	printf("prime %d\n", p);
	if (read(pl[0], &n, 4) < 4) {
		exit(0); // no more process needed, ensure that new process has at least one input
	}
	else 
		// set up pr
		pipe(pr);
	if (fork() == 0) {
		// process i+1
		close(pl[0]);
		pl[0] = pr[0];
		close(pr[1]);
		pr[0] = pr[1] = -1;
		pipeline(pl, pr);
		exit(0);
	}
	else {
		// process i
		close(pr[0]);
		do
			if (n % p != 0)
				write(pr[1], &n, 4);
		while (read(pl[0], &n, 4) == 4);
		close(pr[1]);
		close(pl[0]);
		wait(0);
	}
}

int main() {
	int pl[2], pr[2];
	pipe(pl);
	pr[0] = pr[1] = -1;
	int i;
	for (i = 2; i <= 35; i++)
		write(pl[1], &i, 4);
	close(pl[1]);
	pl[1] = -1;
	pipeline(pl, pr);
	exit(0);
}




























