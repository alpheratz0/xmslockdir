# Copyright (C) 2022 <alpheratz99@protonmail.com>
# This program is free software.

VERSION   = 0.0.1

CC        = cc
INCS      = -I/usr/X11R6/include
CFLAGS    = -pedantic -Wall -Wextra -Os $(INCS) -DVERSION=\"$(VERSION)\"
LDLIBS    = -lxcb
LDFLAGS   = -L/usr/X11R6/lib -s

PREFIX    = /usr/local
MANPREFIX = $(PREFIX)/share/man
