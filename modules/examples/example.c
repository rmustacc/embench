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
 * This is a sample plugin for embench. It doesn't do much of anything, but
 * shows you everything that is necessary to create a functioning module.
 */

/*
 * Every module needs to include <embench.h>. This is the API for writing
 * modules. It's rather sparse and there isn't much there. You're free to
 * include other header files as you need for your tests. The ordering here is
 * not important.
 */
#include <embench.h>

/*
 * Each test can define a number of times it does a specific operation. Often
 * times you will want to do something more than once to get a better sense of
 * its performance .
 */
#define	ITERS	1000

/*
 * This string is the description that shows up when you run embench -l. You do
 * not need to add a trailing newline.
 */
const char *embench_desc = "example plugin for embench";

/*
 * This is an optional entry point. This function will be called by the test
 * before the main embench_run entry point is called. This will be called by
 * only one thread. The purpose of this optional entry point is to set up
 * any global that the test might need: creating mutexes, opening file
 * descriptors, memory mapping something, etc.
 */
void *
embench_prerun(void)
{

	/*
	 * This function should return EMBENCH_SUCCESS to indicate that it has
	 * successfully finished its initialization. If however, some
	 * initialization failed to complete, you should instead print out an
	 * error message and return EMBENCH_FAILURE. This will cause a fatal
	 * error and the program will exit without running the test.
	 */
	return (EMBENCH_SUCCESS);
}

/*
 * This is a required entry point. It will be called into by multiple threads,
 * so the function should ensure that it only uses thread-safe interfaces.
 */
void *
embench_run(embench_arg_t *arg)
{
	int ii;

	/*
	 * This call can be done at any point during the function. Subsequent
	 * calls will overwrite the previous value. This sets the number of
	 * operations of the test that this thread will do.
	 *
	 * You should not call this while inside a section of
	 * embench_timer_start and embench_timer_stop. Otherwise the overhead
	 * will be included in the benchmark run time.
	 */
	embench_info_set_nops(arg, ITERS);

	/*
	 * Here you can do any preparatory work necessary for running your test.
	 * If you're testing File I/O, for example, here you would create your
	 * buffers and open the necessary file descriptors.
	 */

	/*
	 * A call to embench_timer_start takes a snapshot of the current time
	 * (via gethrtime()) for the test. For any given call to
	 * embench_timer_start, there must be a call to embench_timer_stop to
	 * note the end of the test.
	 *
	 * There can be multiple series of calls to embench_timer_start and
	 * embench_timer_stop in a given test. Each pair of calls adds to the
	 * current time.
	 */
	embench_timer_start(arg);

	/*
	 * Now that we are within an embench_timer_start section we should be
	 * running some portion of our test. Any preparatory work should have
	 * been done before calling embench_timer_start.
	 */

	for (ii = 0; ii < ITERS; ii++) {
		/*
		 * Here I would run the small test that I'm benchmarking.
		 */
	}

	/*
	 * This call tells us to stop the timer. Once you call this the test
	 * will diff the amount of time that has passed and add that to the
	 * thread's runtime. You must call this after you have called
	 * embench_timer_start.
	 */
	embench_timer_stop(arg);

	/*
	 * Here you should do any clean up work that was necessary for the
	 * benchmark but not what the benchmark was measuring. For example, if
	 * you opened a file to test file writes, now would be the time to close
	 * and unlink that file.
	 */

	/*
	 * Finally, you should return that the test succeeded. The correct way
	 * to do this is with EMBENCH_SUCCESS.
	 */
	return (EMBENCH_SUCCESS);
}

/*
 * This function will run after the test has terminated. Here you should go
 * through and clean up anything that was done during the embench_prerun
 * function. This entry point is not required if you did not define the
 * embench_prerun entry point.
 */
void
embench_postrun(void)
{
	return;
}
