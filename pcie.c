/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
*/

#include <sys/ioctl.h>
#include <sys/types.h>
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <poll.h>

#include "qdl.h"

int qdl_open_pcie(struct qdl_device *qdl, const char *serial)
{
    int ret;
	if(NULL == serial)
    {
        warnx("serial port is not specified");
        return -1;
    }
    ret = open(serial, O_RDWR | O_SYNC);
    if(ret == -1)
    {
        warnx("Failed to open port (%d)", errno);
        return -1;
    }
    qdl->fd = ret;
    return 0;
}

int qdl_read_pcie(struct qdl_device *qdl, void *buf, size_t len, unsigned int timeout)
{
    int ret;
    struct pollfd fds[] = {{qdl->fd, POLLIN, 0}};

    ret = poll(fds, 1, timeout);
    if(ret <= 0)
    {
        warnx("qdl_read_pcie error : %d", ret);
        return -1;
    }
    else if(!(fds[0].revents & POLLIN))
    {
        warnx("qdl_read_pcie revents error : %d", fds[0].revents);
        return -1;
    }
	return read(qdl->fd, buf, len);
}

int qdl_write_pcie(struct qdl_device *qdl, const void *buf, size_t len)
{
	int ret;
    struct pollfd fds[] = {{qdl->fd, POLLOUT, 0}};
    unsigned char *data = (unsigned char*) buf;
	unsigned int count = 0;

    ret = poll(fds, 1, 1000);
    if(ret <= 0)
    {
        warnx("qdl_write_pcie error : %d", ret);
        return -1;
    }
    else if(!(fds[0].revents & POLLOUT))
    {
        warnx("qdl_write_pcie revents error : %d", fds[0].revents);
        return -1;
    }
    while (len > 0) {
        ret = write(qdl->fd, data, len);
        if(ret < 0)
        {
            warnx("qdl_write_pcie write failed : %d", ret);
            return -1;
        }
        count += ret;
        len -= ret;
        data += ret;
    }
	return count;
}
