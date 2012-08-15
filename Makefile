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

CSTYLE=/opt/onbld/bin/cstyle
HDRCHK=/opt/onbld/bin/hdrchk

all: amd64/Makefile i386/Makefile
	cd amd64 && $(MAKE)
	cd i386 && $(MAKE)

clean: amd64/Makefile i386/Makefile
	cd amd64 && $(MAKE) clean
	cd i386 && $(MAKE) clean

clobber: amd64/Makefile i386/Makefile
	cd amd64 && $(MAKE) clobber
	cd i386 && $(MAKE) clobber
	rm -f cscope.*

check:
	$(CSTYLE) *.c *.h
	$(HDRCHK) *.h

cscope.files:
	find . -type f -name '*.[ch]' > cscope.files

xref: cscope.files
	cscope-fast -bq
	

.PHONY: clean bench xref
