/*
 * stack - use a file as a stack
 *
 * Copyright (C) 2015 Jan Luebbe <j.luebbe@pengutronix.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>

void usage(char *prgname)
{
	printf("usage: %s <stack-file> <command> <arg>\n"
	       "\n"
	       "commands:\n"
	       "  push	append file <arg> to <stack-file>\n"
	       "  pop	remove last element from <stack-file> to file <arg>\n"
	       "  cat	go down <arg> elements from the top of <stack-file> and copy to standard output\n"
	       "  check	check that <stack-file> contains exactly <arg> elements\n",
	       prgname);
}

static void fpush(const char *stackfile, const char *elementfile)
{
	FILE *stack, *element;
	off_t element_size;
	uint64_t remaining, t64;

	stack = fopen(stackfile, "a");
	if (stack == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	if (fseeko(stack, 0, SEEK_END) == -1) {
		perror("fseeko");
		exit(EXIT_FAILURE);
	}

	element = fopen(elementfile, "r");
	if (element == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	if (fseeko(element, 0, SEEK_END) == -1) {
		perror("fseeko");
		exit(EXIT_FAILURE);
	}

	element_size = ftello(element);
	if (element_size == -1) {
		perror("ftello");
		exit(EXIT_FAILURE);
	}

	if (fseeko(element, 0, SEEK_SET) == -1) {
		perror("fseeko");
		exit(EXIT_FAILURE);
	}

	remaining = element_size;
	while (remaining > 0) {
		char buf[4096];
		uint64_t step = remaining < sizeof(buf) ? remaining : sizeof(buf);

		if (fread(buf, 1, step, element) != step) {
			fprintf(stderr, "failed to read from %s\n", elementfile);
			exit(EXIT_FAILURE);
		}
		if (fwrite(buf, 1, step, stack) != step) {
			fprintf(stderr, "failed to write to stack file\n");
			exit(EXIT_FAILURE);
		}
		remaining -= step;
	}

	t64 = htobe64(element_size);
	if (fwrite(&t64, sizeof(t64), 1, stack) != 1) {
		fprintf(stderr, "failed to write to stack file\n");
		exit(EXIT_FAILURE);
	}

	if (fclose(element) != 0) {
		perror("fclose");
		exit(EXIT_FAILURE);
	}

	if (fclose(stack) != 0) {
		perror("fclose");
		exit(EXIT_FAILURE);
	}
}

static void fpop(const char *stackfile, const char *elementfile)
{
	FILE *element, *stack;
	off_t element_size, stack_off;
	uint64_t remaining, t64;

	stack = fopen(stackfile, "r+");
	if (stack == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	if (fseeko(stack, 0, SEEK_END) == -1) {
		perror("fseeko");
		exit(EXIT_FAILURE);
	}

	element = fopen(elementfile, "w");
	if (element == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	if (fseeko(stack, -sizeof(t64), SEEK_END) == -1) {
		perror("fseeko");
		exit(EXIT_FAILURE);
	}

	if (fread(&t64, sizeof(t64), 1, stack) != 1) {
		fprintf(stderr, "failed to read from stack file\n");
		exit(EXIT_FAILURE);
	}
	element_size = be64toh(t64);

	if (fseeko(stack, -(element_size + sizeof(t64)), SEEK_END) == -1) {
		perror("fseeko");
		exit(EXIT_FAILURE);
	}

	stack_off = ftello(stack);
	if (stack_off == -1) {
		perror("ftello");
		exit(EXIT_FAILURE);
	}

	remaining = element_size;
	while (remaining > 0) {
		char buf[4096];
		uint64_t step = remaining < sizeof(buf) ? remaining : sizeof(buf);

		if (fread(buf, 1, step, stack) != step) {
			fprintf(stderr, "failed to read from stack file\n");
			exit(EXIT_FAILURE);
		}
		if (fwrite(buf, 1, step, element) != step) {
			fprintf(stderr, "failed to write to element file %s\n", elementfile);
			exit(EXIT_FAILURE);
		}
		remaining -= step;
	}

	if (ftruncate(fileno(stack), stack_off) != 0) {
		fprintf(stderr, "failed to truncate stack file\n");
		exit(EXIT_FAILURE);
	}

	if (fclose(element) != 0) {
		perror("fclose");
		exit(EXIT_FAILURE);
	}

	if (fclose(stack) != 0) {
		perror("fclose");
		exit(EXIT_FAILURE);
	}
}

static void fcat(const char *stackfile, const char *elementindex)
{
	FILE *stack;
	unsigned long index;
	off_t element_size;
	uint64_t remaining, t64;

	errno = 0;
	index = strtoul(elementindex, NULL, 0);
	if (errno != 0) {
		perror("stroul");
		exit(EXIT_FAILURE);
	};

	stack = fopen(stackfile, "r");
	if (stack == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	if (fseeko(stack, 0, SEEK_END) == -1) {
		perror("fseeko");
		exit(EXIT_FAILURE);
	}

	element_size = 0;
	while (index > 0) {
		if (fseeko(stack, -sizeof(t64), SEEK_CUR) == -1) {
			perror("fseeko");
			exit(EXIT_FAILURE);
		}

		if (fread(&t64, sizeof(t64), 1, stack) != 1) {
			fprintf(stderr, "failed to read from stack file\n");
			exit(EXIT_FAILURE);
		}
		element_size = be64toh(t64);

		if (fseeko(stack, -(element_size + sizeof(t64)), SEEK_CUR) == -1) {
			perror("fseeko");
			exit(EXIT_FAILURE);
		}
		index--;
	}

	if (element_size == 0) {
		fprintf(stderr, "invalid element size\n");
		exit(EXIT_FAILURE);
	}

	remaining = element_size;
	while (remaining > 0) {
		char buf[4096];
		size_t step = remaining < sizeof(buf) ? remaining : sizeof(buf);

		if (fread(buf, 1, step, stack) != step) {
			fprintf(stderr, "failed to read from stack file\n");
			exit(EXIT_FAILURE);
		}
		if (fwrite(buf, 1, step, stdout) != step) {
			fprintf(stderr, "failed to write to stdout\n");
			exit(EXIT_FAILURE);
		}
		remaining -= step;
	}

	if (fclose(stack) != 0) {
		perror("fclose");
		exit(EXIT_FAILURE);
	}
}

static void fcheck(const char *stackfile, const char *elementcount)
{
	FILE *stack;
	off_t offset;
	unsigned long count;
	uint64_t t64;

	errno = 0;
	count = strtoul(elementcount, NULL, 0);
	if (errno != 0) {
		perror("stroul");
		exit(EXIT_FAILURE);
	};

	stack = fopen(stackfile, "r");
	if (stack == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	if (fseeko(stack, 0, SEEK_END) == -1) {
		perror("fseeko");
		exit(EXIT_FAILURE);
	}

	while (count > 0) {
		off_t element_size;

		if (fseeko(stack, -sizeof(t64), SEEK_CUR) == -1) {
			perror("fseeko");
			exit(EXIT_FAILURE);
		}

		if (fread(&t64, sizeof(t64), 1, stack) != 1) {
			fprintf(stderr, "failed to read from stack file\n");
			exit(EXIT_FAILURE);
		}
		element_size = be64toh(t64);
		if (element_size == 0) {
			fprintf(stderr, "invalid element size\n");
			exit(EXIT_FAILURE);
		}

		if (fseeko(stack, -(element_size + sizeof(t64)), SEEK_CUR) == -1) {
			perror("fseeko");
			exit(EXIT_FAILURE);
		}

		count--;
	}

	offset = ftello(stack);
	if (offset == -1) {
		perror("ftello");
		exit(EXIT_FAILURE);
	}
	if (offset != 0) {
		fprintf(stderr, "%" PRIdMAX " unused bytes remaining after %s element(s)\n", (intmax_t)offset, elementcount);
		exit(EXIT_FAILURE);
	} else {
		fprintf(stderr, "no unused bytes remaining after %s element(s)\n", elementcount);
	}

	if (fclose(stack) != 0) {
		perror("fclose");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	const char *stackfile, *command, *arg;

	if (argc < 3) {
		usage(argv[0]);
		exit(1);
	}
	stackfile = argv[1];
	command = argv[2];
	arg = argv[3];

	if (strcmp("push", command) == 0) {
		fpush(stackfile,  arg);
	} else if (strcmp("pop", command) == 0) {
		fpop(stackfile, arg);
	} else if (strcmp("cat", command) == 0) {
		fcat(stackfile, arg);
	} else if (strcmp("check", command) == 0) {
		fcheck(stackfile, arg);
	} else {
		usage(argv[0]);
		exit(1);
	}

	exit(EXIT_SUCCESS);
}
