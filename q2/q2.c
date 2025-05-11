// COMP9315 22T1 Final Exam Q1
// Find longest tuple in a no-frills file

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
	// char longest[MAXTUPLEN];

	// Add variables and code here to find

	// 检查文件大小合法性
	off_t filesize = lseek(fd, 0, SEEK_END);
	if (filesize == 0 || filesize % PAGESIZE != 0)
	{
		printf("<none>\n");
		close(fd);
		return 0;
	}
	lseek(fd, 0, SEEK_SET); // 重置读指针

	char page[PAGESIZE];
	char longest[MAXTUPLEN] = {0};

	int longest_len = 0;

	while (read(fd, page, PAGESIZE) == PAGESIZE)
	{
		int ntuples = page[0];

		if (ntuples == 0)
			continue;

		int i = 1; // 从 page[1] 开始是 tuple 数据区
		for (int t = 0; t < ntuples && i < PAGESIZE; t++)
		{
			int start = i;

			// 寻找 tuple 末尾 '\0'
			while (i < PAGESIZE && page[i] != '\0')
			{
				i++;
			}

			if (i >= PAGESIZE)
				break; // 防御性保护

			int len = i - start;

			if (len > longest_len)
			{
				memcpy(longest, &page[start], len);
				longest[len] = '\0';
				longest_len = len;
			}

			i++; // 跳过 '\0'

			// 如果是最后一个 tuple，再跳过额外一个 '\0'
			if (t == ntuples - 1 && i < PAGESIZE && page[i] == '\0')
			{
				i++;
			}
		}
	}

	if (longest_len == 0)
		printf("<none>\n");
	else
		printf("%s\n", longest);

	close(fd);
	// the longest tuple in the data file

	return 0;
}
