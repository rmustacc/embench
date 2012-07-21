#
# CDDL HEADER START
#
# The contents of this file are subject to the terms of the
# Common Development and Distribution License (the "License").
# You may not use this file except in compliance with the License.
#
# You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
# or http://www.opensolaris.org/os/licensing.
# See the License for the specific language governing permissions
# and limitations under the License.
#
# When distributing Covered Code, include this CDDL HEADER in each
# file and include the License file at usr/src/OPENSOLARIS.LICENSE.
# If applicable, add the following below this CDDL HEADER, with the
# fields enclosed by brackets "[]" replaced with your own identifying
# information: Portions Copyright [yyyy] [name of copyright owner]
#
# CDDL HEADER END
#
# Copyright (c) 2012, Joyent, Inc. All rights reserved.
# Copyright (c) 2012, Robert Mustacchi. All rights reserved.
#

VPATH=..

PROGS=embench

PLUGINS_COMMON=modules/malloc/loop4b.so \
	modules/malloc/loop4k.so \
	modules/malloc/loop4m.so \
	modules/examples/example.so

PLUGINS_64=modules/malloc/loop4g.so

PLUGINS_32=

CC=gcc
CFLAGS += -Wall -Wextra -Werror -std=c99
CPPFLAGS += -I..  -D__EXTENSIONS__ -D_REENTRANT -D_XOPEN_SOURCE=600
SOFLAGS += -shared
LDFLAGS += -lc

all: $(PLUGINS) $(PROGS)

embench: ../embench.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(BINLDFLAGS) -o embench ../embench.c

%.o: %.c
	mkdir -p $$(dirname $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c $< -o $@

%.so: %.o
	$(CC) $(LDFLAGS) $(SOFLAGS) -o $@ $<

clean:
	rm -f $(PROGS) $(PLUGINS)

clobber: clean
	rm -f *.o
	rm -rf modules
