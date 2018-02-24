/*
 * Copyright (c) 2014 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <common.h>
#include <fs.h>
#include <fcntl.h>
#include <malloc.h>
#include <libfile.h>
#include <progress.h>
#include <linux/stat.h>

/*
 * write_full - write to filedescriptor
 *
 * Like write, but guarantees to write the full buffer out, else
 * it returns with an error.
 */
int write_full(int fd, void *buf, size_t size)
{
	size_t insize = size;
	int now;

	while (size) {
		now = write(fd, buf, size);
		if (now <= 0)
			return now;
		size -= now;
		buf += now;
	}

	return insize;
}
EXPORT_SYMBOL(write_full);

/*
 * read_full - read from filedescriptor
 *
 * Like read, but this function only returns less bytes than
 * requested when the end of file is reached.
 */
int read_full(int fd, void *buf, size_t size)
{
	size_t insize = size;
	int now;
	int total = 0;

	while (size) {
		now = read(fd, buf, size);
		if (now == 0)
			return total;
		if (now < 0)
			return now;
		total += now;
		size -= now;
		buf += now;
	}

	return insize;
}
EXPORT_SYMBOL(read_full);

/*
 * read_file_line - read a line from a file
 *
 * Used to compose a filename from a printf format and to read a line from this
 * file. All leading and trailing whitespaces (including line endings) are
 * removed. The returned buffer must be freed with free(). This function is
 * supposed for reading variable like content into a buffer, so files > 1024
 * bytes are ignored.
 */
char *read_file_line(const char *fmt, ...)
{
	va_list args;
	char *filename;
	char *buf, *line = NULL;
	size_t size;
	int ret;
	struct stat s;

	va_start(args, fmt);
	filename = vasprintf(fmt, args);
	va_end(args);

	ret = stat(filename, &s);
	if (ret)
		goto out;

	if (s.st_size > 1024)
		goto out;

	buf = read_file(filename, &size);
	if (!buf)
		goto out;

	line = strim(buf);

	line = xstrdup(line);
	free(buf);
out:
	free(filename);
	return line;
}
EXPORT_SYMBOL_GPL(read_file_line);

/**
 * read_file_2 - read a file to an allocated buffer
 * @filename:  The filename to read
 * @size:      After successful return contains the size of the file
 * @outbuf:    contains a pointer to the file data after successful return
 * @max_size:  The maximum size to read. Use FILESIZE_MAX for reading files
 *             of any size.
 *
 * This function reads a file to an allocated buffer. At maximum @max_size
 * bytes are read. The actual read size is returned in @size. -EFBIG is
 * returned if the file is bigger than @max_size, but the buffer is read
 * anyway up to @max_size in this case. Free the buffer with free() after
 * usage.
 *
 * Return: 0 for success, or negative error code. -EFBIG is returned
 * when the file has been bigger than max_size.
 */
int read_file_2(const char *filename, size_t *size, void **outbuf,
		loff_t max_size)
{
	int fd;
	struct stat s;
	void *buf = NULL;
	const char *tmpfile = "/.read_file_tmp";
	int ret;
	loff_t read_size;

again:
	ret = stat(filename, &s);
	if (ret)
		return ret;

	if (max_size == FILESIZE_MAX)
		read_size = s.st_size;
	else
		read_size = max_size;

	if (read_size == FILESIZE_MAX) {
		ret = copy_file(filename, tmpfile, 0);
		if (ret)
			return ret;
		filename = tmpfile;
		goto again;
	}

	buf = xzalloc(read_size + 1);

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		goto err_out;

	ret = read_full(fd, buf, read_size);
	if (ret < 0)
		goto err_out1;

	close(fd);

	if (size)
		*size = ret;

	if (filename == tmpfile)
		unlink(tmpfile);

	*outbuf = buf;

	if (read_size < s.st_size)
		return -EFBIG;
	else
		return 0;

err_out1:
	close(fd);
err_out:
	free(buf);

	if (filename == tmpfile)
		unlink(tmpfile);

	return ret;
}
EXPORT_SYMBOL(read_file_2);

/**
 * read_file - read a file to an allocated buffer
 * @filename:  The filename to read
 * @size:      After successful return contains the size of the file
 *
 * This function reads a file to an allocated buffer.
 * Some TFTP servers do not transfer the size of a file. In this case
 * a the file is first read to a temporary file.
 *
 * Return: The buffer conataining the file or NULL on failure
 */
void *read_file(const char *filename, size_t *size)
{
	int ret;
	void *buf;

	ret = read_file_2(filename, size, &buf, FILESIZE_MAX);
	if (!ret)
		return buf;

	return NULL;
}
EXPORT_SYMBOL(read_file);

/**
 * write_file - write a buffer to a file
 * @filename:    The filename to write
 * @size:        The size of the buffer
 *
 * Return: 0 for success or negative error value
 */
int write_file(const char *filename, void *buf, size_t size)
{
	int fd, ret;

	fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT);
	if (fd < 0)
		return fd;

	ret = write_full(fd, buf, size);

	close(fd);

	if (ret < 0)
		return ret;

	return 0;
}
EXPORT_SYMBOL(write_file);

/**
 * copy_file - Copy a file
 * @src:	The source filename
 * @dst:	The destination filename
 * @verbose:	if true, show a progression bar
 *
 * Return: 0 for success or negative error code
 */
int copy_file(const char *src, const char *dst, int verbose)
{
	char *rw_buf = NULL;
	int srcfd = 0, dstfd = 0;
	int r, w;
	int ret = 1;
	void *buf;
	int total = 0;
	struct stat statbuf;

	rw_buf = xmalloc(RW_BUF_SIZE);

	srcfd = open(src, O_RDONLY);
	if (srcfd < 0) {
		printf("could not open %s: %s\n", src, errno_str());
		goto out;
	}

	dstfd = open(dst, O_WRONLY | O_CREAT | O_TRUNC);
	if (dstfd < 0) {
		printf("could not open %s: %s\n", dst, errno_str());
		goto out;
	}

	if (verbose) {
		if (stat(src, &statbuf) < 0)
			statbuf.st_size = 0;

		init_progression_bar(statbuf.st_size);
	}

	while (1) {
		r = read(srcfd, rw_buf, RW_BUF_SIZE);
		if (r < 0) {
			perror("read");
			goto out;
		}
		if (!r)
			break;

		buf = rw_buf;
		while (r) {
			w = write(dstfd, buf, r);
			if (w < 0) {
				perror("write");
				goto out;
			}
			buf += w;
			r -= w;
			total += w;
		}

		if (verbose) {
			if (statbuf.st_size && statbuf.st_size != FILESIZE_MAX)
				show_progress(total);
			else
				show_progress(total / 16384);
		}
	}

	ret = 0;
out:
	if (verbose)
		putchar('\n');

	free(rw_buf);
	if (srcfd > 0)
		close(srcfd);
	if (dstfd > 0)
		close(dstfd);

	return ret;
}
EXPORT_SYMBOL(copy_file);
