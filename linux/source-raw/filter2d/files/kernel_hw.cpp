/*
 * Copyright (C) 2013 - 2016  Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 */

#include <cstring>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#define MAX_IMAGE_WIDTH  1920
#define MAX_IMAGE_HEIGHT 1080

#define X2DFILTER_BUFFER_SIZE (1024 * (1 << 12))
struct x2dfilter_config {
    unsigned short filter;
    unsigned short width;
    unsigned short height;
    unsigned short stride;
};

#define FILTER_IDENTITY           0
#define FILTER_BLUR               1
#define FILTER_MOTION_BLUR        2
#define FILTER_EDGES              3
#define FILTER_SHARPEN            4
#define FILTER_GAUSSIAN_SMOOTHING 5
#define FILTER_EMBOSS             6

#define IOCTL_AXICTRL_START              _IO(0, 0)
#define IOCTL_AXICTRL_ISDONE             _IO(0, 1)
#define IOCTL_AXICTRL_ISIDLE             _IO(0, 2)
#define IOCTL_AXICTRL_ISREADY            _IO(0, 3)
#define IOCTL_AXICTRL_CONTINUE           _IO(0, 4)
#define IOCTL_AXICTRL_ENABLEAUTORESTART  _IO(0, 5)
#define IOCTL_AXICTRL_DISABLEAUTORESTART _IO(0, 6)
#define IOCTL_X2DFILTER_CONFIG           _IOW(1, 0, struct x2dfilter_config)

#define DEVFILE "/dev/xfilter2dkernel"

void Filter2DKernel(unsigned short filter, unsigned short width,
                    unsigned short height, unsigned short stride,
                    const unsigned char *src, unsigned char *dst)
{

    int fd;
    int rc;
    unsigned char *buf_src;
    unsigned char *buf_dst;
    struct x2dfilter_config config {
        .filter = filter, .width = width, .height = height, .stride = stride
    };

    fd = open(DEVFILE, O_RDWR);
    if (fd < 0) {
        printf("Error opening device file!\n");
        return;
    }

    buf_src = (unsigned char *)mmap(NULL, X2DFILTER_BUFFER_SIZE,
                                    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buf_src == MAP_FAILED) {
        printf("Error mapping buffer!\n");
        return;
    }
    buf_dst = buf_src + (X2DFILTER_BUFFER_SIZE >> 1);

    memcpy(buf_src, src, stride * height);

    // Check Ready
    while (ioctl(fd, IOCTL_AXICTRL_ISIDLE) == 0)
        ;

    if ((rc = ioctl(fd, IOCTL_X2DFILTER_CONFIG, &config)) < 0) {
        printf("Error configuring device!\n");
        return;
    }

    // Start the accelerator
    ioctl(fd, IOCTL_AXICTRL_START);

    // Wait for the accelerator to finish
    while (ioctl(fd, IOCTL_AXICTRL_ISDONE) == 0)
        ;

    memcpy(dst, buf_dst, stride * height);
    munmap(buf_src, X2DFILTER_BUFFER_SIZE);

    ioctl(fd, IOCTL_AXICTRL_CONTINUE);

    close(fd);
}
