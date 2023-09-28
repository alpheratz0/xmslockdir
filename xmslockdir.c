/*
	Copyright (C) 2022 <alpheratz99@protonmail.com>

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License version 2 as published by
	the Free Software Foundation.

	This program is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
	more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc., 59
	Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <xcb/xcb.h>

enum {
	XMSLOCKDIR_X,
	XMSLOCKDIR_Y
};

static void
die(const char *fmt, ...)
{
	va_list args;

	fputs("xmslockdir: ", stderr);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fputc('\n', stderr);
	exit(1);
}

static void
usage(void)
{
	puts("usage: xmslockdir [-hvxy]");
	exit(0);
}

static void
version(void)
{
	puts("xmslockdir version "VERSION);
	exit(0);
}

static xcb_point_t
x_get_pointer_position(xcb_connection_t *conn, xcb_screen_t *screen)
{
	xcb_point_t pos;
	xcb_query_pointer_cookie_t cookie;
	xcb_query_pointer_reply_t *reply;
	xcb_generic_error_t *error;

	cookie = xcb_query_pointer(conn, screen->root);
	reply = xcb_query_pointer_reply(conn, cookie, &error);

	if (NULL != error) {
		die("xcb_query_pointer failed with error code: %hhu",
				error->error_code);
	}

	pos.x = reply->root_x;
	pos.y = reply->root_y;

	free(reply);

	return pos;
}

static void
x_set_pointer_position(xcb_connection_t *conn,
                       xcb_screen_t *screen,
                       xcb_point_t *pos)
{
	xcb_warp_pointer(conn, XCB_NONE, screen->root,
			0, 0, 0, 0, pos->x, pos->y);
	xcb_flush(conn);
}

static void
xmslockdir(int lock_dir)
{
	xcb_connection_t *conn;
	xcb_screen_t *screen;
	xcb_point_t lock_pos, recv_pos, next_pos;

	if (xcb_connection_has_error(conn = xcb_connect(NULL, NULL)))
		die("can't open display");

	if (NULL == (screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data))
		die("can't get default screen");

	lock_pos = x_get_pointer_position(conn, screen);

	while (1) {
		usleep(20000);
		recv_pos = x_get_pointer_position(conn, screen);
		next_pos.x = lock_dir == XMSLOCKDIR_X ? recv_pos.x : lock_pos.x;
		next_pos.y = lock_dir == XMSLOCKDIR_Y ? recv_pos.y : lock_pos.y;
		if (next_pos.x != recv_pos.x || next_pos.y != recv_pos.y)
			x_set_pointer_position(conn, screen, &next_pos);
	}
}

static void
set_signal_handler(int sig, void (*handler)(int))
{
	struct sigaction sa;

	sigemptyset(&sa.sa_mask);

	sa.sa_flags = 0;
	sa.sa_handler = handler;

	if (sigaction(sig, &sa, NULL) == -1)
		die("couldn't set signal handler");
}

static void
h_sigint(int s)
{
	(void) s;
	exit(0);
}

int
main(int argc, char **argv)
{
	int lock_dir;

	lock_dir = XMSLOCKDIR_X;

	while (++argv, --argc > 0) {
		if ((*argv)[0] == '-' && (*argv)[1] != '\0' && (*argv)[2] == '\0') {
			switch ((*argv)[1]) {
			case 'h': usage(); break;
			case 'v': version(); break;
			case 'x': lock_dir = XMSLOCKDIR_X; break;
			case 'y': lock_dir = XMSLOCKDIR_Y; break;
			default: die("invalid option %s", *argv); break;
			}
		} else {
			die("unexpected argument: %s", *argv);
		}
	}

	set_signal_handler(SIGINT, h_sigint);
	xmslockdir(lock_dir);
	return 0;
}
