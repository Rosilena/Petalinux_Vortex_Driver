/*
* Copyright (C) 2013-2022  Xilinx, Inc.  All rights reserved.
* Copyright (c) 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
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
* IN NO EVENT SHALL XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in this
* Software without prior written authorization from Xilinx.
*
*/

#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include "blinky.h"

#define DEVICE_FILE_NAME "/dev/vortex"

struct data_ioctl {
	uint32_t dcr_data;
	uint32_t dcr_valaddr;
};

struct data_ioctl data_dcr;

#define IOCTL_READ	_IOR('k', 0, struct data_ioctl)
#define IOCTL_WRITE	_IOW('k', 1, struct data_ioctl)

int main(int argc, char **argv)
{
    printf("Hello World!\n");
    
    int fd;
    
    fd = open(DEVICE_FILE_NAME, O_RDWR);
    
    if (fd < 0)
    {
    	printf("Cannot open device\n");
	close(fd);
	    
	return 0;
    }
    else
    {
    	printf("Device opened\n");
    }
    
    printf("Loading vortex kernel into BRAM\n");
    ssize_t offset = 0x80000;
    pwrite(fd, blinky_bin, blinky_bin_len, offset);
    
    data_dcr.dcr_data = 0xA0080000;
    data_dcr.dcr_valaddr = (0x1 | 0x1<<12);
    ioctl(fd, IOCTL_WRITE, &data_dcr);
    
    unsigned char check[blinky_bin_len];
    pread(fd, check, blinky_bin_len, offset);
    
    for(int i=0; i<blinky_bin_len; i++){
    	if(check[i] != blinky_bin[i]){
    		printf("DIVERSO\n");
    	}
    }
    
    //ioctl(fd, IOCTL_READ, &data_dcr);
    close(fd);
    
    return 0;
}
