/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2001, 2002 Red Hat, Inc.
 *
 * Created by David Woodhouse <dwmw2@cambridge.redhat.com>
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id: compr_zlib.c,v 1.18 2002/05/20 14:56:37 dwmw2 Exp $
 *
 */

#ifndef __KERNEL__
#error "The userspace support got too messy and was removed. Update your mkfs.jffs2"
#endif

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/mtd/compatmac.h> /* for min() */
#include <linux/slab.h>
#include <linux/jffs2.h>
#include <linux/zlib_jffs2.h>
#include "nodelist.h"

	/* Plan: call deflate() with avail_in == *sourcelen, 
		avail_out = *dstlen - 12 and flush == Z_FINISH. 
		If it doesn't manage to finish,	call it again with
		avail_in == 0 and avail_out set to the remaining 12
		bytes for it to clean up. 
	   Q: Is 12 bytes sufficient?
	*/
#define STREAM_END_SPACE 12

static DECLARE_MUTEX(deflate_sem);
static DECLARE_MUTEX(inflate_sem);
static void *deflate_workspace;
static void *inflate_workspace;

int __init jffs2_zlib_init(void)
{
	deflate_workspace = vmalloc(zlib_deflate_workspacesize());
	if (!deflate_workspace) {
		printk(KERN_WARNING "Failed to allocate %d bytes for deflate workspace\n", zlib_deflate_workspacesize());
		return -ENOMEM;
	}
	D1(printk(KERN_DEBUG "Allocated %d bytes for deflate workspace\n", zlib_deflate_workspacesize()));
	inflate_workspace = vmalloc(zlib_inflate_workspacesize());
	if (!inflate_workspace) {
		printk(KERN_WARNING "Failed to allocate %d bytes for inflate workspace\n", zlib_inflate_workspacesize());
		vfree(deflate_workspace);
		return -ENOMEM;
	}
	D1(printk(KERN_DEBUG "Allocated %d bytes for inflate workspace\n", zlib_inflate_workspacesize()));
	return 0;
}

void jffs2_zlib_exit(void)
{
	vfree(deflate_workspace);
	vfree(inflate_workspace);
}

int jffs2_zlib_compress(unsigned char *data_in, unsigned char *cpage_out, 
		   uint32_t *sourcelen, uint32_t *dstlen)
{
	z_stream strm;
	int ret;

	if (*dstlen <= STREAM_END_SPACE)
		return -1;

	down(&deflate_sem);
	strm.workspace = deflate_workspace;

	if (Z_OK != zlib_deflateInit(&strm, 3)) {
		printk(KERN_WARNING "deflateInit failed\n");
		up(&deflate_sem);
		return -1;
	}

	strm.next_in = data_in;
	strm.total_in = 0;
	
	strm.next_out = cpage_out;
	strm.total_out = 0;

	while (strm.total_out < *dstlen - STREAM_END_SPACE && strm.total_in < *sourcelen) {
		strm.avail_out = *dstlen - (strm.total_out + STREAM_END_SPACE);
		strm.avail_in = min((unsigned)(*sourcelen-strm.total_in), strm.avail_out);
		D1(printk(KERN_DEBUG "calling deflate with avail_in %d, avail_out %d\n",
			  strm.avail_in, strm.avail_out));
		ret = zlib_deflate(&strm, Z_PARTIAL_FLUSH);
		D1(printk(KERN_DEBUG "deflate returned with avail_in %d, avail_out %d, total_in %ld, total_out %ld\n", 
			  strm.avail_in, strm.avail_out, strm.total_in, strm.total_out));
		if (ret != Z_OK) {
			D1(printk(KERN_DEBUG "deflate in loop returned %d\n", ret));
			zlib_deflateEnd(&strm);
			up(&deflate_sem);
			return -1;
		}
	}
	strm.avail_out += STREAM_END_SPACE;
	strm.avail_in = 0;
	ret = zlib_deflate(&strm, Z_FINISH);
	zlib_deflateEnd(&strm);
	up(&deflate_sem);
	if (ret != Z_STREAM_END) {
		D1(printk(KERN_DEBUG "final deflate returned %d\n", ret));
		return -1;
	}

	D1(printk(KERN_DEBUG "zlib compressed %ld bytes into %ld\n",
		  strm.total_in, strm.total_out));

	if (strm.total_out >= strm.total_in)
		return -1;

	*dstlen = strm.total_out;
	*sourcelen = strm.total_in;
	return 0;
}

void jffs2_zlib_decompress(unsigned char *data_in, unsigned char *cpage_out,
		      uint32_t srclen, uint32_t destlen)
{
	z_stream strm;
	int ret;

	down(&inflate_sem);
	strm.workspace = inflate_workspace;

	if (Z_OK != zlib_inflateInit(&strm)) {
		printk(KERN_WARNING "inflateInit failed\n");
		up(&inflate_sem);
		return;
	}
	strm.next_in = data_in;
	strm.avail_in = srclen;
	strm.total_in = 0;
	
	strm.next_out = cpage_out;
	strm.avail_out = destlen;
	strm.total_out = 0;

	while((ret = zlib_inflate(&strm, Z_FINISH)) == Z_OK)
		;
	if (ret != Z_STREAM_END) {
		printk(KERN_NOTICE "inflate returned %d\n", ret);
	}
	zlib_inflateEnd(&strm);
	up(&inflate_sem);
}
