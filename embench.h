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

#ifndef	_EMBENCH_H
#define	_EMBENCH_H

#include <sys/types.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Opaque structure for tests. Effectively a handle back to the framework.
 */
typedef struct embench_arg embench_arg_t;

typedef int(*embench_func_t)(embench_arg_t *);

void embench_timer_start(embench_arg_t *);
void embench_timer_stop(embench_arg_t *);
void embench_info_set_nops(embench_arg_t *, uint32_t);

#define	EMBENCH_SUCCESS	((void *)0)
#define	EMBENCH_FAILURE	((void *)1)

#ifdef	__cplusplus
}
#endif

#endif	/* _EMBENCH_H */
