/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall hello.c `pkg-config fuse --cflags --libs` -o hello
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

static int hello_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, hello_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);
	} else
		res = -ENOENT;

	return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, hello_path + 1, NULL, 0);

	return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
	int fuse_info_size = sizeof(struct fuse_file_info);
	int flags_offset = offsetof(struct fuse_file_info, flags);
	int fh_old_offset = offsetof(struct fuse_file_info, fh_old);
	int writepage_offset = offsetof(struct fuse_file_info, writepage);
	int direct_io_offset = offsetof(struct fuse_file_info, direct_io);
	int keep_cache_offset = offsetof(struct fuse_file_info, keep_cache);
	int flush_offset = offsetof(struct fuse_file_info, flush);
	int nonseekable_offset = offsetof(struct fuse_file_info, nonseekable);
	int flock_release_offset  = offsetof(struct fuse_file_info, flock_release);
	int fh_offset = offsetof(struct fuse_file_info, fh);
	int lock_owner_offset = offsetof(struct fuse_file_info, lock_owner);
	printf("fuse info size %s flags offset %s fh_old_offset %s writepage %s direct_io %s keep_cache %s flush %s nonseekable %s flock release %s  fh %s lock owner %s \n",
			fuse_info_size, flags_offset, fh_old_offset,
			writepage_offset, direct_io_offset, keep_cache_offset, flush_offset,
			nonseekable_offset, flock_release_offset, fh_offset,
			lock_owner_offset);
	printf("fuse info size %s flags offset %s \n",
			fuse_info_size, flags_offset);
	if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;
	if(strcmp(path, hello_path) != 0)
		return -ENOENT;

	len = strlen(hello_str);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, hello_str + offset, size);
	} else
		size = 0;

	return size;
}

static struct fuse_operations hello_oper = {
	.getattr	= hello_getattr,
	.readdir	= hello_readdir,
	.open		= hello_open,
	.read		= hello_read,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &hello_oper, NULL);
}
