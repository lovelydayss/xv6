#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "kernel/types.h"
#include "user/user.h"

char* fmt_name(char* path) {

	static char buf[DIRSIZ + 1];
	char* p;

	// 查找首个非 '/' 位置
	for (p = path + strlen(path); p >= path && *p != '/'; p--)
		;

	p++;
	memmove(buf, p, strlen(p) + 1);
	return buf;
}

void eq_print(char* file_name, char* find_name) {

	if (strcmp(fmt_name(file_name), find_name) == 0)
		printf("%s\n", file_name);
}

void find(char* path, char* find_name) {

	int fd;
	struct stat st;
	if ((fd = open(path, O_RDONLY)) < 0) {
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}
	if (fstat(fd, &st) < 0) {
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	char buf[512], *p;
	struct dirent de;
	switch (st.type) {
	case T_FILE:
		eq_print(path, find_name);
		break;
	case T_DIR:
		strcpy(buf, path);
		p = buf + strlen(buf);
		*p++ = '/';
		while (read(fd, &de, sizeof(de)) == sizeof(de)) {
			// printf("de.name:%s, de.inum:%d\n", de.name, de.inum);
			if (de.inum == 0 || de.inum == 1 || strcmp(de.name, ".") == 0 ||
			    strcmp(de.name, "..") == 0)
				continue;
			memmove(p, de.name, strlen(de.name));
			p[strlen(de.name)] = 0;
			find(buf, find_name);
		}
		break;
	}

    close(fd);
}

int main(int argc, char* argv[]) {

	if (argc < 3) {
		printf("find: find <path> <fileName>\n");
		exit(0);
	}

	// 转为调用 find 函数
	find(argv[1], argv[2]);
	exit(0);
}
