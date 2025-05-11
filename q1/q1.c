// COMP9315 22T1 Final Exam Q1
// Count tuples in a no-frills file

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "no-frills.h"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Usage: %s DataFile\n", argv[0]);
		exit(1);
	}
	int fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		printf("Can't open file %s\n", argv[1]);
		exit(1);
	}

	int ntuples = 0;

	// Add variables and code here to work out

	// 获取文件大小
	off_t filesize = lseek(fd, 0, SEEK_END);

	if (filesize == 0 || filesize % PAGESIZE != 0)
	{
		printf("-1\n");
		close(fd);
		return 0;
	}

	lseek(fd, 0, SEEK_SET);

	char buffer[PAGESIZE];

	while (read(fd, buffer, PAGESIZE) == PAGESIZE)
	{
		ntuples += buffer[0];
	}

	// the total number of tuples

	printf("%d\n", ntuples);
	return 0;
}
