#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// find all the files in a directory tree with a specific name

/*
// formatted output
char* fmtname(char *path)
{
	static char buf[DIRSIZ+1];
	char *p;

	// Find first character after last slash.
	for(p=path+strlen(path); p >= path && *p != '/'; p--)
		;
	p++;

	// Return blank-padded name.
	if(strlen(p) >= DIRSIZ)
		return p;
	memmove(buf, p, strlen(p));
	memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
	return buf;
}
*/

// print all filename in the given directory fd (with path) recursively
// do not recurse into . ..
// assume: fd has been opened, fd is a directory, fd = open(path, 0);
void find(int fd, char* path, char* name) {
	struct dirent de; //directory entry
	struct stat st; // file status
	char buf[512], *p;
	int _fd;

	// read each entry of directory fd into de
	while (read(fd, &de, sizeof(de)) == sizeof(de)) {
		if (de.inum == 0)
			continue;
		if ((strcmp(de.name, ".") == 0) || (strcmp(de.name, "..") == 0))
			continue;

		// construct full filename into buf (path + de.name)
		memmove(buf, path, strlen(path)+1);
		p = buf + strlen(path); // *p == 0
		*(p++) = '/';
		memmove(p, de.name, strlen(de.name)+1);

		if (strcmp(de.name, name) == 0)
			printf("%s\n", buf);

		if(stat(buf, &st) < 0){
			printf("cannot stat %s\n", buf);
			continue;
		}
		if (st.type == T_DIR) {
			if((_fd = open(buf, 0)) < 0){
				fprintf(2, "find: cannot open %s\n", buf);
				continue;
			}
			find(_fd, buf, name);
			close(_fd);
		}
	}
}


int main (int argc, char** argv) {
	char path[512];
	char name[DIRSIZ+1];

	if (argc == 2) {// find in current path
		path[0] = '.';
		path[1] = 0;
		memmove(name, argv[1], strlen(argv[1])+1);
	}
	else if (argc == 3) {
		memmove(path, argv[1], strlen(argv[1])+1);
		if (strlen(argv[2]) > DIRSIZ) {
			fprintf(2, "name is too long\n");
			exit(0);
		}
		memmove(name, argv[2], strlen(argv[2])+1);
	}
	else {
		fprintf(2, "Usage: %s <path> <name>\n", argv[0]);
		exit(0);
	}

	// path and name has been created
	// path can be either relative or absolute

	// open the file: path
	int fd;
	if((fd = open(path, 0)) < 0){
		fprintf(2, "find: cannot open %s\n", path);
		exit(1);
	}
	// get file status
	struct stat st;
	if(fstat(fd, &st) < 0){
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		exit(1);
	}
	switch(st.type) {
		case T_FILE:
			fprintf(2, "%s: not a directory\n", path);
			close(fd);
			exit(1);
		case T_DIR:
			find(fd, path, name);
			close(fd);
			break;
	}
	exit(0);
}














