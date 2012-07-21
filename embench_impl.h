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

#ifndef	_EMBENCH_IMPL_H
#define	_EMBENCH_IMPL_H

#include <sys/types.h>
#include <sys/time.h>
#include <embench.h>

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef __sun__
#define	NANOSEC	1000000000
typedef long long hrtime_t;
#endif

/*
 * The following is an argument that is passed in as a pointer to routines that
 * are implementing the embench API. Users can call embench_timer_start and
 * embench_timer_stop to increment the runtime timer.
 */
struct embench_arg {
	hrtime_t	mb_total;
	hrtime_t	mb_tmp;
	uint32_t	mb_nops;
};

#ifdef	__cplusplus
}
#endif

#endif /* _EMBENCH_IMPL_H */
