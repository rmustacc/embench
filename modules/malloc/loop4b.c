/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright (c) 2012, Joyent, Inc. All rights reserved.
 * Copyright (c) 2012, Robert Mustacchi. All rights reserved.
 */
/*
 * Malloc and free 4 byte buffers
 */

#include <stdlib.h>
#include <embench.h>

#define	MALLOC_ITERS	1024*1024	/* 1 M */
#define MALLOC_SIZE	4

const char *embench_desc = "malloc and free 4 B buffers in a loop";

void *
embench_run(embench_arg_t *arg)
{
	int ii;
	void *buf;
	embench_info_set_nops(arg, MALLOC_ITERS);
	embench_timer_start(arg);
	for (ii = 0; ii < MALLOC_ITERS; ii++) {
		buf = malloc(MALLOC_SIZE);
		if (buf == NULL)
			return (EMBENCH_FAILURE);
		free(buf);
	}
	embench_timer_stop(arg);
	return (EMBENCH_SUCCESS);
}
