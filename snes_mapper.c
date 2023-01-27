/*
 * Copyright (c) 2023 RMF <rawmonk@firemail.cc>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef __FreeBSD__
#warning incompatible operating system detected
#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <xcb/xcb.h>
#include <xcb/xtest.h>

#define SLEEP		5000 /* nanoseconds */

#define DPADY		0x000000FF00000000
#define DPADX		0x00000000FF000000
#define BTN_X		0x0000100000000000
#define BTN_Y		0x0000200000000000
#define BTN_A		0x0000400000000000
#define BTN_B		0x0000800000000000
#define BTN_L		0x0001000000000000
#define BTN_R		0x0002000000000000
#define BTN_START	0x0010000000000000
#define BTN_SELECT	0x0020000000000000

xcb_connection_t *conn;
int offset = 0;

void press(int k) {
        xcb_test_fake_input(conn, XCB_KEY_PRESS, k + offset, XCB_CURRENT_TIME,
                                XCB_NONE, 0, 0, 0);
        xcb_flush(conn);
}

void release(int k) {
        xcb_test_fake_input(conn, XCB_KEY_RELEASE, k + offset,
                                XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0);
        xcb_flush(conn);
}

void button(uint64_t now, uint64_t prev, uint64_t mask, int key) {
        if (now & mask && !(prev & mask))
                press(key);
        if (!(now & mask) && prev & mask)
                release(key);
}

void dpad(uint64_t now, uint64_t prev, uint64_t mask, int key) {
        if ((now & mask) == mask && (prev & mask) != mask)
                press(key);
        if ((now & mask) == 0 && (prev & mask) != 0)
                press(key + 1);
        if ((prev & mask) == mask && (now & mask) != mask)
                release(key);
        if ((prev & mask) == 0 && (now & mask) != 0)
                release(key + 1);
}

int main(int argc, char *argv[]) {

        uint64_t state = 0;
        int fd;

        if (argc < 2) {
                printf("%s <device> (/dev/uhid_snes0)\n", argv[0]);
                return -1;
        }

        conn = xcb_connect(NULL, NULL);
        if (xcb_connection_has_error(conn)) {
                printf("xcb cannot find X11\n");
                return -1;
        }

        fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
                printf("open: %s\n", strerror(errno));
                return -1;
        }
	/* allow mapping multiple controllers */
        offset = (argv[1][strlen(argv[1]) - 1] - '0') * 12;
	if (offset < 0) offset = 0;

        while (1) {
                uint64_t prev = state;
		const struct timespec ts = {0, SLEEP};
                if (read(fd, &state, sizeof(state)) != sizeof(state)) break;
                button(state, prev, BTN_X, 25);
                button(state, prev, BTN_Y, 26);
                button(state, prev, BTN_A, 27);
                button(state, prev, BTN_B, 28);
                button(state, prev, BTN_L, 30);
                button(state, prev, BTN_R, 31);
                button(state, prev, BTN_START, 32);
                button(state, prev, BTN_SELECT, 33);
                dpad(state, prev, DPADX, 34);
                dpad(state, prev, DPADY, 36);
		nanosleep(&ts, NULL);
        }

        close(fd);
        xcb_flush(conn);
        xcb_disconnect(conn);

        return 0;
}
