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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <libgen.h>
#include <dlfcn.h>
#include <link.h>
#include <limits.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "embench.h"
#include "embench_impl.h"

#define	E_SUCCESS	0
#define	E_ERROR		1
#define	E_USAGE		2

static char *g_pname;
static char *g_dname;
static int g_nthreads;
static embench_func_t g_func;
static embench_prerun_t g_pref;
static embench_postrun_t g_postf;

/*
 * Wrapper function for the fact that other platforms don't have gethrtime.
 */
#ifndef __sun__
static hrtime_t
gethrtime(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	return (ts.tv_sec * NANOSEC + ts.tv_nsec);
}
#endif	/* !__sun__ */

static void
warn(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	(void) vfprintf(stderr, fmt, ap);
	va_end(ap);
}

static void
fatal(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	(void) vfprintf(stderr, fmt, ap);
	va_end(ap);

	exit(E_ERROR);
}

static int
run_tests(int nthreads)
{
	pthread_t *thr;
	embench_arg_t *data, *cur;
	void *status;
	int ii;

	thr = malloc(sizeof (pthread_t) * nthreads);
	if (thr == NULL)
		fatal("failed to alloc %d pthread_ts\n", nthreads);

	data = malloc(sizeof (embench_arg_t) * nthreads);
	if (data == NULL)
		fatal("failed to alloc memory to hold results\n");
	memset(data, '\0', sizeof (embench_arg_t) * nthreads);

	cur = data;
	for (ii = 0; ii < nthreads; ii++) {
		if (pthread_create(&thr[ii], NULL,
		    (void *(*)(void *))g_func, cur) != 0)
			fatal("failed to create thread: %s\n",
			    strerror(errno));
		cur++;
	}

	for (ii = 0; ii < nthreads; ii++) {
		assert(pthread_join(thr[ii], &status) == 0);
		assert(status == EMBENCH_SUCCESS);
	}
	free(thr);

	printf("%-4s  %-10s %-20s\n", "TID", "OPS", "TIME(ns)");
	cur = data;
	for (ii = 0; ii < nthreads; ii++)
		printf("%-4d  %-10d %-20lld\n", ii,
		    cur[ii].mb_nops, cur[ii].mb_total);
	free(data);

	return (0);
}

void
embench_timer_start(embench_arg_t *arg)
{
	arg->mb_tmp = gethrtime();
}

void
embench_timer_stop(embench_arg_t *arg)
{
	assert(arg->mb_tmp != 0);
	arg->mb_total += gethrtime() - arg->mb_tmp;
	arg->mb_tmp = 0;
}

void
embench_info_set_nops(embench_arg_t *arg, uint32_t ops)
{
	arg->mb_nops = ops;
}

static void
walk_directory(char *dpath)
{
	char path[1024], mpath[1024], *base;
	char **desc;
	DIR *d;
	void *dl;
	struct dirent *dp;
	int len;

	base = basename(dpath);
	d = opendir(dpath);
	if (d == NULL) {
		warn("failed to walk directory: %s\n", base);
		return;
	}

	while ((dp = readdir(d)) != NULL) {
		len = strlen(dp->d_name);
		if (len < 3)
			continue;

		if (strcmp(dp->d_name + len - 3, ".so") != 0)
			continue;

		snprintf(path, sizeof (path), "%s/%s", dpath, dp->d_name);
		dp->d_name[len - 3] = '\0';
		snprintf(mpath, sizeof (mpath), "%s/%s", base, dp->d_name);
		dl = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
		if (dl == NULL) {
			warn("unable to open %s: %s\n", path, dlerror());
			continue;
		}
		desc = dlsym(dl, "embench_desc");
		if (desc == NULL)
			warn("%-20s missing description\n", mpath);
		else
			printf("%-20s %s\n", mpath, *desc);
		(void) dlclose(dl);
	}
	(void) closedir(d);
}

static int
print_modules(void)
{
	char dir[1024], path[1024];
	DIR *modd;
	struct dirent *dp;
	struct stat sbuf;

	snprintf(dir, sizeof (dir), "%s/modules", g_dname);
	modd = opendir(dir);
	if (modd == NULL)
		fatal("failed to walk modules: %s\n", strerror(errno));

	printf("%-20s %s\n", "MODULE", "DESCRIPTION");
	while ((dp = readdir(modd)) != NULL) {
		if (strcmp(dp->d_name, ".") == 0 ||
		    strcmp(dp->d_name, "..") == 0)
			continue;

		(void) snprintf(path, sizeof (path), "%s/modules/%s",
		    g_dname, dp->d_name);
		if (stat(path, &sbuf) != 0) {
			warn("cannot descend into %s: %s\n", path,
			    strerror(errno));
			continue;
		}

		if (S_ISDIR(sbuf.st_mode))
			walk_directory(path);
	}

	(void) closedir(modd);

	return (0);
}

static int
usage(FILE *fp)
{
	(void) fprintf(fp, "Usage: %s [-m module] [-n nthreads] "
	    "[-h] [-l]\n\n", g_pname);
	(void) fprintf(fp, "\t-n nthreads:\t\tnumber of posts in parallel\n");
	(void) fprintf(fp, "\t-m module:\t\tname of the module\n");
	(void) fprintf(fp, "\t-l\t\t\tlist all modules\n");
	return (E_USAGE);
}

int
main(int argc, char *argv[])
{
	char *modulep;
	char c;
	char buf[PATH_MAX];
	void *mod;

	g_nthreads = 1;
	modulep = NULL;

	g_pname = basename(argv[0]);
	g_dname = dirname(argv[0]);

	if (argc == 1)
		return (usage(stderr));

	while ((c = getopt(argc, argv, "lhn:m:")) != EOF) {
		switch (c) {
		case 'n':
			g_nthreads = atoi(optarg);
			break;
		case 'm':
			modulep = optarg;
			break;
		case 'h':
			return (usage(stderr));
			break;
		case 'l':
			return (print_modules());
			break;
		default:
			return (usage(stderr));
		}
	}

	if (modulep == NULL)
		fatal("missing test module to run\n");

	snprintf(buf, sizeof (buf), "%s/modules/%s.so", g_dname, modulep);
	mod = dlopen(buf, RTLD_NOW | RTLD_LOCAL);
	if (mod == NULL)
		fatal("unable to open %s: %s\n", buf, dlerror());
	g_pref = dlsym(mod, "embench_prerun");
	g_postf = dlsym(mod, "embench_postrun");

	if (g_pref == NULL && g_postf != NULL)
		fatal("embench_postrun defined but not embench_prerun");

	if (g_pref != NULL && g_postf == NULL)
		fatal("embench_prerun defined but not embench_postrun");

	g_func = dlsym(mod, "embench_run");
	if (g_func == NULL)
		fatal("unable to find embench_run symbol\n");

	if (g_pref != NULL)
		if (g_pref() != EMBENCH_SUCCESS)
			fatal("embench_prerun failed\n");


	run_tests(g_nthreads);
	if (g_postf != NULL)
		g_postf();

	return (0);
}
