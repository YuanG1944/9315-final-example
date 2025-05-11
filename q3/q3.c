#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "no-frills.h"

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s DataFile TupleFile\n", argv[0]);
		exit(1);
	}

	unsigned int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, mode);
	if (fd < 0)
	{
		perror("Cannot open data file");
		exit(1);
	}

	FILE *input = fopen(argv[2], "r");
	if (input == NULL)
	{
		perror("Cannot open tuple file");
		close(fd);
		exit(1);
	}

	char page[PAGESIZE];
	memset(page, 0, PAGESIZE);
	char *cur = &page[1]; // 数据起始位置
	page[0] = 0;					// 元组数量计数器

	char tuple[MAXTUPLEN];
	int page_number = 0;

	while (fgets(tuple, MAXTUPLEN, input) != NULL)
	{
		size_t len = strcspn(tuple, "\r\n");
		tuple[len] = '\0';
		size_t tuplen = len + 1;

		size_t remaining = PAGESIZE - (cur - page);
		if (tuplen + 2 > remaining)
		{
			if (write(fd, page, PAGESIZE) != PAGESIZE)
			{
				perror("Error writing page");
				fclose(input);
				close(fd);
				exit(1);
			}
			page_number++;
			memset(page, 0, PAGESIZE);
			page[0] = 0;
			cur = &page[1];
		}

		// ✅ 插入元组，并更新计数
		memcpy(cur, tuple, tuplen);
		cur += tuplen;
		page[0]++;
	}

	// 最后一页（即使为 0 元组）也要写
	if (page[0] > 0 || page_number == 0)
	{
		if (write(fd, page, PAGESIZE) != PAGESIZE)
		{
			perror("Error writing final page");
			fclose(input);
			close(fd);
			exit(1);
		}
	}

	fclose(input);
	close(fd);
	return 0;
}
